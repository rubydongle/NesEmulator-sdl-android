#include "main.h"
#include "throttle.h"
#include "config.h"
#include "../common/cheat.h"
#include "../../fceu.h"
#include "../../movie.h"
#include "../../version.h"
#include "input.h"
#include "dface.h"
#include "unix-netplay.h"
#include "../common/configSys.h"
#include "../../oldmovie.h"
#include "../../types.h"
#ifdef _S9XLUA_H
#include "../../fceulua.h"
#endif

#include <unistd.h>
#include <csignal>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>

#include "Emulator.h"

extern double g_fpsScale;
extern bool MaxSpeed;

//int isloaded;
bool turbo = false;
int closeFinishedMovie = 0;
int eoptions=0;
int driverInited = 0;

uint64 FCEUD_GetTime();
int gametype = 0;

//static int noconfig;
int pal_emulation;
int dendy;
bool swapDuty;
// global configuration object
Config *g_config;


/**
 * Opens a file to be read a byte at a time.
 */
EMUFILE_FILE* FCEUD_UTF8_fstream(const char *fn, const char *m) {
	SDL_Log("->FCEUD_UTF8_fstream");
	std::ios_base::openmode mode = std::ios_base::binary;
	if(!strcmp(m,"r") || !strcmp(m,"rb"))
		mode |= std::ios_base::in;
	else if(!strcmp(m,"w") || !strcmp(m,"wb"))
		mode |= std::ios_base::out | std::ios_base::trunc;
	else if(!strcmp(m,"a") || !strcmp(m,"ab"))
		mode |= std::ios_base::out | std::ios_base::app;
	else if(!strcmp(m,"r+") || !strcmp(m,"r+b"))
		mode |= std::ios_base::in | std::ios_base::out;
	else if(!strcmp(m,"w+") || !strcmp(m,"w+b"))
		mode |= std::ios_base::in | std::ios_base::out | std::ios_base::trunc;
	else if(!strcmp(m,"a+") || !strcmp(m,"a+b"))
		mode |= std::ios_base::in | std::ios_base::out | std::ios_base::app;
    return new EMUFILE_FILE(fn, m);
	//return new std::fstream(fn,mode);
}

/**
 * Opens a file, C++ style, to be read a byte at a time.
 */
FILE *FCEUD_UTF8fopen(const char *fn, const char *mode) {
	return(fopen(fn,mode));
}

static char *s_linuxCompilerString = "g++ " __VERSION__;
/**
 * Returns the compiler string.
 */
const char *FCEUD_GetCompilerString() {
	return (const char *)s_linuxCompilerString;
}

int KillFCEUXonFrame = 0;

int main(int argc, char *argv[]) {
	FCEUD_Message("Starting " FCEU_NAME_AND_VERSION "...\n");
	if(SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Could not initialize SDL: %s.\n", SDL_GetError());
		return(-1);
	}
	// 设置竖屏 参考SDLActivity中setOrientationBis
	SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft");

	int error = FCEUI_Initialize();
	if(error != 1) {
		SDL_Log("FCUI_Initialize failed errno:%d", error);
		SDL_Quit();
		return -1;
	}

	Emulator* emulator = new Emulator();
	if(!g_config) {
		SDL_Quit();
		SDL_Log("InitConfig Failed\n");
		return -1;
	}

	int romIndex = g_config->parse(argc, argv);
	error = emulator->loadGame(argv[romIndex]);
	if(error != 1) {
		SDL_Log("loadGame error:%d\n", error);
		emulator->driverKill();
		SDL_Quit();
		return -1;
	}
	SDL_Log("loadGame done");

	int frameskip = 0;
	int periodic_saves = 0;
	while(GameInfo) {
		emulator->doFun();
	}
	emulator->closeGame();

	// exit the infrastructure
	FCEUI_Kill();
	SDL_Quit();
	return 0;
}

/**
 * Get the time in ticks.
 */
uint64 FCEUD_GetTime() {
	return SDL_GetTicks();
}

/**
 * Get the tick frequency in Hz.
 */
uint64 FCEUD_GetTimeFreq(void) {
	// SDL_GetTicks() is in milliseconds
	return 1000;
}

void FCEUD_Message(const char *text) {
    SDL_Log("fceud message: %s\n", text);
}

void FCEUD_PrintError(const char *errormsg) {
    SDL_Log("fceud error: %s\n", errormsg);
}

