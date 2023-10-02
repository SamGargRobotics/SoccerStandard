#pragma config(Sensor, S1,     irFront,         sensorI2CCustom)
#pragma config(Sensor, S3,     compass,         sensorEV3_GenericI2C)
#pragma config(Sensor, S4,     UltraBa,         sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          frontleft,     tmotorEV3_Medium, openLoop, encoder)
#pragma config(Motor,  motorB,          frontright,    tmotorEV3_Medium, openLoop, encoder)
#pragma config(Motor,  motorC,          backright,     tmotorEV3_Medium, openLoop, encoder)
#pragma config(Motor,  motorD,          backleft,      tmotorEV3_Medium, openLoop, reversed, encoder)

#include "hitechnic-irseeker-v2.h"
#include "PF_Motors.c"

int currentSpeedA;\

task main {
	initPFMotors();
	startMotorTask();
	while(true) {
		currentSpeedA = getMotorSpeed(motorA);
		motorIn.structure.A = 50;
		motorIn.structure.B = 100 - currentSpeedA;
		motorIn.structure.C = 75 - currentSpeedA;
		motorIn.structure.D = 75 - currentSpeedA;
	}
}
