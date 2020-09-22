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

// NES resolution = 256x240
const int NES_WIDTH = 256;
const int NES_HEIGHT = 240;

// GLOBALS
extern Config *g_config;

// STATIC GLOBALS
static SDL_Surface *s_screen;
static SDL_Window *s_window;
static SDL_Renderer *s_renderer;
static SDL_Texture *s_texture;

static SDL_Surface *s_BlitBuf; // Buffer when using hardware-accelerated blits.
static SDL_Surface *s_IconSurface = NULL;

static int s_curbpp;
static int s_srendline, s_erendline;
static int s_tlines;
static int s_inited;

static double s_exs, s_eys;
static int s_eefx;
static int s_clipSides;
static int s_fullscreen;

#define NWIDTH	(256 - (s_clipSides ? 16 : 0))
#define NOFFSET	(s_clipSides ? 8 : 0)

static int s_paletterefresh;

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

int
KillVideo()
{
	// if the IconSurface has been initialized, destroy it
	if(s_IconSurface) {
		SDL_FreeSurface(s_IconSurface);
		s_IconSurface=0;
	}

	// return failure if the video system was not initialized
	if(s_inited == 0)
		return -1;

	// if the rest of the system has been initialized, shut it down
	// shut down the system that converts from 8 to 16/32 bpp
	if(s_curbpp > 8)
		KillBlitToHigh();

	// shut down the SDL video sub-system
	SDL_QuitSubSystem(SDL_INIT_VIDEO);

	s_inited = 0;
	return 0;
}

// this variable contains information about the special scaling filters
static int s_sponge = 0;

/**
 * These functions determine an appropriate scale factor for fullscreen/
 */
inline double GetXScale(int xres)
{
	return ((double)xres) / NWIDTH;
}
inline double GetYScale(int yres)
{
	return ((double)yres) / s_tlines;
}
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
	s_paletterefresh = 1;
	s_curbpp = 32;
//	if (!gi) {
//		return -1;
//	}
	// 创建一个窗口
	s_window = SDL_CreateWindow("fceux game window" , SDL_WINDOWPOS_CENTERED,
								SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_SHOWN);

//	FCEUI_SetShowFPS(1);

	// 创建一个渲染器
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
	int sdlRendW, sdlRendH;
	SDL_GetRendererOutputSize( s_renderer, &sdlRendW, &sdlRendH );
	SDL_Log("[SDL] Renderer Output Size: %i x %i \n", sdlRendW, sdlRendH );

	int xres, yres;
	SDL_GetWindowSize(s_window, &xres, &yres);

	s_texture = SDL_CreateTexture(s_renderer,
								  SDL_PIXELFORMAT_ARGB8888,
								  SDL_TEXTUREACCESS_STREAMING,
								  xres, yres);

	SDL_SetRenderDrawColor(s_renderer, 100, 100, 100, 255);
//	SDL_RenderClear(s_renderer);
//	SDL_RenderPresent(s_renderer);

	FCEUI_GetCurrentVidSystem(&s_srendline, &s_erendline);
	s_tlines = s_erendline - s_srendline + 1;

	if (!SDL_WasInit(SDL_INIT_VIDEO)) {
		error = SDL_InitSubSystem(SDL_INIT_VIDEO);
		if (error) {
			FCEUD_PrintError(SDL_GetError());
			return -1;
		}
	}
	s_inited = 1;

	double auto_xscale;// = GetXScale(xres);
	double auto_yscale;// = GetYScale(yres);
	double native_ratio = ((double)NWIDTH) / s_tlines;
	double screen_ratio = ((double)xres) / yres;
	int keep_ratio = 1;

	if (screen_ratio < native_ratio) {
		auto_xscale = auto_yscale = GetXScale(xres);
		if (keep_ratio) {
			auto_yscale = GetYScale(yres);
		}
	} else {
		auto_yscale = auto_xscale = GetYScale(xres);
		if (keep_ratio) {
			auto_xscale = GetXScale(yres);
		}
	}
	s_exs = auto_xscale;
	s_eys = auto_yscale;
	s_eefx = 0;

	uint32_t  rmask, gmask, bmask, amask;
	amask = 0xFF000000;
#ifdef LSB_FIRST
	rmask = 0x00FF0000;
	gmask = 0x0000FF00;
	bmask = 0x000000FF;
#else
	rmask = 0x00FF0000;
	gmask = 0x0000FF00;
	bmask = 0x000000FF;
#endif
	s_BlitBuf = SDL_CreateRGBSurface(0, xres, yres,
									 s_curbpp,
									 rmask,
									 gmask,
									 bmask, amask);

//	s_texture = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, xres, yres);

	InitBlitToHigh(s_curbpp >> 3,
				   rmask,
				   gmask,
				   bmask,
				   s_eefx, s_sponge, 0);

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

static SDL_Color s_psdl[256];

/**
 * Sets the color for a particular index in the palette.
 */
void
FCEUD_SetPalette(uint8 index,
				 uint8 r,
				 uint8 g,
				 uint8 b)
{
	s_psdl[index].r = r;
	s_psdl[index].g = g;
	s_psdl[index].b = b;

	s_paletterefresh = 1;
}

/**
 * Gets the color for a particular index in the palette.
 */
void FCEUD_GetPalette(uint8 index, uint8 *r, uint8 *g, uint8 *b) {
	*r = s_psdl[index].r;
	*g = s_psdl[index].g;
	*b = s_psdl[index].b;
}

/**
 * Pushes the palette structure into the underlying video subsystem.
 */
static void RedoPalette() {
	if(s_curbpp > 8) {
		SetPaletteBlitToHigh((uint8*)s_psdl);
	} else {
#if SDL_VERSION_ATLEAST(2, 0, 0)
		//TODO - SDL2
#else
		SDL_SetPalette(s_screen, SDL_PHYSPAL, s_psdl, 0, 256);
#endif
	}
}
// XXX soules - console lock/unlock unimplemented?

///Currently unimplemented.
void LockConsole(){}

///Currently unimplemented.
void UnlockConsole(){}

/**
 * Pushes the given buffer of bits to the screen.
 */
void
BlitScreen(uint8 *XBuf)
{
	SDL_Surface *TmpScreen;
	uint8 *dest;
	int xo = 0, yo = 0;
	XBuf += s_srendline * 256;
	TmpScreen = s_BlitBuf;

	if (s_paletterefresh)
	{
		RedoPalette();
		s_paletterefresh = 0;
	}

	// lock the display, if necessary
	if(SDL_MUSTLOCK(TmpScreen)) {
		if(SDL_LockSurface(TmpScreen) < 0) {
			return;
		}
	}

	dest = (uint8*)TmpScreen->pixels;

	Blit8ToHigh(XBuf + NOFFSET, dest, NWIDTH, s_tlines,
				TmpScreen->pitch, (int)s_exs, (int)s_eys);

	// unlock the display, if necessary
	if(SDL_MUSTLOCK(TmpScreen)) {
		SDL_UnlockSurface(TmpScreen);
	}

	SDL_Rect sdlRect;
	int scale = 0;
	if (!scale) {
		int xres, yres;
		SDL_GetWindowSize(s_window, &xres, &yres);
		//double scale = xres/NES_WIDTH > yres / NES_HEIGHT ? yres / NES_HEIGHT : yres / NES_WIDTH;//s_exs < s_eys ? s_exs : s_eys;
		double scale = s_exs < s_eys ? s_exs : s_eys;
		sdlRect.x = (xres - (NES_WIDTH *scale)) / 2;
		sdlRect.y = 0;
		sdlRect.w = NES_WIDTH * scale;
//		sdlRect.h = s_tlines * scale;
		sdlRect.h = NES_HEIGHT * scale;
	}

	SDL_UpdateTexture(s_texture, NULL, TmpScreen->pixels, TmpScreen->pitch);
	SDL_RenderClear(s_renderer);
	SDL_RenderCopy(s_renderer, s_texture, NULL, &sdlRect);
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
