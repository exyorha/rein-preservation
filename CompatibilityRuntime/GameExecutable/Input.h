#ifndef INPUT_H
#define INPUT_H

#include <il2cpp-api.h>

class TouchEmulator;

void InitializeInput(void);

extern bool EmulateTouchInput;

extern TouchEmulator touchEmulator;

#endif

