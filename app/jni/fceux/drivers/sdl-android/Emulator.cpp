#include "Emulator.h"
#include "main.h"
#include "config.h"
#include "../../fceu.h"
#include "../../driver.h"
#include "input.h"
#include "SDL.h"
#include "throttle.h"

//#include <unistd.h>

extern Config* g_config;

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

    if(InitSound())
        driverInited|=1;

    if(InitJoysticks())
        driverInited|=2;

    int fourscore=0;
    g_config->getOption("SDL.FourScore", &fourscore);
    eoptions &= ~EO_FOURSCORE;
    if(fourscore)
        eoptions |= EO_FOURSCORE;

    InitInputInterface();
    return 1;
}

void Emulator::driverKill()
{
//    if (!noconfig)
//        g_config->save();

    if(driverInited&2)
        KillJoysticks();
    if(driverInited&4)
        KillVideo();
    if(driverInited&1)
        KillSound();
    driverInited=0;
}

int Emulator::loadGame(const char *path)
{
    if (GameInfo) {//isloaded){
        closeGame();
    }
    if(!FCEUI_LoadGame(path, 0)) {
        return 0;
    }

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
    int id;
    g_config->getOption("SDL.PAL", &id);
    FCEUI_SetRegion(id);

    g_config->getOption("SDL.SwapDuty", &id);
    swapDuty = id;

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

int Emulator::loadGame(int argc, char** argv)
{
    int romIndex = g_config->parse(argc, argv);
    const char* path = argv[romIndex];
    if (GameInfo){
        closeGame();
    }
    if (!FCEUI_LoadGame(path, 0)) {
        return 0;
    }

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
    int id;
    g_config->getOption("SDL.PAL", &id);
    FCEUI_SetRegion(id);

    g_config->getOption("SDL.SwapDuty", &id);
    swapDuty = id;

    std::string filename;
    g_config->getOption("SDL.Sound.RecordFile", &filename);
    if(filename.size()) {
        if(!FCEUI_BeginWaveRecord(filename.c_str())) {
            g_config->setOption("SDL.Sound.RecordFile", "");
        }
    }

    FCEUD_NetworkConnect();
    return 1;
}

//int XloadGame(int argc, char** argv)//const char *path)
//{
//    int romIndex = gConfig->parse(argc, argv);
//    const char* path = argv[romIndex];
//
//    if (gameInfo){
//        closeGame();
//    }
//
//    gameInfo = FCEUI_LoadGame(path, 0);
//    if(!gameInfo) {
//        return 0;
//    }
//
////    int state_to_load;
////    g_config->getOption("SDL.AutoLoadState", &state_to_load);
////    if (state_to_load >= 0 && state_to_load < 10){
////        FCEUI_SelectState(state_to_load, 0);
////        FCEUI_LoadState(NULL, false);
////    }
//
//    ParseGIInput(GameInfo);//gameInfo);
//    RefreshThrottleFPS();
//
//    if(!driverInitialize()) {
//        return(0);
//    }
//
//    // set pal/ntsc
////    FCEUI_SetVidSystem(isPal ? 1 : 0);
//    int id;
//    gConfig->getOption("SDL.PAL", &id);
//    FCEUI_SetRegion(id);
//
//    gConfig->getOption("SDL.SwapDuty", &id);
//    swapDuty = id;
//
////    std::string filename;
////    g_config->getOption("SDL.Sound.RecordFile", &filename);
////    if(filename.size()) {
////        if(!FCEUI_BeginWaveRecord(filename.c_str())) {
////            g_config->setOption("SDL.Sound.RecordFile", "");
////        }
////    }
//
////    FCEUD_NetworkConnect();
//    return 1;
//}
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
    FCEUD_Update(gfx, sound, ssize);

    if(opause!=FCEUI_EmulationPaused()) {
        opause=FCEUI_EmulationPaused();
        SilenceSound(opause);
    }
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
