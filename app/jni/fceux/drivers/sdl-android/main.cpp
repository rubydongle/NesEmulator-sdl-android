#include "main.h"
#include "throttle.h"
#include "config.h"
#include "../common/cheat.h"
#include "../../fceu.h"
#include "../../movie.h"
#include "../../version.h"
#include "input.h"
#include "dface.h"
#include "sdl-video.h"
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
 * Update the video, audio, and input subsystems with the provided
 * video (XBuf) and audio (Buffer) information.
 */
void FCEUD_Update(uint8 *XBuf, int32 *Buffer, int Count) {
	extern int FCEUDnetplay;
	int ocount = Count;
	// apply frame scaling to Count
	Count = (int)(Count / g_fpsScale);
	if(Count) {
		int32 can=GetWriteSound();
		static int uflow=0;
		int32 tmpcan;

		// don't underflow when scaling fps
		if(can >= GetMaxSound() && g_fpsScale==1.0) uflow=1;	/* Go into massive underflow mode. */

		if(can > Count) can=Count;
		else uflow=0;

		WriteSound(Buffer,can);
		//if(uflow) puts("Underflow");
		tmpcan = GetWriteSound();
		// don't underflow when scaling fps
		if(g_fpsScale>1.0 || ((tmpcan < Count*0.90) && !uflow)) {
			if(XBuf && (driverInited&4) && !(NoWaiting & 2))
				BlitScreen(XBuf);
			Buffer+=can;
			Count-=can;
			if(Count) {
				if(NoWaiting) {
					can=GetWriteSound();
					if(Count>can) Count=can;
					WriteSound(Buffer,Count);
				} else {
					while(Count>0) {
						WriteSound(Buffer,(Count<ocount) ? Count : ocount);
						Count -= ocount;
					}
				}
			}
		} //else puts("Skipped");
		else if(!NoWaiting && FCEUDnetplay && (uflow || tmpcan >= (Count * 1.8))) {
			if(Count > tmpcan) Count=tmpcan;
			while(tmpcan > 0) {
				//	printf("Overwrite: %d\n", (Count <= tmpcan)?Count : tmpcan);
				WriteSound(Buffer, (Count <= tmpcan)?Count : tmpcan);
				tmpcan -= Count;
			}
		}

	} else {
		if(!NoWaiting && (!(eoptions&EO_NOTHROTTLE) || FCEUI_EmulationPaused()))
		while (SpeedThrottle())
		{
			FCEUD_UpdateInput();
		}
		if(XBuf && (driverInited&4)) {
			BlitScreen(XBuf);
		}
	}
	FCEUD_UpdateInput();
}

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

// dummy functions
#define DUMMY(__f) \
    void __f(void) {\
        printf("%s\n", #__f);\
        FCEU_DispMessage("Not implemented.",0);\
    }
DUMMY(FCEUD_HideMenuToggle)
DUMMY(FCEUD_MovieReplayFrom)
DUMMY(FCEUD_ToggleStatusIcon)
DUMMY(FCEUD_AviRecordTo)
DUMMY(FCEUD_AviStop)
void FCEUI_AviVideoUpdate(const unsigned char* buffer) { }
int FCEUD_ShowStatusIcon(void) {return 0;}
bool FCEUI_AviIsRecording(void) {return false;}
void FCEUI_UseInputPreset(int preset) { }
bool FCEUD_PauseAfterPlayback() { return false; }
// These are actually fine, but will be unused and overriden by the current UI code.
void FCEUD_TurboOn	(void) { NoWaiting|= 1; }
void FCEUD_TurboOff   (void) { NoWaiting&=~1; }
void FCEUD_TurboToggle(void) { NoWaiting^= 1; }
FCEUFILE* FCEUD_OpenArchiveIndex(ArchiveScanRecord& asr, std::string &fname, int innerIndex) { return 0; }
FCEUFILE* FCEUD_OpenArchive(ArchiveScanRecord& asr, std::string& fname, std::string* innerFilename) { return 0; }
FCEUFILE* FCEUD_OpenArchiveIndex(ArchiveScanRecord& asr, std::string &fname, int innerIndex, int* userCancel) { return 0; }
FCEUFILE* FCEUD_OpenArchive(ArchiveScanRecord& asr, std::string& fname, std::string* innerFilename, int* userCancel) { return 0; }
ArchiveScanRecord FCEUD_ScanArchive(std::string fname) { return ArchiveScanRecord(); }
void FCEUD_DebugBreakpoint() {return;}
void FCEUD_TraceInstruction() {return;}
