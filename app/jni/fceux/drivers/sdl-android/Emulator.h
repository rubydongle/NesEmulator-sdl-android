#ifndef NESEMULATOR_SDL_ANDROID_EMULATOR_H
#define NESEMULATOR_SDL_ANDROID_EMULATOR_H


#include "../common/configSys.h"
#include "../../git.h"
#include "VideoDriver.h"
#include "AudioDriver.h"

class Emulator {
public:
    Emulator();
    int loadGame(const char* path);
    int closeGame();
    void doFun();
    int driverInitialize(FCEUGI *gi);
    void driverKill();
    void setBaseDirectory(const char* path);
    void updateEmulateData(uint8 *xbuf, int32 *sbuf, int scount);

private:
    int isPal;
    int frameSkip = 0;
    int periodicSaves = 0;

    int videoInited = 0;
    int audioInited = 0;
    int joyStickInited = 0;
    int eOptions;

    VideoDriver *videoDriver;
    AudioDriver *audioDriver;

};


#endif //NESEMULATOR_SDL_ANDROID_EMULATOR_H
