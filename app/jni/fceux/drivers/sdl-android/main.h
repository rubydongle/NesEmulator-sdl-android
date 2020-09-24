#ifndef __FCEU_SDL_MAIN_H
#define __FCEU_SDL_MAIN_H

#include "../../driver.h"
#include "../common/config.h"
#include "../common/args.h"
#include <SDL.h>

#include "dface.h"
#include "input.h"

#define PERIODIC_SAVE_INTERVAL 5000 // milliseconds

extern int driverInited;// = 0;
const int INVALID_STATE = 99;

extern int isloaded;

extern int dendy;
extern int pal_emulation;
extern bool swapDuty;


int DriverInitialize(FCEUGI *gi);
int LoadGame(const char *path);
int CloseGame(void);
//void FCEUD_Update(uint8 *XBuf, int32 *Buffer, int Count);
uint64 FCEUD_GetTime();

extern int eoptions;
#define EO_NO8LIM      1
#define EO_SUBASE      2
#define EO_CLIPSIDES   8
#define EO_SNAPNAME    16
#define EO_FOURSCORE	32
#define EO_NOTHROTTLE	64
#define EO_GAMEGENIE	128
#define EO_PAL		256
#define EO_LOWPASS	512
#define EO_AUTOHIDE	1024

extern int _sound;
extern long soundrate;
extern long soundbufsize;

extern int pal_emulation;

// Device management defaults
#define NUM_INPUT_DEVICES 3

// GamePad defaults
#define GAMEPAD_NUM_DEVICES 4
#define GAMEPAD_NUM_BUTTONS 10
extern const char *GamePadNames[GAMEPAD_NUM_BUTTONS];
extern const char *DefaultGamePadDevice[GAMEPAD_NUM_DEVICES];
extern const int DefaultGamePad[GAMEPAD_NUM_DEVICES][GAMEPAD_NUM_BUTTONS];

// PowerPad defaults
#define POWERPAD_NUM_DEVICES 2
#define POWERPAD_NUM_BUTTONS 12
extern const char *PowerPadNames[POWERPAD_NUM_BUTTONS];
extern const char *DefaultPowerPadDevice[POWERPAD_NUM_DEVICES];
extern const int DefaultPowerPad[POWERPAD_NUM_DEVICES][POWERPAD_NUM_BUTTONS];

// QuizKing defaults
#define QUIZKING_NUM_BUTTONS 6
extern const char *QuizKingNames[QUIZKING_NUM_BUTTONS];
extern const char *DefaultQuizKingDevice;
extern const int DefaultQuizKing[QUIZKING_NUM_BUTTONS];

// HyperShot defaults
#define HYPERSHOT_NUM_BUTTONS 4
extern const char *HyperShotNames[HYPERSHOT_NUM_BUTTONS];
extern const char *DefaultHyperShotDevice;
extern const int DefaultHyperShot[HYPERSHOT_NUM_BUTTONS];

// Mahjong defaults
#define MAHJONG_NUM_BUTTONS 21
extern const char *MahjongNames[MAHJONG_NUM_BUTTONS];
extern const char *DefaultMahjongDevice;
extern const int DefaultMahjong[MAHJONG_NUM_BUTTONS];

// TopRider defaults
#define TOPRIDER_NUM_BUTTONS 8
extern const char *TopRiderNames[TOPRIDER_NUM_BUTTONS];
extern const char *DefaultTopRiderDevice;
extern const int DefaultTopRider[TOPRIDER_NUM_BUTTONS];

// FTrainer defaults
#define FTRAINER_NUM_BUTTONS 12
extern const char *FTrainerNames[FTRAINER_NUM_BUTTONS];
extern const char *DefaultFTrainerDevice;
extern const int DefaultFTrainer[FTRAINER_NUM_BUTTONS];

// FamilyKeyBoard defaults
#define FAMILYKEYBOARD_NUM_BUTTONS 0x48
extern const char *FamilyKeyBoardNames[FAMILYKEYBOARD_NUM_BUTTONS];
extern const char *DefaultFamilyKeyBoardDevice;
extern const int DefaultFamilyKeyBoard[FAMILYKEYBOARD_NUM_BUTTONS];

#endif
