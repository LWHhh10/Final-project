//XY mirrors control program, implementing interpolation and D/A conversion

#include "XY.h"
#include "MCP48xx.h"
MCP4822 dac(53); //Create a dac object

bool IsMoving_XY;
bool anticlockwise;
int Times_LinearInterpolation;
int T_LinearInterpolation;
int Times_CircleInterpolation;
int T_CircleInterpolation;

unsigned short* DAValBufferX;
unsigned short* DAValBufferY;
unsigned short* BufferX;
unsigned short* BufferY;

float Radius;
float Goal_X_float, Goal_Y_float, Start_X_float, Start_Y_float;
float Dis_X_float, Dis_Y_float;//how far between Goal and Start
float Plus_X_float, Plus_Y_float;//how many long in once Linearinterpolation
float Plane_X_float, Plane_Y_float;
float Plane_X_circle, Plane_Y_circle;
float RadVal_X_float, RadVal_Y_float;
float cos_Ti,sin_Ti;
float theta_per_segment;//how large angle in once Circularinterpolation
float Offset_X,Offset_Y,Center_X,Center_Y;
float CenterStart_X,CenterStart_Y,CenterGoal_X,CenterGoal_Y;
float Angular_travel,Plus_rad_float , Arc_Length;

void XY_Init()
{  
    dac.init();
    dac.turnOnChannelA();// The channels are turned off at startup so we need to turn the channel we need on
    dac.turnOnChannelB();
    dac.setGainA(MCP4822::High);
    dac.setGainB(MCP4822::High);
    IsMoving_XY=false;
    pinMode(PIN_LASER1, OUTPUT);//Set the port as the output
    analogWrite(PIN_LASER1, LASER1_OFF_POWER);
    pinMode(PIN_LASER2, OUTPUT);
    analogWrite(PIN_LASER2, LASER2_OFF_POWER);
}


void XY_LinearInterpolation()
{
  Goal_X_float = GCode_Arry_Command[0];//Get the expected coordinate from the serial port 
  Goal_Y_float = GCode_Arry_Command[1];
  Dis_X_float = Goal_X_float - Start_X_float;//Calculate the distance from the start position to the end position
  Dis_Y_float = Goal_Y_float - Start_Y_float;
  Times_LinearInterpolation= sqrt(Dis_X_float*Dis_X_float + Dis_Y_float*Dis_Y_float) / SPEED_XY*CYCLE_INTERPOLARION;//Determine the number of interpolations

  if (Times_LinearInterpolation != 0) { 
    DAValBufferX = new unsigned short[Times_LinearInterpolation];
    DAValBufferY = new unsigned short[Times_LinearInterpolation];

    if (Dis_X_float > 0.00001 || Dis_X_float < 0.00001)//Calculate the X and Y increase components for each time
      Plus_X_float = (Dis_X_float) / Times_LinearInterpolation;
    else
      Plus_X_float = 0;

    if (Dis_Y_float > 0.00001 || Dis_Y_float < 0.00001)
      Plus_Y_float = (Dis_Y_float) / Times_LinearInterpolation;
    else
      Plus_Y_float = 0;

    Plane_X_float = Start_X_float;//Initial the position
    Plane_Y_float = Start_Y_float;

    while (T_LinearInterpolation < Times_LinearInterpolation)//Obtain the coordinates of middle positions until the end of the interpolation
    {
      Plane_X_float = Plane_X_float + Plus_X_float;
      Plane_Y_float = Plane_Y_float + Plus_Y_float;

      DAValBufferX[T_LinearInterpolation] = DisToDA_X(Plane_X_float, Plane_Y_float);//Convert the coordinates to digital values
      DAValBufferY[T_LinearInterpolation] = DisToDA_Y(Plane_Y_float);
      BStepperAdjust(Plane_X_float,Plane_Y_float);//Compensate the defocus error
      dac.setVoltageA(DAValBufferX[T_LinearInterpolation]);//Use the dac library to send the voltage signal to the galvanometer control cards
      dac.setVoltageB(DAValBufferY[T_LinearInterpolation]);
      dac.updateDAC();
      T_LinearInterpolation++;

    }
    Start_X_float = Goal_X_float;//Updata the current position
    Start_Y_float = Goal_Y_float;
    T_LinearInterpolation = 0;
  }
  
  else //when the distance is evry small, the laser can moves to the expected position directly
  { 
    DAValBufferX = new unsigned short[1];
    DAValBufferY = new unsigned short[1];
    Times_LinearInterpolation = 1;
    DAValBufferX[0] = { DisToDA_X(Goal_X_float, Goal_Y_float) };
    DAValBufferY[0] = { DisToDA_Y(Goal_Y_float) };
    dac.setVoltageA(DAValBufferX[0]);
    dac.setVoltageB(DAValBufferY[0]);
    dac.updateDAC();
    Start_X_float = Goal_X_float;
    Start_Y_float = Goal_Y_float;
  }

}

void XY_CircleInterpolation()//Circular interpolation algorithm
{
  
  Goal_X_float = GCode_Arry_Command[0];
  Goal_Y_float = GCode_Arry_Command[1];
  Offset_X = GCode_Arry_Command[4];
  Offset_Y = GCode_Arry_Command[5];
  
  Center_X = Start_X_float + Offset_X;//Calculate the center of the circle
  Center_Y = Start_Y_float + Offset_Y;
  CenterStart_X = -Offset_X;//vector from the center to the start position
  CenterStart_Y = -Offset_Y;
  CenterGoal_X = Goal_X_float-Center_X;//vector from the center to the end position
  CenterGoal_Y = Goal_Y_float-Center_Y;
  
  Radius = sqrt(Offset_X*Offset_X+Offset_Y*Offset_Y);//Radius of the circle
  Angular_travel=atan2((CenterStart_X*CenterGoal_Y-CenterStart_Y*CenterGoal_X),(CenterStart_X*CenterGoal_X+CenterStart_Y*CenterGoal_Y));
  //Calculate the travelled angle according to the dot product and cross product of two vectors
  
  if (Angular_travel > 0)//clockwise circle: the travelled angle should be a negative value
  Angular_travel = -Angular_travel;
  
  if(anticlockwise)//anticlockwise circle: the travelled angle should add 360
  Angular_travel += radians(360);
  
  Arc_Length = abs( Radius * Angular_travel);//Calcuate the arc length
  Times_CircleInterpolation = floor(Arc_Length/ SPEED_XY*CYCLE_INTERPOLARION);//Calculate the interpolation times
  theta_per_segment = Angular_travel/ Times_CircleInterpolation;//Changing angle for each time
  
   if (Times_CircleInterpolation != 0) { 
    BufferX = new unsigned short[Times_CircleInterpolation];
    BufferY = new unsigned short[Times_CircleInterpolation];
    T_CircleInterpolation = 1;
    
  while (T_CircleInterpolation < Times_CircleInterpolation)
    {
      cos_Ti = cos(T_CircleInterpolation*theta_per_segment); 
      sin_Ti = sin(T_CircleInterpolation*theta_per_segment);
      //Plane_circle=(rcos(θs+θi),rsin(θs+θi)),θi=i*theta_per_segment
      Plane_X_circle = CenterStart_X * cos_Ti - CenterStart_Y * sin_Ti;//Plane_X_circle=rcos(θs+θi)=rcosθscosθi-rsinθssinθi
      Plane_Y_circle = CenterStart_X * sin_Ti + CenterStart_Y * cos_Ti;//PlaneY_circle=rsin(θs+θi)=rcosθssinθi+rsinθscosθi  
      BufferX[T_CircleInterpolation] = DisToDA_X(Center_X+Plane_X_circle,Center_Y+Plane_Y_circle);//Convert the coordinates to digital values
      BufferY[T_CircleInterpolation] = DisToDA_Y(Center_Y+Plane_Y_circle);
      BStepperAdjust(Plane_X_circle,Plane_Y_circle);//Comppenste the defocus error
      dac.setVoltageA(BufferX[T_CircleInterpolation]);//Use the dac library to send the voltage signal to the galvanometer control cards
      dac.setVoltageB(BufferY[T_CircleInterpolation]);
      dac.updateDAC();
      T_CircleInterpolation++;
    }
    T_CircleInterpolation = 1;
    Start_X_float=Goal_X_float;//Updata the current position
    Start_Y_float=Goal_Y_float;
   
   }
}


inline unsigned short DisToDA_X(float x, float y) //Convert the coordinates x to a digital value from 0 to 4095
{
  RadVal_X_float = atan(x / (sqrt(DIS_F_THETA*DIS_F_THETA + y*y) + DIS_XYMOTOR));
  RadVal_Y_float = atan(y / DIS_F_THETA);
  return (atan((x - (DIS_F_THETA*tan(RadVal_X_float)*(1 / cos(RadVal_Y_float) - 1))) / (sqrt(DIS_F_THETA*DIS_F_THETA + y*y) + DIS_XYMOTOR)) / RAD_MG_MAX * 2048 + 2048)* 5/BASE_VOLTAGE;
}

inline unsigned short DisToDA_Y(float y) //Convert the coordinates y to a digital value from 0 to 4095
{
  return  (RadVal_Y_float / RAD_MG_MAX * 2048 + 2048)*5 / BASE_VOLTAGE;
}

void XY_StartMove(bool Laser)//Determine which laser works for the linear printing

{
  if (IsLaser1) //If the machine is at the laser1 position, the first laser works
  {
    analogWrite(PIN_LASER2, LASER2_OFF_POWER);
    analogWrite(PIN_LASER1, LASER1_ON_POWER);
   XY_LinearInterpolation();//Move according to the array DAValBufferX and DAValBufferY data
  }
  else if (IsLaser2) //If the machine is at the laser2 position, the second laser works
  {
    analogWrite(PIN_LASER1, LASER1_OFF_POWER);
    analogWrite(PIN_LASER2, LASER2_ON_POWER);
    XY_LinearInterpolation();
  }
 
    analogWrite(PIN_LASER1, LASER1_OFF_POWER);
    analogWrite(PIN_LASER2, LASER2_OFF_POWER);
    delete[] DAValBufferX;//Clear the array
    delete[] DAValBufferY;
    IsMoving_XY=false;
}

void XY_StartMoveCircle(bool Laser)//Determine which laser works for the circular printing

{

  if (IsLaser1) 
  {
    analogWrite(PIN_LASER2, LASER2_OFF_POWER);
    analogWrite(PIN_LASER1, LASER1_ON_POWER);
   XY_CircleInterpolation();//Move according to the array BufferX and BufferY data
  }
  else if (IsLaser2) 
  {
    analogWrite(PIN_LASER1, LASER1_OFF_POWER);
    analogWrite(PIN_LASER2, LASER2_ON_POWER);
    XY_CircleInterpolation();
  }
  
    analogWrite(PIN_LASER1, LASER1_OFF_POWER);
    analogWrite(PIN_LASER2, LASER2_OFF_POWER);
    delete[] BufferX;//Clear the array
    delete[] BufferY;
    IsMoving_XY=false;
}
