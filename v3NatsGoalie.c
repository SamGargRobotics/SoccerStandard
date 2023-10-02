#pragma config(Sensor, S1, irFront, sensorI2CCustom)
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

int speed = 100;
#define COMP_MULTI 0.05 //1:.075, 2:
int motorSpeeds[18][4] = {
	{ -speed,-speed,-speed,-speed}, // Forward - Works
	{  speed,     0, speed,     0}, // Forward-Right - More Forward
	{  speed,-speed, speed,-speed}, // Right - Works
	{  speed,     0, speed,     0}, // Backward-Right
	{  speed, speed, speed, speed}, // Backward
	{ -speed,     0, -speed,    0}, // Backward-Left
	{ -speed, speed,-speed, speed}, // Left
	{      0, speed,     0, speed}, // Forward-Left - More Forward
	{      0,     0,     0,     0}, // Still
	{ -speed,-speed,-speed,-speed}, // Forward - Works
	{  speed,-speed, speed,-speed}, // Right - Works
	{  speed,     0, speed,     0}, // Backward-Right
	{  speed, speed, speed, speed}, // Backward
	{ -speed,     0, -speed,    0}, // Backward-Left
	{ -speed, speed,-speed, speed}, // Left
	{      0, speed,     0, speed}, // Forward-Left - More Forward
	{      0,     0,     0,     0}, // Still
};
int motorVal[4] = {0, 0, 0, 0};
float max(float a, float b) {
	return a > b? a : b;
}
float calculateCorrection(int target, int current) { //Converting values for compass correction
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
int calculateBallDir(int frontStr, int frontDir) { //Change 1 & 10 if IR messes up
	int ballDir;
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
	initSensor(&irSeeker1, irFront);
	float correction;
	int ballDir;
	int target = SensorValue[compass];
	int current = target;
	for(int i = 0; i < 4; i++) {
		motorIn.array[i] = 0;
	}
	initPFMotors();
	startMotorTask();
	while(true) {
		readSensor(&irSeeker1);
		current = SensorValue[compass];
		ballDir = calculateBallDir(irSeeker1.enhStrength, irSeeker1.acDirection);
		moving(ballDir, correction);
	}
}
