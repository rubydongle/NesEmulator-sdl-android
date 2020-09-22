#include "main.h"

#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

#define MAX_JOYSTICKS	32
static SDL_Joystick *s_Joysticks[MAX_JOYSTICKS] = {NULL};

static int s_jinited = 0;


/**
 * Tests if the given button is active on the joystick.
 */
int
DTestButtonJoy(ButtConfig *bc)
{
	int x;

	for(x = 0; x < bc->NumC; x++)
	{
		if(bc->ButtonNum[x] & 0x2000)
		{
			/* Hat "button" */
			if(SDL_JoystickGetHat(s_Joysticks[bc->DeviceNum[x]],
								((bc->ButtonNum[x] >> 8) & 0x1F)) & 
								(bc->ButtonNum[x]&0xFF))
				return 1; 
		}
		else if(bc->ButtonNum[x] & 0x8000) 
		{
			/* Axis "button" */
			int pos;
			pos = SDL_JoystickGetAxis(s_Joysticks[bc->DeviceNum[x]],
									bc->ButtonNum[x] & 16383);
			if ((bc->ButtonNum[x] & 0x4000) && pos <= -16383) {
				return 1;
			} else if (!(bc->ButtonNum[x] & 0x4000) && pos >= 16363) {
				return 1;
			}
		} 
		else if(SDL_JoystickGetButton(s_Joysticks[bc->DeviceNum[x]],
									bc->ButtonNum[x]))
		return 1;
	}
	return 0;
}

/**
 * Shutdown the SDL joystick subsystem.
 */
int
KillJoysticks()
{
	int n;  /* joystick index */

	if(!s_jinited) {
		return -1;
	}

	for(n = 0; n < MAX_JOYSTICKS; n++) {
		if (s_Joysticks[n] != 0) {
			SDL_JoystickClose(s_Joysticks[n]);
		}
		s_Joysticks[n]=0;
	}
	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
	return 0;
}

/**
 * Initialize the SDL joystick subsystem.
 */
int
InitJoysticks()
{
	int n; /* joystick index */
	int total;

	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	total = SDL_NumJoysticks();
	if(total>MAX_JOYSTICKS) {
		total = MAX_JOYSTICKS;
	}

	for(n = 0; n < total; n++) {
		/* Open the joystick under SDL. */
		s_Joysticks[n] = SDL_JoystickOpen(n);
		//printf("Could not open joystick %d: %s.\n",
		//joy[n] - 1, SDL_GetError());
		continue;
	}

	s_jinited = 1;
	return 1;
}
