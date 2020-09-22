#ifndef __FCEU_SDL_H
#define __FCEU_SDL_H

#include <SDL.h>

#include "main.h"
#include "dface.h"
#include "input.h"

#define PERIODIC_SAVE_INTERVAL 5000 // milliseconds

const int INVALID_STATE = 99;

extern int isloaded;

extern int dendy;
extern int pal_emulation;
extern bool swapDuty;

int LoadGame(const char *path);
int CloseGame(void);
void FCEUD_Update(uint8 *XBuf, int32 *Buffer, int Count);
uint64 FCEUD_GetTime();

#endif
