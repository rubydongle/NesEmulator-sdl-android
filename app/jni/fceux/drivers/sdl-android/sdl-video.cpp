#include "main.h"
#include "sdl-opengl.h"
#include "../common/vidblit.h"
#include "../../fceu.h"
#include "../../version.h"
#include "../../video.h"
#include "../../utils/memory.h"
#include "dface.h"
#include "../common/configSys.h"
#include "sdl-video.h"

#ifdef CREATE_AVI
#include "../videolog/nesvideos-piece.h"
#endif

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <GLES2/gl2.h>

// NES resolution = 256x240
#define NES_WIDTH  256
//#define NES_HEIGHT  240
#define NES_HEIGHT  224

static SDL_Color s_psdl[256];
//#define PALETTE_TYPE unsigned int
#define GET_PIXEL(buf, idx) emuPalette[buf[idx]]

//PALETTE_TYPE emuPalette[256];
//void setPalette(int idx, int value) {
//	emuPalette[idx] = value;
//}

#define WIDTH 256
#define HEIGHT_PAL 240
#define HEIGHT_NTSC 224

// GLOBALS
extern Config *g_config;

// STATIC GLOBALS
static SDL_Window *s_window;
static SDL_Renderer *s_renderer;
static SDL_Texture *s_texture;

//static int s_curbpp;
static int s_srendline, s_erendline;
static int s_tlines;
static int s_inited;

static double s_exs = 1, s_eys = 1;
//static int s_eefx;
static int s_clipSides;
static int s_fullscreen;

#define NWIDTH	(256 - (s_clipSides ? 16 : 0))
#define NOFFSET	(s_clipSides ? 8 : 0)

extern bool MaxSpeed;

/**
 * Attempts to destroy the graphical video display.  Returns 0 on
 * success, -1 on failure.
 */

//draw input aids if we are fullscreen
bool FCEUD_ShouldDrawInputAids()
{
	return s_fullscreen!=0;
}


int KillVideo() {
	// return failure if the video system was not initialized
	if(s_inited == 0)
		return -1;
	// shut down the SDL video sub-system
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	s_inited = 0;
	return 0;
}

// this variable contains information about the special scaling filters
void FCEUD_VideoChanged()
{
	int buf;
	g_config->getOption("SDL.PAL", &buf);
	if(buf == 1)
		PAL = 1;
	else
		PAL = 0; // NTSC and Dendy
}

int InitVideo(FCEUGI *gi)
{
	int error;
	// 创建一个窗口
	s_window = SDL_CreateWindow("fceux game window" , SDL_WINDOWPOS_CENTERED,
								SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_SHOWN);
	 FCEUI_SetShowFPS(1);
//	 创建一个渲染器
	uint32_t baseFlags = 0;//vsyncEnabled ? SDL_RENDERER_PRESENTVSYNC : 0;
	s_renderer = SDL_CreateRenderer(s_window, -1, baseFlags | SDL_RENDERER_ACCELERATED);
	if (s_renderer == NULL)
	{
		printf("[SDL] Failed to create accelerated renderer.\n");
		printf("[SDL] Attempting to create software renderer...\n");
		s_renderer = SDL_CreateRenderer(s_window, -1, baseFlags | SDL_RENDERER_SOFTWARE);

		if (s_renderer == NULL)
		{
			printf("[SDL] Failed to create software renderer.\n");
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

/**
 * Toggles the full-screen display.
 */
void ToggleFS()
{
	// pause while we we are making the switch
	bool paused = FCEUI_EmulationPaused();
	if(!paused)
		FCEUI_ToggleEmulationPause();

	int error, fullscreen = s_fullscreen;

	// shut down the current video system
	KillVideo();

	// flip the fullscreen flag
	g_config->setOption("SDL.Fullscreen", !fullscreen);
	// try to initialize the video
	error = InitVideo(GameInfo);
	if(error) {
		// if we fail, just continue with what worked before
		g_config->setOption("SDL.Fullscreen", fullscreen);
		InitVideo(GameInfo);
	}
	// if we paused to make the switch; unpause
	if(!paused)
		FCEUI_ToggleEmulationPause();
}


///**
// * Sets the color for a particular index in the palette.
// */
//void FCEUD_SetPalette(uint8 index, uint8 r, uint8 g, uint8 b)
//{
//	s_psdl[index].r = r;
//	s_psdl[index].g = g;
//	s_psdl[index].b = b;
//}
//
///**
// * Gets the color for a particular index in the palette.
// */
//void FCEUD_GetPalette(uint8 index, uint8 *r, uint8 *g, uint8 *b) {
//	*r = s_psdl[index].r;
//	*g = s_psdl[index].g;
//	*b = s_psdl[index].b;
//}


/**
 * Pushes the given buffer of bits to the screen.
 */
void
BlitScreen(uint8 *XBuf)
{
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
	sdlRect.w = (NES_WIDTH * yres)/s_tlines;//scale;
	sdlRect.h = yres;//s_tlines * scale;
	sdlRect.y = 0;
	sdlRect.x = (xres - sdlRect.w) / 2;

	SDL_RenderCopy(s_renderer, s_texture, 0, &sdlRect);//&destRect);
	SDL_RenderPresent(s_renderer);
}

/**
 *  Converts an x-y coordinate in the window manager into an x-y
 *  coordinate on FCEU's screen.
 */
uint32 PtoV(uint16 x, uint16 y) {
	y = (uint16)((double)y / s_eys);
	x = (uint16)((double)x / s_exs);
	if(s_clipSides) {
		x += 8;
	}
	y += s_srendline;
	return (x | (y << 16));
}

bool enableHUDrecording = false;
bool FCEUI_AviEnableHUDrecording() {
	if (enableHUDrecording)
		return true;

	return false;
}
void FCEUI_SetAviEnableHUDrecording(bool enable) {
	enableHUDrecording = enable;
}

bool disableMovieMessages = false;
bool FCEUI_AviDisableMovieMessages() {
	if (disableMovieMessages)
		return true;

	return false;
}
void FCEUI_SetAviDisableMovieMessages(bool disable) {
	disableMovieMessages = disable;
}
