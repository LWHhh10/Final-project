//Receive commands from the computer and explian them 

#include "command.h"

String GCode_String;
char Serial_Char;
int KeyStart_int;
int KeyEnd_int;
float GCode_Arry_Command[6];//X,Y,A,B,I,K
bool NeedMove_bool_Command[5];//XY(line),XY(arc1),XY(arc2),A,B
bool NeedOpen_Laser;
bool Comment_Command;

struct
{
  String S;
  float F;
}KeyVal_struct;

void Command_Init()
{
  Serial.begin(SPEED_SERIAL);
}

void Command_Get()
{
  /**
  * Loop while serial characters are incoming and the queue is not full
  */
  while (Serial.available() > 0)
  {
    Serial_Char = Serial.read();
    if (Serial_Char == ';')
      Comment_Command = true;
    if (Comment_Command == false)
      GCode_String += Serial_Char;
    delayMicroseconds(DELAY_SERIAL);
  } 
 //Serial.println(GCode_String);
  
  if (GCode_String.length() > 0)//if the serial port gets the command 
  {
    if (Command_Key('G')) //if the first character is G
    {
      switch ((int)KeyVal_struct.F)//judge the parameter after the character
      {
      case 1://G1: linear movement    
        if (Command_Key('X')) 
        { 
          GCode_Arry_Command[0] = KeyVal_struct.F;
          NeedMove_bool_Command[0]=true;
          //Serial.println("X");//send information to the console 
          //Serial.println(GCode_Arry_Command[0]);
        }
        if (Command_Key('Y')) 
        {
          GCode_Arry_Command[1] = KeyVal_struct.F;
           NeedMove_bool_Command[0]=true;
          //Serial.println("Y");
          //Serial.println(GCode_Arry_Command[1]);
        }
        else if (Command_Key('A')) 
        {
          GCode_Arry_Command[2] = KeyVal_struct.F;
           NeedMove_bool_Command[1]=true;
         // Serial.println("A");
         // Serial.println(GCode_Arry_Command[2]);
        }
        else if (Command_Key('B')) 
        {
          GCode_Arry_Command[3] = KeyVal_struct.F;
           NeedMove_bool_Command[2]=true;
         // Serial.println("B");
         // Serial.println(GCode_Arry_Command[3]);
        } 
      break;
      case 2://G2: circular movement (clockwise)
      
         if (Command_Key('X')) 
         {
          GCode_Arry_Command[0] = KeyVal_struct.F;
          NeedMove_bool_Command[3]=true;
          //Serial.println("X1");
         // Serial.println(GCode_Arry_Command[0]);
        }
        if (Command_Key('Y')) 
        {
          GCode_Arry_Command[1] = KeyVal_struct.F;
          NeedMove_bool_Command[3]=true;
          //Serial.println("Y1");
          //Serial.println(GCode_Arry_Command[1]);
        }
        if (Command_Key('I')) 
        {
          GCode_Arry_Command[4] = KeyVal_struct.F;
           NeedMove_bool_Command[3]=true;
         // Serial.println("I");
         // Serial.println(GCode_Arry_Command[4]);
        }
        if (Command_Key('K')) 
        {
          GCode_Arry_Command[5] = KeyVal_struct.F;
           NeedMove_bool_Command[3]=true;
         // Serial.println("K");
         // Serial.println(GCode_Arry_Command[5]);
        }   
      break;
      case 3:////G2: circular movement (anticlockwise)
        if (Command_Key('X')) 
        {
          GCode_Arry_Command[0] = KeyVal_struct.F;
          NeedMove_bool_Command[4]=true;
          //Serial.println("X");
          //Serial.println(GCode_Arry_Command[0]);
        }
        if (Command_Key('Y')) 
        {
          GCode_Arry_Command[1] = KeyVal_struct.F;
          NeedMove_bool_Command[4]=true; 
          //Serial.println("Y");
          //Serial.println(GCode_Arry_Command[1]);
        }
        if (Command_Key('I')) 
        {
          GCode_Arry_Command[4] = KeyVal_struct.F;
          NeedMove_bool_Command[4]=true;
          //Serial.println("I");
          //Serial.println(GCode_Arry_Command[4]);
        }
        if (Command_Key('K')) 
        {
          GCode_Arry_Command[5] = KeyVal_struct.F;
          NeedMove_bool_Command[4]=true;
          //Serial.println("K");
          //Serial.println(GCode_Arry_Command[5]);
      }
      break;
      } 
      if (Command_Key('E')) {
        NeedOpen_Laser = true;
        //Serial.println("E");
      }
    }   
   }
  
  Comment_Command = false;
  GCode_String = "";
  KeyStart_int = 0;
  KeyEnd_int = 0;
}

inline bool Command_Key(char key) 
{
  KeyStart_int = GCode_String.indexOf(key, KeyStart_int);
  if (KeyStart_int >= 0) //when no found keyword ,the indexOf function return -1
  {   
        KeyEnd_int = GCode_String.indexOf(' ', KeyEnd_int + 1);//Get the position of the first space
        if (KeyEnd_int >= 0) 
        {
          KeyVal_struct.S = GCode_String.substring(KeyStart_int + 1, KeyEnd_int);//Get the parameter behind the related command 
         }    
        else 
        {
          KeyEnd_int = GCode_String.indexOf('*', KeyEnd_int + 1);
          if (KeyEnd_int >= 0)
          {
            KeyVal_struct.S = GCode_String.substring(KeyStart_int + 1, KeyEnd_int);
          }
          else 
          {
            KeyVal_struct.S = GCode_String.substring(KeyStart_int + 1);
          }
        }
    KeyVal_struct.F = KeyVal_struct.S.toFloat();
    return true;
   }
  else 
  {
    KeyStart_int = 0; //when KeyStart_int == 0 ,the fun : String.indexOf(key, KeyStart_int) have not do anything
    return false;
  }
}

