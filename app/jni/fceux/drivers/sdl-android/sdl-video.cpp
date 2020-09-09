/* FCE Ultra - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2002 Xodnizel
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/// \file
/// \brief Handles the graphical game display for the SDL implementation.

#include "sdl.h"
#include "sdl-opengl.h"
#include "../common/vidblit.h"
#include "../../fceu.h"
#include "../../version.h"
#include "../../video.h"

#include "../../utils/memory.h"

#include "sdl-icon.h"
#include "dface.h"

#include "../common/configSys.h"
#include "sdl-video.h"

#ifdef CREATE_AVI
#include "../videolog/nesvideos-piece.h"
#endif

#ifdef _GTK
#include "gui.h"
#endif
#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#endif

#include <cstdio>
#include <cstring>
#include <cstdlib>

// GLOBALS
extern Config *g_config;

// STATIC GLOBALS
extern SDL_Surface *s_screen;
extern SDL_Window *s_window;
extern SDL_Renderer *s_renderer;

static SDL_Surface *s_BlitBuf; // Buffer when using hardware-accelerated blits.
static SDL_Surface *s_IconSurface = NULL;

static int s_curbpp;
static int s_srendline, s_erendline;
static int s_tlines;
static int s_inited;
static int initBlitToHighDone = 0;

#ifdef OPENGL
static int s_useOpenGL;
#endif
static double s_exs, s_eys;
static int s_eefx;
static int s_clipSides;
static int s_fullscreen;
static int noframe;
static int s_nativeWidth = -1;
static int s_nativeHeight = -1;

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
#ifdef OPENGL
	// check for OpenGL and shut it down
	if(s_useOpenGL)
		KillOpenGL();
	else
#endif
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

#if SDL_VERSION_ATLEAST(2, 0, 0)
int InitVideo(FCEUGI *gi)
{
	// This is a big TODO.  Stubbing this off into its own function,
	// as the SDL surface routines have changed drastically in SDL2
	// TODO - SDL2

	int error;
	s_curbpp = 32;
	// 创建一个窗口
	s_window = SDL_CreateWindow("fceux game window" , SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_SHOWN);
	// 创建一个渲染器
	s_renderer = SDL_CreateRenderer(s_window, -1, 0);
	int xres, yres;
	SDL_GetWindowSize(s_window, &xres, &yres);
	s_texture = SDL_CreateTexture(s_renderer,
								   SDL_PIXELFORMAT_ARGB8888,
								   SDL_TEXTUREACCESS_STREAMING,
								   xres, yres);
//	SDL_SetRenderDrawColor(s_renderer, 255, 0, 0, 255);
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
	int keep_ratio = 0;

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

	// 创建一个Surface
//	SDL_Surface *bmpSurface = SDL_LoadBMP("sdl.bmp" );
//	SDL_Log("after SDL_loadBMP");
	//设置图片中的白色为透明色
//	SDL_SetColorKey(bmpSurface, SDL_TRUE, 0xffffff);
	// 创建一个Texture
//	SDL_Texture *texture = SDL_CreateTextureFromSurface(s_renderer, bmpSurface);
//	SDL_Log("after SDL_CreateTextureFromSurface");
	//清除所有事件
//	SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);

	//使用红色填充背景
//	SDL_SetRenderDrawColor(s_renderer, 255, 0, 0, 255);
//	SDL_RenderClear(s_renderer);
	// 将纹理布置到渲染器
//	SDL_RenderCopy(s_renderer, texture, NULL, NULL);
	// 刷新屏幕
//	SDL_RenderPresent(s_renderer);

	return 0;
}
#else
/**
 * Attempts to initialize the graphical video display.  Returns 0 on
 * success, -1 on failure.
 */
int
InitVideo(FCEUGI *gi)
{
	// XXX soules - const?  is this necessary?
	const SDL_VideoInfo *vinf;
	int error, flags = 0;
	int doublebuf, xstretch, ystretch, xres, yres, show_fps;

	FCEUI_printf("Initializing video...");

	// load the relevant configuration variables
	g_config->getOption("SDL.Fullscreen", &s_fullscreen);
	g_config->getOption("SDL.DoubleBuffering", &doublebuf);
#ifdef OPENGL
	g_config->getOption("SDL.OpenGL", &s_useOpenGL);
#endif
	g_config->getOption("SDL.SpecialFilter", &s_sponge);
	g_config->getOption("SDL.XStretch", &xstretch);
	g_config->getOption("SDL.YStretch", &ystretch);
	g_config->getOption("SDL.LastXRes", &xres);
	g_config->getOption("SDL.LastYRes", &yres);
	g_config->getOption("SDL.ClipSides", &s_clipSides);
	g_config->getOption("SDL.NoFrame", &noframe);
	g_config->getOption("SDL.ShowFPS", &show_fps);

	// check the starting, ending, and total scan lines
	FCEUI_GetCurrentVidSystem(&s_srendline, &s_erendline);
	s_tlines = s_erendline - s_srendline + 1;

	// check if we should auto-set x/y resolution

    // check for OpenGL and set the global flags
#if OPENGL
	if(s_useOpenGL && !s_sponge) {
		flags = SDL_OPENGL;
	}
#endif

	// initialize the SDL video subsystem if it is not already active
	if(!SDL_WasInit(SDL_INIT_VIDEO)) {
		error = SDL_InitSubSystem(SDL_INIT_VIDEO);
		if(error) {
			FCEUD_PrintError(SDL_GetError());
			return -1;
		}
	}
	s_inited = 1;

	// shows the cursor within the display window
	SDL_ShowCursor(1);

	// determine if we can allocate the display on the video card
	vinf = SDL_GetVideoInfo();
	if(vinf->hw_available) {
		flags |= SDL_HWSURFACE;
	}

	// get the monitor's current resolution if we do not already have it
	if(s_nativeWidth < 0) {
		s_nativeWidth = vinf->current_w;
	}
	if(s_nativeHeight < 0) {
		s_nativeHeight = vinf->current_h;
	}

	// check to see if we are showing FPS
	FCEUI_SetShowFPS(show_fps);
    
	// check if we are rendering fullscreen
	if(s_fullscreen) {
		int no_cursor;
		g_config->getOption("SDL.NoFullscreenCursor", &no_cursor);
		flags |= SDL_FULLSCREEN;
		SDL_ShowCursor(!no_cursor);
	}
	else {
		SDL_ShowCursor(1);
	}
    
	if(noframe) {
		flags |= SDL_NOFRAME;
	}

	// gives the SDL exclusive palette control... ensures the requested colors
	flags |= SDL_HWPALETTE;

	// enable double buffering if requested and we have hardware support
#ifdef OPENGL
	if(s_useOpenGL) {
		FCEU_printf("Initializing with OpenGL (Disable with '--opengl 0').\n");
		if(doublebuf) {
			 SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		}
	} else
#endif
		if(doublebuf && (flags & SDL_HWSURFACE)) {
			flags |= SDL_DOUBLEBUF;
		}

	if(s_fullscreen) {
		int desbpp, autoscale;
		g_config->getOption("SDL.BitsPerPixel", &desbpp);
		g_config->getOption("SDL.AutoScale", &autoscale);
		if (autoscale)
		{
			double auto_xscale = GetXScale(xres);
			double auto_yscale = GetYScale(yres);
			double native_ratio = ((double)NWIDTH) / s_tlines;
			double screen_ratio = ((double)xres) / yres;
			int keep_ratio;
            
			g_config->getOption("SDL.KeepRatio", &keep_ratio);
            
			// Try to choose resolution
			if (screen_ratio < native_ratio)
			{
				// The screen is narrower than the original. Maximizing width will not clip
				auto_xscale = auto_yscale = GetXScale(xres);
				if (keep_ratio) 
					auto_yscale = GetYScale(yres);
			}
			else
			{
				auto_yscale = auto_xscale = GetYScale(yres);
				if (keep_ratio) 
					auto_xscale = GetXScale(xres);
			}
			s_exs = auto_xscale;
			s_eys = auto_yscale;
		}
		else
		{
			g_config->getOption("SDL.XScale", &s_exs);
			g_config->getOption("SDL.YScale", &s_eys);
		}
		g_config->getOption("SDL.SpecialFX", &s_eefx);

#ifdef OPENGL
		if(!s_useOpenGL) {
			s_exs = (int)s_exs;
			s_eys = (int)s_eys;
		} else {
			desbpp = 0;
		}
        

		if((s_useOpenGL && !xstretch) || !s_useOpenGL)
#endif
			if(xres < (NWIDTH * s_exs) || s_exs <= 0.01) {
				FCEUD_PrintError("xscale out of bounds.");
				KillVideo();
				return -1;
			}

#ifdef OPENGL
		if((s_useOpenGL && !ystretch) || !s_useOpenGL)
#endif
			if(yres < s_tlines * s_eys || s_eys <= 0.01) {
				FCEUD_PrintError("yscale out of bounds.");
				KillVideo();
				return -1;
			}

#ifdef OPENGL
		s_screen = SDL_SetVideoMode(s_useOpenGL ? s_nativeWidth : xres,
									s_useOpenGL ? s_nativeHeight : yres,
									desbpp, flags);
#else
		s_screen = SDL_SetVideoMode(xres, yres, desbpp, flags);
#endif

		if(!s_screen) {
			FCEUD_PrintError(SDL_GetError());
			return -1;
		}
	} else {
		int desbpp;
		g_config->getOption("SDL.BitsPerPixel", &desbpp);

		g_config->getOption("SDL.XScale", &s_exs);
		g_config->getOption("SDL.YScale", &s_eys);
		g_config->getOption("SDL.SpecialFX", &s_eefx);

		// -Video Modes Tag-
		if(s_sponge) {
			if(s_sponge <= 3 && s_sponge >= 1)
			{
				s_exs = s_eys = 2;
			} else if (s_sponge >=4 && s_sponge <= 5)
			{
				s_exs = s_eys = 3;
			} else if (s_sponge >= 6 && s_sponge <= 8)
			{
				s_exs = s_eys = s_sponge - 4;
			}
			else if(s_sponge == 9)
			{
				s_exs = s_eys = 3;
			}
			else
			{
				s_exs = s_eys = 1;
			}
			if(s_sponge == 3) {
				xres = 301 * s_exs;
			}
			s_eefx = 0;
			if(s_sponge == 1 || s_sponge == 4) {
				desbpp = 32;
			}
		}

		int scrw = NWIDTH * s_exs;
		if(s_sponge == 3) {
			scrw = 301 * s_exs;
		}

#ifdef OPENGL
		if(!s_useOpenGL) {
			s_exs = (int)s_exs;
			s_eys = (int)s_eys;
		}
		if(s_exs <= 0.01) {
			FCEUD_PrintError("xscale out of bounds.");
			KillVideo();
			return -1;
		}
		if(s_eys <= 0.01) {
			FCEUD_PrintError("yscale out of bounds.");
			KillVideo();
			return -1;
		}
		if(s_sponge && s_useOpenGL) {
			FCEUD_PrintError("scalers not compatible with openGL mode.");
			KillVideo();
			return -1;
		}
#endif

#if defined(_GTK) && defined(SDL_VIDEO_DRIVER_X11) && defined(GDK_WINDOWING_X11)
		if(noGui == 0)
		{
			while (gtk_events_pending())
				gtk_main_iteration_do(FALSE);
        
			char SDL_windowhack[128];
			sprintf(SDL_windowhack, "SDL_WINDOWID=%u", (unsigned int)GDK_WINDOW_XID(gtk_widget_get_window(evbox)));
			SDL_putenv(SDL_windowhack);
        
			// init SDL video
			if (SDL_WasInit(SDL_INIT_VIDEO))
				SDL_QuitSubSystem(SDL_INIT_VIDEO);
			if ( SDL_InitSubSystem(SDL_INIT_VIDEO) < 0 )
			{
				fprintf(stderr, "Couldn't init SDL video: %s\n", SDL_GetError());
				gtk_main_quit();
			}
		}
#endif
        
		s_screen = SDL_SetVideoMode(scrw, (int)(s_tlines * s_eys),
								desbpp, flags);
		if(!s_screen) {
			FCEUD_PrintError(SDL_GetError());
			return -1;
		}

#ifdef _GTK
		if(noGui == 0)
		{
			GtkRequisition req;
			gtk_widget_size_request(GTK_WIDGET(MainWindow), &req);
			gtk_window_resize(GTK_WINDOW(MainWindow), req.width, req.height);
		 }
#endif
		 }
	s_curbpp = s_screen->format->BitsPerPixel;
	if(!s_screen) {
		FCEUD_PrintError(SDL_GetError());
		KillVideo();
		return -1;
	}

#if 0
	// XXX soules - this would be creating a surface on the video
    //              card, but was commented out for some reason...
    s_BlitBuf = SDL_CreateRGBSurface(SDL_HWSURFACE, 256, 240,
                                     s_screen->format->BitsPerPixel,
                                     s_screen->format->Rmask,
                                     s_screen->format->Gmask,
                                     s_screen->format->Bmask, 0);
#endif

	FCEU_printf(" Video Mode: %d x %d x %d bpp %s\n",
				s_screen->w, s_screen->h, s_screen->format->BitsPerPixel,
				s_fullscreen ? "full screen" : "");

	if(s_curbpp != 8 && s_curbpp != 16 && s_curbpp != 24 && s_curbpp != 32) {
		FCEU_printf("  Sorry, %dbpp modes are not supported by FCE Ultra.  Supported bit depths are 8bpp, 16bpp, and 32bpp.\n", s_curbpp);
		KillVideo();
		return -1;
	}

	// if the game being run has a name, set it as the window name
	if(gi)
	{
		if(gi->name) {
			SDL_WM_SetCaption((const char *)gi->name, (const char *)gi->name);
		} else {
			SDL_WM_SetCaption(FCEU_NAME_AND_VERSION,"FCE Ultra");
		}
	}

	// create the surface for displaying graphical messages
#ifdef LSB_FIRST
	s_IconSurface = SDL_CreateRGBSurfaceFrom((void *)fceu_playicon.pixel_data,
											32, 32, 24, 32 * 3,
											0xFF, 0xFF00, 0xFF0000, 0x00);
#else
	s_IconSurface = SDL_CreateRGBSurfaceFrom((void *)fceu_playicon.pixel_data,
											32, 32, 24, 32 * 3,
											0xFF0000, 0xFF00, 0xFF, 0x00);
#endif
	SDL_WM_SetIcon(s_IconSurface,0);
	s_paletterefresh = 1;

	// XXX soules - can't SDL do this for us?
	 // if using more than 8bpp, initialize the conversion routines
	if(s_curbpp > 8) {
	InitBlitToHigh(s_curbpp >> 3,
						s_screen->format->Rmask,
						s_screen->format->Gmask,
						s_screen->format->Bmask,
						s_eefx, s_sponge, 0);
#ifdef OPENGL
		if(s_useOpenGL) 
		{
			int openGLip;
			g_config->getOption("SDL.OpenGLip", &openGLip);

			if(!InitOpenGL(NOFFSET, 256 - (s_clipSides ? 8 : 0),
						s_srendline, s_erendline + 1,
						s_exs, s_eys, s_eefx,
						openGLip, xstretch, ystretch, s_screen)) 
			{
				FCEUD_PrintError("Error initializing OpenGL.");
				KillVideo();
				return -1;
			}
		}
#endif
	}
	return 0;
}
#endif

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
#ifdef _GTK
	if(noGui == 0)
	{
		if(!fullscreen)
		showGui(0);
		else
			showGui(1);
	}
#endif
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
void
FCEUD_GetPalette(uint8 index,
				uint8 *r,
				uint8 *g,
				uint8 *b)
{
	*r = s_psdl[index].r;
	*g = s_psdl[index].g;
	*b = s_psdl[index].b;
}

/** 
 * Pushes the palette structure into the underlying video subsystem.
 */
static void RedoPalette()
{
#ifdef OPENGL
	if(s_useOpenGL)
		SetOpenGLPalette((uint8*)s_psdl);
	else 
#endif
	{
		if(s_curbpp > 8) {
			SetPaletteBlitToHigh((uint8*)s_psdl);
		} else
		{
#if SDL_VERSION_ATLEAST(2, 0, 0)
			//TODO - SDL2
#else
			SDL_SetPalette(s_screen, SDL_PHYSPAL, s_psdl, 0, 256);
#endif
		}
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
    SDL_Log("%s XBuf size: %d\n", __FUNCTION__, sizeof(XBuf));
	SDL_Surface *TmpScreen;
	uint8 *dest;
	int xo = 0, yo = 0;
	XBuf += s_srendline * 256;
	TmpScreen = s_BlitBuf;

	// lock the display, if necessary
	if(SDL_MUSTLOCK(TmpScreen)) {
		if(SDL_LockSurface(TmpScreen) < 0) {
			return;
		}
	}

	dest = (uint8*)TmpScreen->pixels;
	xo = (int)(((TmpScreen->w - NWIDTH * s_exs)) / 2);
	dest += xo * (s_curbpp >> 3);
	if(TmpScreen->h > (s_tlines * s_eys)) {
		yo = (int)((TmpScreen->h - s_tlines * s_eys) / 2);
		dest += yo * TmpScreen->pitch;
	}

	// XXX soules - again, I'm surprised SDL can't handle this
	// perform the blit, converting bpp if necessary
	if(s_curbpp > 8) {
		if(s_BlitBuf) {
			Blit8ToHigh(XBuf + NOFFSET, dest, NWIDTH, s_tlines,
						TmpScreen->pitch, 1, 1);
		} else {
			Blit8ToHigh(XBuf + NOFFSET, dest, NWIDTH, s_tlines,
						TmpScreen->pitch, (int)s_exs, (int)s_eys);
		}
	} else {
		if(s_BlitBuf) {
			Blit8To8(XBuf + NOFFSET, dest, NWIDTH, s_tlines,
					TmpScreen->pitch, 1, 1, 0, s_sponge);
		} else {
			Blit8To8(XBuf + NOFFSET, dest, NWIDTH, s_tlines,
					TmpScreen->pitch, (int)s_exs, (int)s_eys,
					s_eefx, s_sponge);
		}
	}

	// unlock the display, if necessary
	if(SDL_MUSTLOCK(TmpScreen)) {
		SDL_UnlockSurface(TmpScreen);
	}

	SDL_UpdateTexture(s_texture, NULL, TmpScreen->pixels, TmpScreen->pitch);
	SDL_RenderClear(s_renderer);
	SDL_RenderCopy(s_renderer, s_texture, NULL, NULL);
	SDL_RenderPresent(s_renderer);
}

/**
 *  Converts an x-y coordinate in the window manager into an x-y
 *  coordinate on FCEU's screen.
 */
uint32
PtoV(uint16 x,
	uint16 y)
{
	y = (uint16)((double)y / s_eys);
	x = (uint16)((double)x / s_exs);
	if(s_clipSides) {
		x += 8;
	}
	y += s_srendline;
	return (x | (y << 16));
}

bool enableHUDrecording = false;
bool FCEUI_AviEnableHUDrecording()
{
	if (enableHUDrecording)
		return true;

	return false;
}
void FCEUI_SetAviEnableHUDrecording(bool enable)
{
	enableHUDrecording = enable;
}

bool disableMovieMessages = false;
bool FCEUI_AviDisableMovieMessages()
{
	if (disableMovieMessages)
		return true;

	return false;
}
void FCEUI_SetAviDisableMovieMessages(bool disable)
{
	disableMovieMessages = disable;
}
