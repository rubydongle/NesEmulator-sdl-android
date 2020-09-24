#include "Emulator.h"
#include "main.h"
#include "config.h"
#include "../../fceu.h"
#include "../../driver.h"
#include "input.h"
#include "SDL.h"
#include "throttle.h"

extern Config* g_config;

#define WIDTH 256
#define HEIGHT_PAL 240
#define HEIGHT_NTSC 224

Emulator::Emulator() {
    SDL_Log("Emulator start constructor");
    g_config = InitConfig();//gConfig;
	UpdateInput(g_config);
	UpdateEMUCore(g_config);
	setHotKeys(g_config);
    SDL_Log("Emulator constructor done");
}

int Emulator::driverInitialize(FCEUGI *gi)
{
    if(InitVideo(gi) < 0) return 0;
    driverInited|=4;
    videoInited = 1;

    if(InitSound()) {
        driverInited|=1;
        audioInited = 1;

    }

    if(InitJoysticks()) {
        driverInited|=2;
        joyStickInited = 1;
    }

    int fourscore=0;
    g_config->getOption("SDL.FourScore", &fourscore);
    eoptions &= ~EO_FOURSCORE;
    eOptions &= ~EO_FOURSCORE;
    if(fourscore) {
        eoptions |= EO_FOURSCORE;
        eOptions |= EO_FOURSCORE;
    }

    InitInputInterface();
    return 1;
}

void Emulator::driverKill()
{
//    if (!noconfig)
//        g_config->save();

//    if(driverInited&2)
//        KillJoysticks();
//    if(driverInited&4)
//        KillVideo();
//    if(driverInited&1)
//        KillSound();

    if(joyStickInited) {
        KillJoysticks();
        joyStickInited = 0;
    }

    if(videoInited) {
        KillVideo();
        videoInited = 0;
    }

    if(audioInited) {
        KillSound();
        audioInited = 0;
    }
    driverInited=0;
}

void Emulator::setBaseDirectory(const char *path) {
    FCEUI_SetBaseDirectory(path);
}

int Emulator::loadGame(const char *path)
{
    if (GameInfo) {//isloaded){
        closeGame();
    }
    if(!FCEUI_LoadGame(path, 0)) {
        return 0;
    }
    FCEUI_SetVidSystem(isPal ? 1 : 0);

    int state_to_load;
    g_config->getOption("SDL.AutoLoadState", &state_to_load);
    if (state_to_load >= 0 && state_to_load < 10){
        FCEUI_SelectState(state_to_load, 0);
        FCEUI_LoadState(NULL, false);
    }

    ParseGIInput(GameInfo);
    RefreshThrottleFPS();

    if(!driverInitialize(GameInfo)) {
        return(0);
    }

    // set pal/ntsc
//    int id;
//    g_config->getOption("SDL.PAL", &id);
//    FCEUI_SetRegion(id);

//    g_config->getOption("SDL.SwapDuty", &id);
//    swapDuty = id;

    std::string filename;
    g_config->getOption("SDL.Sound.RecordFile", &filename);
    if(filename.size()) {
        if(!FCEUI_BeginWaveRecord(filename.c_str())) {
            g_config->setOption("SDL.Sound.RecordFile", "");
        }
    }
//	isloaded = 1;

    FCEUD_NetworkConnect();
    return 1;
}

void Emulator::doFun()//int frameskip, int periodic_saves)
{
    SDL_Log("%s", __FUNCTION__);
    uint8 *gfx;
    int32 *sound;
    int32 ssize;
    static int fskipc = 0;
    static int opause = 0;

    int frameskip = 0;
    int periodic_saves = 0;

    //TODO peroidic saves, working on it right now
    if (periodic_saves && FCEUD_GetTime() % PERIODIC_SAVE_INTERVAL < 30){
        FCEUI_SaveState(NULL, false);
    }
#ifdef FRAMESKIP
    fskipc = (fskipc + 1) % (frameskip + 1);
#endif

    if(NoWaiting) {
        gfx = 0;
    }
    FCEUI_Emulate(&gfx, &sound, &ssize, fskipc);
    updateEmulateData(gfx, sound, ssize);
//    FCEUD_Update(gfx, sound, ssize);

    if(opause!=FCEUI_EmulationPaused()) {
        opause=FCEUI_EmulationPaused();
        SilenceSound(opause);
    }
}

extern double g_fpsScale;
void Emulator::updateEmulateData(uint8 *xbuf, int32 *sbuf, int scount) {
    extern int FCEUDnetplay;
    int ocount = scount;
    // apply frame scaling to Count
    scount = (int)(scount / g_fpsScale);
    if(scount) {
        int32 can=GetWriteSound();
        static int uflow=0;
        int32 tmpcan;

        // don't underflow when scaling fps
        if(can >= GetMaxSound() && g_fpsScale==1.0) uflow=1;	/* Go into massive underflow mode. */

        if(can > scount) can=scount;
        else uflow=0;

        WriteSound(sbuf, can);
        //if(uflow) puts("Underflow");
        tmpcan = GetWriteSound();
        // don't underflow when scaling fps
        if(g_fpsScale>1.0 || ((tmpcan < scount * 0.90) && !uflow)) {
            if(xbuf && videoInited && !(NoWaiting & 2))
                BlitScreen(xbuf);
            sbuf+=can;
            scount-=can;
            if(scount) {
                if(NoWaiting) {
                    can=GetWriteSound();
                    if(scount > can) scount=can;
                    WriteSound(sbuf, scount);
                } else {
                    while(scount > 0) {
                        WriteSound(sbuf, (scount < ocount) ? scount : ocount);
                        scount -= ocount;
                    }
                }
            }
        } //else puts("Skipped");
//        else if(!NoWaiting && FCEUDnetplay && (uflow || tmpcan >= (scount * 1.8))) {
//            if(scount > tmpcan) scount=tmpcan;
//            while(tmpcan > 0) {
//                //	printf("Overwrite: %d\n", (Count <= tmpcan)?Count : tmpcan);
//                WriteSound(sbuf, (scount <= tmpcan) ? scount : tmpcan);
//                tmpcan -= scount;
//            }
//        }

    } else {
        if(!NoWaiting && (!(eoptions&EO_NOTHROTTLE) || FCEUI_EmulationPaused()))
            while (SpeedThrottle())
            {
                FCEUD_UpdateInput();
            }
        if(xbuf && videoInited) {
            BlitScreen(xbuf);
        }
    }
    FCEUD_UpdateInput();
}

int Emulator::closeGame() {
    std::string filename;

    if(!GameInfo) {//isloaded) {
        return(0);
    }

    int state_to_save;
    g_config->getOption("SDL.AutoSaveState", &state_to_save);
    if (state_to_save < 10 && state_to_save >= 0){
        FCEUI_SelectState(state_to_save, 0);
        FCEUI_SaveState(NULL, false);
    }
    FCEUI_CloseGame();

    driverKill();
//	isloaded = 0;
    GameInfo = 0;

    g_config->getOption("SDL.Sound.RecordFile", &filename);
    if(filename.size()) {
        FCEUI_EndWaveRecord();
    }

    InputUserActiveFix();
    return(1);
}
