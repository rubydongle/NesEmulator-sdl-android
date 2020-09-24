#include "AudioDriver.h"
#include "../common/configSys.h"
#include "../../driver.h"
#include "../../utils/memory.h"
#include "../../../sdl/include/SDL_audio.h"

extern Config *g_config;

static volatile int *s_Buffer = 0;
static unsigned int s_BufferSize;
static unsigned int s_BufferRead;
static unsigned int s_BufferWrite;
static volatile unsigned int s_BufferIn;

static int s_mute = 0;
static void fillaudio(void *udata, uint8 *stream, int len) {
    int16 *tmps = (int16*)stream;
    len >>= 1;
    while(len) {
        int16 sample = 0;
        if(s_BufferIn) {
            sample = s_Buffer[s_BufferRead];
            s_BufferRead = (s_BufferRead + 1) % s_BufferSize;
            s_BufferIn--;
        } else {
            sample = 0;
        }

        *tmps = sample;
        tmps++;
        len--;
    }
}

int AudioDriver::initAudio() {
    int sound, soundrate, soundbufsize, soundvolume, soundtrianglevolume, soundsquare1volume, soundsquare2volume, soundnoisevolume, soundpcmvolume, soundq;
    SDL_AudioSpec spec;

    g_config->getOption("SDL.Sound", &sound);
    if(!sound) {
        return 0;
    }

    memset(&spec, 0, sizeof(spec));
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        puts(SDL_GetError());
        killAudio();
        return 0;
    }
    char driverName[8];

    // load configuration variables
    g_config->getOption("SDL.Sound.Rate", &soundrate);
    g_config->getOption("SDL.Sound.BufSize", &soundbufsize);
    g_config->getOption("SDL.Sound.Volume", &soundvolume);
    g_config->getOption("SDL.Sound.Quality", &soundq);
    g_config->getOption("SDL.Sound.TriangleVolume", &soundtrianglevolume);
    g_config->getOption("SDL.Sound.Square1Volume", &soundsquare1volume);
    g_config->getOption("SDL.Sound.Square2Volume", &soundsquare2volume);
    g_config->getOption("SDL.Sound.NoiseVolume", &soundnoisevolume);
    g_config->getOption("SDL.Sound.PCMVolume", &soundpcmvolume);

    spec.freq = soundrate;
    spec.format = AUDIO_S16SYS;
    spec.channels = 1;
    spec.samples = 512;
    spec.callback = fillaudio;
    spec.userdata = 0;

    s_BufferSize = soundbufsize * soundrate / 1000;

    // For safety, set a bare minimum:
    if (s_BufferSize < spec.samples * 2)
        s_BufferSize = spec.samples * 2;

    s_Buffer = (int *)FCEU_dmalloc(sizeof(int) * s_BufferSize);
    if (!s_Buffer)
        return 0;
    s_BufferRead = s_BufferWrite = s_BufferIn = 0;

    if(SDL_OpenAudio(&spec, 0) < 0)
    {
        puts(SDL_GetError());
        killAudio();
        return 0;
    }
    SDL_PauseAudio(0);

    FCEUI_SetSoundVolume(soundvolume);
    FCEUI_SetSoundQuality(soundq);
    FCEUI_Sound(soundrate);
    FCEUI_SetTriangleVolume(soundtrianglevolume);
    FCEUI_SetSquare1Volume(soundsquare1volume);
    FCEUI_SetSquare2Volume(soundsquare2volume);
    FCEUI_SetNoiseVolume(soundnoisevolume);
    FCEUI_SetPCMVolume(soundpcmvolume);
    return 1;
}

int AudioDriver::killAudio() {
    FCEUI_Sound(0);
    SDL_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    if(s_Buffer) {
        free((void *)s_Buffer);
        s_Buffer = 0;
    }
    return 0;
}

uint32 AudioDriver::getMaxSound(void)
{
    return(s_BufferSize);
}

/**
 * Returns the amount of free space in the audio buffer.
 */
uint32 AudioDriver::getWriteSound(void)
{
    return(s_BufferSize - s_BufferIn);
}

/**
 * Send a sound clip to the audio subsystem.
 */
void AudioDriver::writeSound(int32 *buf, int Count)
{
    extern int EmulationPaused;
    if (EmulationPaused == 0)
        while(Count)
        {
            while(s_BufferIn == s_BufferSize)
            {
                SDL_Delay(1);
            }

            s_Buffer[s_BufferWrite] = *buf;
            Count--;
            s_BufferWrite = (s_BufferWrite + 1) % s_BufferSize;

            SDL_LockAudio();
            s_BufferIn++;
            SDL_UnlockAudio();

            buf++;
        }
}

/**
 * Pause (1) or unpause (0) the audio output.
 */
void AudioDriver::silenceSound(int n)
{
    SDL_PauseAudio(n);
}

void FCEUD_SoundVolumeAdjust(int n) {
    int soundvolume;
    g_config->getOption("SDL.Sound.Volume", &soundvolume);

    switch(n) {
        case -1:
            soundvolume -= 10;
            if(soundvolume < 0) {
                soundvolume = 0;
            }
            break;
        case 0:
            soundvolume = 100;
            break;
        case 1:
            soundvolume += 10;
            if(soundvolume > 150) {
                soundvolume = 150;
            }
            break;
    }

    s_mute = 0;
    FCEUI_SetSoundVolume(soundvolume);
    g_config->setOption("SDL.Sound.Volume", soundvolume);

    FCEU_DispMessage("Sound volume %d.",0, soundvolume);
}

/**
 * Toggles the sound on or off.
 */
void FCEUD_SoundToggle(void) {
    if(s_mute) {
        int soundvolume;
        g_config->getOption("SDL.SoundVolume", &soundvolume);

        s_mute = 0;
        FCEUI_SetSoundVolume(soundvolume);
        FCEU_DispMessage("Sound mute off.",0);
    } else {
        s_mute = 1;
        FCEUI_SetSoundVolume(0);
        FCEU_DispMessage("Sound mute on.",0);
    }
}
