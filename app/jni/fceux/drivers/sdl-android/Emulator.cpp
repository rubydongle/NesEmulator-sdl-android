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

	videoDriver = new VideoDriver();
	audioDriver = new AudioDriver();
    SDL_Log("Emulator constructor done");
}

int Emulator::driverInitialize(FCEUGI *gi)
{
//    if(InitVideo(gi) < 0) return 0;
    if(videoDriver->initVideo(gi) < 0) return 0;
    driverInited|=4;
    videoInited = 1;

    if(audioDriver->initAudio()) {
//    if(InitSound()) {
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
        videoDriver->killVideo();
        videoInited = 0;
    }

    if(audioInited) {
        audioDriver->killAudio();
//        KillSound();
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
        audioDriver->silenceSound(opause);
//        SilenceSound(opause);
    }
}

extern double g_fpsScale;
void Emulator::updateEmulateData(uint8 *xbuf, int32 *sbuf, int scount) {
    extern int FCEUDnetplay;
    int ocount = scount;
    // apply frame scaling to Count
    scount = (int)(scount / g_fpsScale);
    if(scount) {
        int32 can=audioDriver->getWriteSound();//GetWriteSound();
        static int uflow=0;
        int32 tmpcan;

        // don't underflow when scaling fps
        if(can >= audioDriver->getMaxSound()/*GetMaxSound()*/ && g_fpsScale==1.0) uflow=1;	/* Go into massive underflow mode. */

        if(can > scount) can=scount;
        else uflow=0;

        audioDriver->writeSound(sbuf, can);
        //if(uflow) puts("Underflow");
        tmpcan = audioDriver->getWriteSound();
        // don't underflow when scaling fps
        if(g_fpsScale>1.0 || ((tmpcan < scount * 0.90) && !uflow)) {
            if(xbuf && videoInited && !(NoWaiting & 2))
                videoDriver->render(xbuf);
//                BlitScreen(xbuf);
            sbuf+=can;
            scount-=can;
            if(scount) {
                if(NoWaiting) {
                    can=audioDriver->getWriteSound();
                    if(scount > can) scount=can;
                    audioDriver->writeSound(sbuf, scount);
                } else {
                    while(scount > 0) {
                        audioDriver->writeSound(sbuf, (scount < ocount) ? scount : ocount);
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
            videoDriver->render(xbuf);
//            BlitScreen(xbuf);
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

// dummy functions
#define DUMMY(__f) \
    void __f(void) {\
        printf("%s\n", #__f);\
        FCEU_DispMessage("Not implemented.",0);\
    }
DUMMY(FCEUD_HideMenuToggle)
DUMMY(FCEUD_MovieReplayFrom)
DUMMY(FCEUD_ToggleStatusIcon)
DUMMY(FCEUD_AviRecordTo)
DUMMY(FCEUD_AviStop)
void FCEUI_AviVideoUpdate(const unsigned char* buffer) { }
int FCEUD_ShowStatusIcon(void) {return 0;}
bool FCEUI_AviIsRecording(void) {return false;}
void FCEUI_UseInputPreset(int preset) { }
bool FCEUD_PauseAfterPlayback() { return false; }
// These are actually fine, but will be unused and overriden by the current UI code.
void FCEUD_TurboOn	(void) { NoWaiting|= 1; }
void FCEUD_TurboOff   (void) { NoWaiting&=~1; }
void FCEUD_TurboToggle(void) { NoWaiting^= 1; }
FCEUFILE* FCEUD_OpenArchiveIndex(ArchiveScanRecord& asr, std::string &fname, int innerIndex) { return 0; }
FCEUFILE* FCEUD_OpenArchive(ArchiveScanRecord& asr, std::string& fname, std::string* innerFilename) { return 0; }
FCEUFILE* FCEUD_OpenArchiveIndex(ArchiveScanRecord& asr, std::string &fname, int innerIndex, int* userCancel) { return 0; }
FCEUFILE* FCEUD_OpenArchive(ArchiveScanRecord& asr, std::string& fname, std::string* innerFilename, int* userCancel) { return 0; }
ArchiveScanRecord FCEUD_ScanArchive(std::string fname) { return ArchiveScanRecord(); }
void FCEUD_DebugBreakpoint() {return;}
void FCEUD_TraceInstruction() {return;}
