
#include "../../types.h"
#include "../../driver.h"
#include "../common/cheat.h"
#include "../../fceu.h"
#include "../../movie.h"
#include "../../version.h"

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
void FCEUD_TurboOn	(void) {}//NoWaiting|= 1; }
void FCEUD_TurboOff   (void) { }//NoWaiting&=~1; }
void FCEUD_TurboToggle(void) { }//NoWaiting^= 1; }
FCEUFILE* FCEUD_OpenArchiveIndex(ArchiveScanRecord& asr, std::string &fname, int innerIndex) { return 0; }
FCEUFILE* FCEUD_OpenArchive(ArchiveScanRecord& asr, std::string& fname, std::string* innerFilename) { return 0; }
FCEUFILE* FCEUD_OpenArchiveIndex(ArchiveScanRecord& asr, std::string &fname, int innerIndex, int* userCancel) { return 0; }
FCEUFILE* FCEUD_OpenArchive(ArchiveScanRecord& asr, std::string& fname, std::string* innerFilename, int* userCancel) { return 0; }
ArchiveScanRecord FCEUD_ScanArchive(std::string fname) { return ArchiveScanRecord(); }

bool swapDuty = 0;
bool paldeemphswap = 0;
int closeFinishedMovie = 1;

unsigned int *GetKeyboard(void) {
    static unsigned int key = 256;
    return &key;
}

void GetMouseData(uint32 (&md)[3]) {
    md[0] = 0;
    md[1] = 1;
    md[2] = 3;
}

u32 ModernDeemphColorMap(u8* src, u8* srcbuf, int xscale, int yscale) {
    u32 ret = 100;
    return ret;
}
int FCEUD_RecvData(void *data, uint32 len)
{
    return 0;
}
int FCEUD_SendData(void *data, uint32 len)
{
    return 1;
}
void FCEUD_SetInput(bool fourscore, bool microphone, ESI port0, ESI port1, ESIFC fcexp)
{

}
void FCEUD_NetworkClose(void)
{

}
void FCEUD_NetplayText(uint8 *text)
{

}

void FCEUD_PrintError(const char *errormsg) {

}
void FCEUD_SetPalette(unsigned char index, unsigned char r, unsigned char g, unsigned char b){

}
void FCEUD_GetPalette(unsigned char i, unsigned char *r, unsigned char *g, unsigned char *b) {

}

uint64 FCEUD_GetTime(void)
{
    return 0;//(GetCurTime()){}
}
uint64 FCEUD_GetTimeFreq(void)
{
    return 0;//(tfreq>>16){}
}
bool FCEUD_ShouldDrawInputAids()
{
    return false;//fullscreen!=0;
}

EMUFILE_FILE* FCEUD_UTF8_fstream(const char *n, const char *m) {
    return nullptr;
}

/**
 * Opens a file, C++ style, to be read a byte at a time.
 */
FILE *FCEUD_UTF8fopen(const char *fn, const char *mode)
{
    return(fopen(fn,mode));
}

//mbg 7/23/06
const char *FCEUD_GetCompilerString()
{}

//This makes me feel dirty for some reason.
void FCEU_printf(const char *format, ...) {

}
#define FCEUI_printf FCEU_printf

void FCEUD_Message(const char *s) {

}

//Network interface

//Call only when a game is loaded.
int FCEUI_NetplayStart(int nlocal, int divisor) {

}

// Call when network play needs to stop.
void FCEUI_NetplayStop(void) {

}

//Encode and send text over the network.
void FCEUI_NetplayText(uint8 *text) {

}

//Called when a fatal error occurred and network play can't continue.  This function
//should call FCEUI_NetplayStop() after it has deinitialized the network on the driver
//side.

bool FCEUI_BeginWaveRecord(const char *fn) {
    return false;
}
int FCEUI_EndWaveRecord(void) {
    return false;
}

void FCEUI_ResetNES(void) {

}
void FCEUI_PowerNES(void) {

}

void FCEUI_NTSCSELHUE(void){}
void FCEUI_NTSCSELTINT(void) {}
void FCEUI_NTSCDEC(void) {}
void FCEUI_NTSCINC(void) {}
void FCEUI_GetNTSCTH(int *tint, int *hue) {}
void FCEUI_SetNTSCTH(bool en, int tint, int hue) {}

void FCEUI_SetInput(int port, ESI type, void *ptr, int attrib) {}
void FCEUI_SetInputFC(ESIFC type, void *ptr, int attrib) {}

//tells the emulator whether a fourscore is attached
void FCEUI_SetInputFourscore(bool attachFourscore) {}
//tells whether a fourscore is attached
bool FCEUI_GetInputFourscore() {}
//tells whether the microphone is used
bool FCEUI_GetInputMicrophone() {}


//New interface functions

//0 to order screen snapshots numerically(0.png), 1 to order them file base-numerically(smb3-0.png).
//this variable isn't used at all, snap is always name-based
//void FCEUI_SetSnapName(bool a){}

//0 to keep 8-sprites limitation, 1 to remove it
void FCEUI_DisableSpriteLimitation(int a){}

void FCEUI_SetRenderPlanes(bool sprites, bool bg){}
void FCEUI_GetRenderPlanes(bool& sprites, bool& bg){}

//name=path and file to load.  returns null if it failed
//FCEUGI *FCEUI_LoadGame(const char *name, int OverwriteVidMode, bool silent = false){}

//same as FCEUI_LoadGame, except that it can load from a tempfile.
//name is the logical path to open; archiveFilename is the archive which contains name
//FCEUGI *FCEUI_LoadGameVirtual(const char *name, int OverwriteVidMode, bool silent = false){}

//general purpose emulator initialization. returns true if successful
bool FCEUI_Initialize(){}

//Emulates a frame.
void FCEUI_Emulate(uint8 **, int32 **, int32 *, int){}

//Closes currently loaded game
void FCEUI_CloseGame(void){}

//Deallocates all allocated memory.  Call after FCEUI_Emulate() returns.
void FCEUI_Kill(void){}

//Enable/Disable game genie. a=true->enabled
void FCEUI_SetGameGenie(bool a){}

//Set video system a=0 NTSC, a=1 PAL
void FCEUI_SetVidSystem(int a){}

//Set variables for NTSC(0) / PAL(1) / Dendy(2)
//Dendy has PAL framerate and resolution, but ~NTSC timings, and has 50 dummy scanlines to force 50 fps
//void FCEUI_SetRegion(int region, int notify = 1){}

//Convenience function; returns currently emulated video system(0=NTSC, 1=PAL).
int FCEUI_GetCurrentVidSystem(int *slstart, int *slend){}


//First and last scanlines to render, for ntsc and pal emulation.
void FCEUI_SetRenderedLines(int ntscf, int ntscl, int palf, int pall){}

//Sets the base directory(save states, snapshots, etc. are saved in directories below this directory.
//void FCEUI_SetBaseDirectory(std::string const & dir){}
const char *FCEUI_GetBaseDirectory(void){}

bool FCEUI_GetUserPaletteAvail(void){}
void FCEUI_SetUserPalette(uint8 *pal, int nEntries){}

//Sets up sound code to render sound at the specified rate, in samples
//per second.  Only sample rates of 44100, 48000, and 96000 are currently supported.
//If "Rate" equals 0, sound is disabled.
void FCEUI_Sound(int Rate){}
void FCEUI_SetSoundVolume(uint32 volume){}
void FCEUI_SetTriangleVolume(uint32 volume){}
void FCEUI_SetSquare1Volume(uint32 volume){}
void FCEUI_SetSquare2Volume(uint32 volume){}
void FCEUI_SetNoiseVolume(uint32 volume){}
void FCEUI_SetPCMVolume(uint32 volume){}

void FCEUI_SetSoundQuality(int quality){}

void FCEUD_SoundToggle(void){}
void FCEUD_SoundVolumeAdjust(int){}

int FCEUI_SelectState(int, int){}
extern void FCEUI_SelectStateNext(int){}

//"fname" overrides the default save state filename code if non-NULL.
//void FCEUI_SaveState(const char *fname, bool display_message=true){}
//void FCEUI_LoadState(const char *fname, bool display_message=true){}

void FCEUD_SaveStateAs(void){}
void FCEUD_LoadStateFrom(void){}

void FCEUD_MovieRecordTo(void){}
void FCEUD_LuaRunFrom(void){}


void FCEUD_SetEmulationSpeed(int cmd){}


///signals the driver to perform a file open GUI operation
void FCEUD_CmdOpen(void){}

//new merge-era driver routines here:

///signals that the cpu core hit a breakpoint. this function should not return until the core is ready for the next cycle
void FCEUD_DebugBreakpoint(int bp_num){}

///the driver should log the current instruction, if it wants (we should move the code in the win driver that does this to the shared area)
void FCEUD_TraceInstruction(uint8 *opcode, int size){}

///the driver might should update its NTView (only used if debugging support is compiled in)
void FCEUD_UpdateNTView(int scanline, bool drawall){}

///the driver might should update its PPUView (only used if debugging support is compiled in)
void FCEUD_UpdatePPUView(int scanline, int drawall){}

///called when fceu changes something in the video system you might be interested in
void FCEUD_VideoChanged(){}

//./drivers/sdl/sdl-throttle.cpp
static const double Slowest = 0.015625; // 1/64x speed (around 1 fps on NTSC)
static const double Fastest = 32;       // 32x speed   (around 1920 fps on NTSC)
static const double Normal  = 1.0;      // 1x speed    (around 60 fps on NTSC)
static uint64 Lasttime, Nexttime;
static long double desired_frametime;
static int InFrame;
double g_fpsScale = Normal; // used by sdl.cpp
bool MaxSpeed = false;
/**
 * Refreshes the FPS throttling variables.
 */
void
RefreshThrottleFPS()
{
    uint64 fps = FCEUI_GetDesiredFPS(); // Do >> 24 to get in Hz
    desired_frametime = 16777216.0l / (fps * g_fpsScale);

    Lasttime=0;
    Nexttime=0;
    InFrame=0;
}

//./drivers/sdl/sdl.cpp
int pal_emulation;
bool turbo = false;
int KillFCEUXonFrame = 0;
int dendy;

