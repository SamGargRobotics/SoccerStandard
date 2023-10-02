#pragma config(Sensor, S1, irBack, sensorI2CCustom)
#pragma config(Sensor, S3, compass, sensorEV3_GenericI2C)
#pragma config(Sensor, S2, UltraSi, sensorEV3_Ultrasonic)
#pragma config(Sensor, S4, UltraBa, sensorEV3_Ultrasonic)
#pragma config(Motor, motorA, frontleft, tmotorEV3_Medium, openLoop, encoder)
#pragma config(Motor, motorB, frontright, tmotorEV3_Medium, openLoop, encoder)
#pragma config(Motor, motorC, backright, tmotorEV3_Medium, openLoop, encoder)
#pragma config(Motor, motorD, backleft, tmotorEV3_Medium, openLoop, reversed, encoder)

#include "hitechnic-irseeker-v2.h"
#include "PF_Motors.h"
#include "PF_Motors.c"

#define COMP_MULTI 0.05 //1:.075, 2:
int motorSpeeds[18][4] = { //Defining movement
	{ -100, -100,  100,  100}, // Forward
	{ -100,   10, -100,   10}, // Forward-Right
	{ -100,  100,  100, -100}, // Right
	{    0,  100,    0, -100}, // Backward
	{  100,  100, -100, -100}, // Backward
	{  100,    0, -100,    0}, // Backward
	{  100, -100, -100,  100}, // Left
	{   10, -100,   10,  100}, // Forward-Left
	{    0,    0,    0,    0}, // Still
	{  100,  100, -100, -100}, // Forward
	{  100,    0, -100,    0}, // Forward-Right
	{  100, -100, -100,  100}, // Right
	{    0, -100,    0,  100}, // Backward-Right
	{ -100, -100,  100,  100}, // Backward
	{ -100,    0,  100,    0}, // Backward-Left
	{ -100,  100,  100, -100}, // Left
	{    0,  100,    0, -100}, // Forward-Left
	{    0,    0,    0,    0}, // Still
};
int motorVal[4] = {0, 0, 0, 0};
float max(float a, float b) {
	return a > b? a : b;
}
float caluculateCorrection(int target, int current) { //Coverting values for compass correct
	float correction = (target - current) * 2;
	if(correction > 180) {
		correction -= 360;
	}
	else if(correction < -180) {
		correction += 360;
	}
	correction = correction * -1;
	correction *= COMP_MULTI;
	return correction;
}
int calculateBallDir(int frontStr, int backStr, int frontDir, int backDir) { //Change 1 & 10 if IR messes up
	int ballDir;
	if(frontStr >= backStr){
		switch(frontDir) {
		case 1:
			ballDir = 6;
			break;
		case 2:
			ballDir = 5;
			break;
		case 3:
			ballDir = 6;
			break;
		case 4:
			ballDir = 7;
			break;
		case 5:
			ballDir = 0;
			break;
		case 6:
			ballDir = 1;
			break;
		case 7:
			ballDir = 2;
			break;
		case 8:
			ballDir = 3;
			break;
		case 9:
			ballDir = 6;
			break;
		default:
			ballDir = 4;
		}
	}
	else {
		switch(backDir) {
		case 1:
			ballDir = 16;
			break;
		case 2:
			ballDir = 17;
			break;
		case 3:
			ballDir = 9;
			break;
		case 4:
			ballDir = 10;
			break;
		case 5:
			ballDir = 11;
			break;
		case 6:
			ballDir = 12;
			break;
		case 7:
			ballDir = 9;
			break;
		case 8:
			ballDir = 10;
			break;
		case 9:
			ballDir = 11;
			break;
		default:
			ballDir = 9;
		}
	}
	return ballDir;
}
void moving(int ballDir, float correction) { //Hard code
	for(int i = 0; i < 4; i++) {
		motorVal[i] = motorSpeeds[ballDir][i];
		motorVal[i] -= correction;
	}
	float highestVal = max(abs(motorVal[0]), abs(motorVal[1]));
	highestVal = max(highestVal, abs(motorVal[2]));
	highestVal = max(highestVal, abs(motorVal[3]));
	if(highestVal > 100) {
		if(highestVal != 0) {
			for(int i = 0; i <= 3; i++) {
				motorVal[i] /= highestVal;
				motorVal[i] *= 100;
			}
		}
	}
	motorIn.structure.A = motorVal[0]; //Convert
	motorIn.structure.B = motorVal[1];
	motorIn.structure.C = motorVal[2];
	motorIn.structure.D = motorVal[3];
}

task main()
{
	tHTIRS2 irSeeker1;
	initSensor(&irSeeker1, irBack);
	float correction;
	int ballDir;
	int target = SensorValue[S3];
	int current = target;
	for(int i = 0; i < 4; i++) {
		motorIn.array[i] = 0;
	}
	initPFMotors();
	startMotorTask();
	while(true) {
		readsensor(&irSeeker1)
		current = SensorValue[S3];
		ballDir = calculateBallDir(irSeeker1.enhStrength, irSeeker1.acDirection);
    correction = caluculateCorrection(target, current);
    moving(ballDir, correction);
  }
}
