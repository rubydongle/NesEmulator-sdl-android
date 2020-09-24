#include "VideoDriver.h"
#include "../../driver.h"

int VideoDriver::initVideo(FCEUGI *gi)
{
    int error;
    // 创建一个窗口
    s_window = SDL_CreateWindow("fceux game window" , SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_SHOWN);
//    FCEUI_SetShowFPS(1);
//	 创建一个渲染器
    uint32_t baseFlags = 0;//vsyncEnabled ? SDL_RENDERER_PRESENTVSYNC : 0;
    s_renderer = SDL_CreateRenderer(s_window, -1, baseFlags | SDL_RENDERER_ACCELERATED);
    if (s_renderer == NULL)
    {
        SDL_Log("[SDL] Failed to create accelerated renderer.\n");
        SDL_Log("[SDL] Attempting to create software renderer...\n");
        s_renderer = SDL_CreateRenderer(s_window, -1, baseFlags | SDL_RENDERER_SOFTWARE);

        if (s_renderer == NULL)
        {
            SDL_Log("[SDL] Failed to create software renderer.\n");
            return -1;
        }
    }

    FCEUI_GetCurrentVidSystem(&s_srendline, &s_erendline);
    s_tlines = s_erendline - s_srendline + 1;

    s_texture = SDL_CreateTexture(s_renderer,
                                  SDL_PIXELFORMAT_ARGB8888,
                                  SDL_TEXTUREACCESS_STREAMING,
                                  WIDTH, s_tlines);//HEIGHT_NTSC);

    SDL_SetRenderDrawColor(s_renderer, 200, 100, 100, 255);


    if (!SDL_WasInit(SDL_INIT_VIDEO)) {
        error = SDL_InitSubSystem(SDL_INIT_VIDEO);
        if (error) {
            FCEUD_PrintError(SDL_GetError());
            return -1;
        }
    }
    s_inited = 1;

    return 0;
}

void VideoDriver::render(uint8 *XBuf) {
    XBuf += s_srendline * 256;
    SDL_RenderClear(s_renderer);
    //-------------------------
    uint8* texture_buffer;
    int rowPitch;
    SDL_LockTexture(s_texture, nullptr, (void**)&texture_buffer, &rowPitch);

    unsigned char *buf = XBuf;
    int inOffset = 0;
    int outOffset = 0;

    for (int y = s_tlines; y > 0; y--) {
        int* data = (int*)texture_buffer;
        for (int x = WIDTH; x > 0; x--) {
            SDL_Color color = s_psdl[buf[inOffset++]];
#ifdef LSB_FIRST
            data[outOffset++] = ((uint32) 0xFF000000) | (color.b << 16) | (color.g << 8) | (color.r << 0);
#else
            data[outOffset++] = ((uint32) 0xFF000000) | (color.r << 16) | (color.g << 8) | (color.b << 0);
#endif

        }
    }

    SDL_UnlockTexture(s_texture);

    //------------------------
    SDL_Rect sdlRect;
    int xres, yres;
    SDL_GetWindowSize(s_window, &xres, &yres);
    sdlRect.w = (WIDTH * yres)/s_tlines;//scale;
    sdlRect.h = yres;//s_tlines * scale;
    sdlRect.y = 0;
    sdlRect.x = (xres - sdlRect.w) / 2;

    SDL_RenderCopy(s_renderer, s_texture, 0, &sdlRect);//&destRect);
    SDL_RenderPresent(s_renderer);
}

int VideoDriver::killVideo() {
	if(s_inited == 0)
		return -1;

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	s_inited = 0;
	return 0;
}

/**
 * Sets the color for a particular index in the palette.
 */
void FCEUD_SetPalette(uint8 index, uint8 r, uint8 g, uint8 b)
{
    s_psdl[index].r = r;
    s_psdl[index].g = g;
    s_psdl[index].b = b;
}

/**
 * Gets the color for a particular index in the palette.
 */
void FCEUD_GetPalette(uint8 index, uint8 *r, uint8 *g, uint8 *b) {
    *r = s_psdl[index].r;
    *g = s_psdl[index].g;
    *b = s_psdl[index].b;
}

bool enableHUDrecording = false;
bool FCEUI_AviEnableHUDrecording() {
    return enableHUDrecording;
}
void FCEUI_SetAviEnableHUDrecording(bool enable) {
    enableHUDrecording = enable;
}

bool disableMovieMessages = false;
bool FCEUI_AviDisableMovieMessages() {
    return disableMovieMessages;
}
void FCEUI_SetAviDisableMovieMessages(bool disable) {
    disableMovieMessages = disable;
}

// this variable contains information about the special scaling filters
void FCEUD_VideoChanged()
{
//    int buf;
//    g_config->getOption("SDL.PAL", &buf);
//    if(buf == 1)
//        PAL = 1;
//    else
//        PAL = 0; // NTSC and Dendy
}
