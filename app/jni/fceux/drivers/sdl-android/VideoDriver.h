#ifndef NESEMULATOR_SDL_ANDROID_VIDEODRIVER_H
#define NESEMULATOR_SDL_ANDROID_VIDEODRIVER_H

#include "../../types.h"
#include "../../git.h"
#include <SDL.h>

#define WIDTH 256
#define HEIGHT_PAL 240
#define HEIGHT_NTSC 224

static SDL_Color s_psdl[256];

class VideoDriver {
public:
    void render(uint8* XBuf);
    int initVideo(FCEUGI *gi);

private:
    SDL_Window *s_window;
    SDL_Renderer *s_renderer;
    SDL_Texture *s_texture;
    int s_srendline, s_erendline, s_tlines;
    int s_inited;
};

#endif //NESEMULATOR_SDL_ANDROID_VIDEODRIVER_H
