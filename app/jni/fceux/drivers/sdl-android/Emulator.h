#ifndef NESEMULATOR_SDL_ANDROID_EMULATOR_H
#define NESEMULATOR_SDL_ANDROID_EMULATOR_H


#include "../common/configSys.h"
#include "../../git.h"

class Emulator {
public:
    Emulator();
    int loadGame(const char* path);
    int loadGame(int argc, char **argv);
    int closeGame();
    void doFun();

//    FCEUGI* getGameInfo() {
//        return gameInfo;
//    };

//    Config* getConfig() {
//        return gConfig;
//    };

private:
//    Config *gConfig;
//    FCEUGI *gameInfo;
    int isPal;
//    int inited = 0;
    int frameSkip = 0;
    int periodicSaves = 0;

    int driverInitialize(FCEUGI *gi);
    void driverKill();
};


#endif //NESEMULATOR_SDL_ANDROID_EMULATOR_H
