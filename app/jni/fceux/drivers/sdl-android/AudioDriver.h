#ifndef NESEMULATOR_SDL_ANDROID_AUDIODRIVER_H
#define NESEMULATOR_SDL_ANDROID_AUDIODRIVER_H
#include <SDL.h>
#include "../../types.h"

class AudioDriver {
public:
    int initAudio();
    int killAudio();
    uint32 getMaxSound(void);
    uint32 getWriteSound(void);
    void writeSound(int32 *buf, int Count);
    void silenceSound(int n);
};

#endif //NESEMULATOR_SDL_ANDROID_AUDIODRIVER_H
