// XY.h

#ifndef _XY_h
#define _XY_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif
#include "config.h"
#include "command.h"
#include "MCP48xx.h"
#include "stepper.h"
#include <Wire.h>
extern bool IsMoving_XY;
extern int Times_LinearLnterpolation_int_XYMG;
extern unsigned short* DAValMoveX_ushortPointer_XYMG;
extern unsigned short* DAValMoveY_ushortPointer_XYMG;
extern bool IsMoving_XY;
extern bool anticlockwise;
void XY_Init();
void XY_LinearInterpolation();
void XY_CircleInterpolation();
void XY_StartMove(bool laser);
void XY_StartMoveCircle(bool Laser);
inline unsigned short DisToDA_Y(float y);
inline unsigned short DisToDA_X(float x, float y);
#endif

