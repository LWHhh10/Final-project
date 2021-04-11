#include <SPI.h>
#include "command.h"
#include "XY.h"
#include "stepper.h"

void setup() {
Command_Init();
XY_Init();
Stepper_Init();
}

void loop() {
  // put your main code here, to run repeatedly:

Command_Get();
PlaneAndMove();

}

void PlaneAndMove() {
  
  if (NeedMove_bool_Command[0]) //move the XY scanners 
  {  
    NeedMove_bool_Command[0] = false;
    IsMoving_XY=true;
    //BStepperAdjust();//Compensate the defocus error
    BStepperAdjust(GCode_Arry_Command[0],GCode_Arry_Command[1]);
    while(IsMoving_XY){
        XY_StartMove(NeedOpen_Laser);//X and Y scanner move according to the imterpolation points
      }    
    NeedOpen_Laser=false; //Close the laser  
    }
    
    else if (NeedMove_bool_Command[3]) //move the XY scanners in arc (clockwoise)
    {  
    NeedMove_bool_Command[3] = false;
    anticlockwise=false;
    IsMoving_XY=true;
    //BStepperAdjust();
    while(IsMoving_XY){
      XY_StartMoveCircle(NeedOpen_Laser);
      }          
    NeedOpen_Laser=false; 
    }
    
    else if (NeedMove_bool_Command[4]) //move the XY scanners in arc  (anticlockwise)
    {  
    NeedMove_bool_Command[4] = false;
    anticlockwise=true;
    while(IsMoving_XY){
      XY_StartMoveCircle(NeedOpen_Laser);
      }     
    //BStepperAdjust();      
    NeedOpen_Laser=false;   
    }
    
   else if (NeedMove_bool_Command[1]) //switch the laser 
   {  
   NeedMove_bool_Command[1] = false; 
   IsMoving_A=true; 
   IsMoving_B=true; 
   if(GCode_Arry_Command[2] == 1)//G1 A1: Switch to laser1
   {
    IsLaser1=true;
    IsLaser2=false;
    //analogWrite(PIN_LASER2, LASER2_OFF_POWER);
    while(IsMoving_A or IsMoving_B)
    {
   MoveStepperA(StepperA_LASER1_POSITION);
   MoveStepperB(StepperB_LASER1_POSITION);
   }  
   }
   
   else if(GCode_Arry_Command[2] == 2)//G1 A2: Switch to laser2
   {
    IsLaser1=false;
    IsLaser2=true; 
    //analogWrite(PIN_LASER1, LASER1_OFF_POWER);
    while(IsMoving_A or IsMoving_B)
    {
    MoveStepperA(StepperA_LASER2_POSITION);
    MoveStepperB(StepperB_LASER2_POSITION);
    }
   }
   else//Adjust axis A  according to the command
   {
    IsLaser1=false;
    IsLaser2=false;
    while(IsMoving_A)
    //MoveStepperA(StepperA_ORIGINAL_POSITION);//return to original
    MoveStepperA(GCode_Arry_Command[2]);   
   }
  }   
  else if (NeedMove_bool_Command[2])//Adjust axis B 
  {
    NeedMove_bool_Command[2] = false;
    IsMoving_B=true; 
    while (IsMoving_B) 
    MoveStepperB(GCode_Arry_Command[3]);  //input the position of B axis
  }     
}

