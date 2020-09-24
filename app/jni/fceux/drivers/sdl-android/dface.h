#include "../common/args.h"
#include "../common/config.h"

#include "input.h"

extern CFGSTRUCT DriverConfig[];
extern ARGPSTRUCT DriverArgs[];

void DoDriverArgs(void);

int InitJoysticks(void);
int KillJoysticks(void);
uint32 *GetJSOr(void);

int FCEUD_NetworkConnect(void);

