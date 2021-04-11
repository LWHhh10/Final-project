//Stepper motor A and B control program, implementing A and B axes homing and moving functions

#include "AccelStepper.h"
#include "Stepper.h"
const int AMin = 12;
const int BMin = 3;
byte BMinVal;
byte AMinVal;
// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper StepperA(8, StepperA_IN1, StepperA_IN3, StepperA_IN2, StepperA_IN4);
AccelStepper StepperB(8, StepperB_IN5, StepperB_IN7, StepperB_IN6, StepperB_IN8);
bool IsMoving_A;
bool IsMoving_B;
bool IsLaser1;
bool IsLaser2;
float StepperA_Goal,StepperB_Goal;
float Goal_X_Position, Goal_Y_Position, Start_X_Position, Start_Y_Position;
float DefocusError_Goal,DefocusError_Start;
float Relative_Position;
void Stepper_Init() {
  pinMode(AMin, INPUT);
  digitalWrite(AMin, HIGH);
  pinMode(BMin, INPUT);
  digitalWrite(BMin, HIGH);
  IsLaser1=false;
  IsLaser2=false;
  StepperA.setMaxSpeed(1000);     // slow speed for calibrating
  //StepperA.moveTo(4096);          // try to move to max
  StepperA.setAcceleration(5000);
  AStepperHome();
  StepperB.setMaxSpeed(1000);     // slow speed for calibrating
  StepperB.moveTo(4096);          // try to move to max
  StepperB.setAcceleration(5000);
  BStepperHome();
  StepperA.setMaxSpeed(1000.0);
  StepperA.setAcceleration(50000.0);
  StepperB.setMaxSpeed(1000.0);
  StepperB.setAcceleration(50000.0);
}

void MoveStepperA(float pos)
{
    StepperA_Goal = pos / STEP_PER_CIRCLE * IMPLUSE_PER_CIRCLE;//Calculate the numbers of pulse
    StepperA.moveTo(StepperA_Goal);//Move to the expected position
    StepperA.run();
    if(StepperA.distanceToGo() == 0)
    {
    IsMoving_A = false;
  }  
}
void MoveStepperB(float pos)
{  
  StepperB_Goal = pos / STEP_PER_CIRCLE * IMPLUSE_PER_CIRCLE;//Calculate the numbers of pulse
  StepperB.moveTo(StepperB_Goal);//Move to the expected position
  StepperB.run();
  if(StepperB.distanceToGo() == 0)
  {
  IsMoving_B = false;
  }
}

void AStepperHome(){ //this routine should run the motor
  AMinVal = digitalRead(AMin);
  while (AMinVal == HIGH)
  {
    //backwards slowly till it hits the switch and stops
    StepperA.move(-2048);
    StepperA.run();
    AMinVal = digitalRead(AMin);
  }
  StepperA.setCurrentPosition(0); //set current motor position to zero
  StepperA.moveTo(StepperA_LASER1_POSITION/ STEP_PER_CIRCLE * IMPLUSE_PER_CIRCLE);
  while (StepperA.distanceToGo() != 0){
    StepperA.run();
  }
  IsLaser1=true;
}

void BStepperHome(){ //this routine should run the motor
  BMinVal = digitalRead(BMin);
  while (BMinVal == HIGH)
  {
    //backwards slowly till it hits the switch and stops
    StepperB.move(2048);
    StepperB.run();
    BMinVal = digitalRead(BMin);
  }
  StepperB.setCurrentPosition(0); //set current motor position to zero
  StepperB.moveTo(StepperB_LASER1_POSITION/ STEP_PER_CIRCLE * IMPLUSE_PER_CIRCLE);
  while (StepperB.distanceToGo() != 0){
    StepperB.run();
  }
}

void BStepperAdjust(float x,float y)//补偿离焦误差
{
  Goal_X_Position = x;
  Goal_Y_Position = y;
  DefocusError_Goal=0.1*(sqrt((DIS_XYMOTOR+sqrt(DIS_F_THETA*DIS_F_THETA+Goal_Y_Position*Goal_Y_Position))*(DIS_XYMOTOR+sqrt(DIS_F_THETA*DIS_F_THETA+Goal_Y_Position*Goal_Y_Position))+Goal_X_Position*Goal_X_Position)-(DIS_XYMOTOR+DIS_F_THETA));//calculate the defocus error
  DefocusError_Start=0.1*(sqrt((DIS_XYMOTOR+sqrt(DIS_F_THETA*DIS_F_THETA+Start_Y_Position*Start_Y_Position))*(DIS_XYMOTOR+sqrt(DIS_F_THETA*DIS_F_THETA+Start_Y_Position*Start_Y_Position))+Start_X_Position*Start_X_Position)-(DIS_XYMOTOR+DIS_F_THETA));//calculate the defocus error
  //Relative_Position = DefocusError+StepperB.currentPosition();
  //MoveStepperB(Relative_Position);
  StepperB.move((DefocusError_Start - DefocusError_Goal)/ STEP_PER_CIRCLE * IMPLUSE_PER_CIRCLE);
  while (StepperB.distanceToGo() != 0){
    StepperB.run();
  }
  Start_X_Position=Goal_X_Position;
  Start_Y_Position=Goal_Y_Position;
}


