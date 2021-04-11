// stepper.h

#ifndef _STEPPER_h
#define _STEPPER_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif
#include "AccelStepper.h"
#include "command.h"
#include "config.h"
extern bool IsMoving_A;
extern bool IsMoving_B;
extern bool IsLaser1;
extern bool IsLaser2;
void Stepper_Init();
void MoveStepperA(float pos);
void MoveStepperB(float pos);
void BStepperHome();
void AStepperHome();
void BStepperAdjust(float x,float y);
#endif

