#pragma once
//Command
#define SPEED_SERIAL 57600
#define DELAY_SERIAL 200//microsencords 

//XY
#define BASE_VOLTAGE 5//

//TWO LASERS
#define LASER1_OFF_POWER 0//0-255  correspond 0-5W (when the laser max voltage is 3V)
#define LASER1_ON_POWER 153//3V
#define LASER2_OFF_POWER 0//0-255  correspond 0-5W (when the laser max voltage is 3V)
#define LASER2_ON_POWER 153//3V
#define PIN_LASER1 24
#define PIN_LASER2 26

//planter
#define SPEED_XY 3000 //  mm per seconds
#define DIS_F_THETA 571.5//distance between F-Theta and plane =571.5mm
#define DIS_XYMOTOR 9//Distance between X and Y scanner = 9mm
#define RAD_MG_MAX 0.0873 //the Mirror galvanometer max rad  (5/180*pi)
#define CYCLE_INTERPOLARION 1000// per seconds

//Steppers
#define STEP_PER_CIRCLE 4//4cm per circle
#define IMPLUSE_PER_CIRCLE 2048//one circle needs 2048 impluses

//LASER CHANGING
#define StepperA_IN1 8
#define StepperA_IN2 9
#define StepperA_IN3 10
#define StepperA_IN4 11
#define StepperA_LASER1_POSITION 5.3
#define StepperA_LASER2_POSITION 1.3
#define StepperB_LASER1_POSITION -2.0
#define StepperB_LASER2_POSITION -15.4
//FOCUS ADJUSTMENT
#define StepperB_IN5 4
#define StepperB_IN6 5
#define StepperB_IN7 6
#define StepperB_IN8 7
