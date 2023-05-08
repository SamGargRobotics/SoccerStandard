#pragma config(Sensor, S1,     IRfront,        sensorI2CCustom)
#pragma config(Sensor, S2,     IRback,         sensorI2CCustom)
#pragma config(Sensor, S3,     Compass,        sensorEV3_GenericI2C)
#pragma config(Motor,  motorA,          brPF,          tmotorEV3_Large, openLoop, encoder)
#pragma config(Motor,  motorB,          blPF,          tmotorEV3_Large, openLoop, encoder)
#pragma config(Motor,  motorC,          tlPF,          tmotorEV3_Large, openLoop, encoder)
#pragma config(Motor,  motorD,          trPF,          tmotorEV3_Large, openLoop, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "hitechnic-irseeker-v2.h" //IRsensor Library
#include "PF_Motors.h" // PF Motor Library
#include "PF_Motors.c" // PF Motor Library

#define COMP_MULTI 0.7 //Compass Multiplier (Sense of Direction in degrees)

//int frontStr; //Front IR sensor beacon strength detection variable
//int backStr; //Back IR sensor beacon strength detection variable
int current; //Compass value at the start of program
int target; //Target
float ballDir; //Direction of ball
//float pgmStart; //Timer Start
float compassValue; //Current compass value
float correction;
/* Basic Motor Movements */
//Still   			{    0,    0,    0,    0}
//Left    			{  100, -100, -100,  100}
//Right   			{ -100,  100,  100, -100}
//Forward 			{  100,  100, -100, -100}
//Back    			{ -100, -100,  100,  100}
//Back-Right		{ -100,    0,  100,    0}
//Forward-Left	{  100,    0, -100,    0}
//Forward-Right	{    0,  100,    0, -100}
//Back-Left			{    0, -100,    0,  100}
//Right - Works {
//Left - Works	{

float calculateCorrection(int target, int current) { // This section works fine
	float correction = (current - target) * 2.0;

	if(correction > 180) {
		correction -= 360;
	} else if(correction < -180) {
		correction += 360;
	}

	if(correction > 100) {
		correction = 100;
	} else if(correction < -100) {
		correction = -100;
	}

	return correction * COMP_MULTI;
}

int calculateBallDir(int frontStr, int backStr, int frontDir, int backDir) {
	int ballDir;
	if(frontStr >= backStr){
		switch(frontDir) {
		case 1:
			ballDir = 4;
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
			ballDir = 4;
			break;
		default:
			ballDir = 4;
		}
		} else {
		switch(backDir) { // NOTE: Find out how to get AROUND the ball, not directly at it
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
			ballDir = 17;
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

task main()
{
	tHTIRS2 irSeeker1;
	tHTIRS2 irSeeker2; //Allowing IR sensor's to function
	initSensor(&irSeeker1, IRfront);
	initSensor(&irSeeker2, IRback); //Initialising sensor's.
	initPFMotors();
	startMotorTask();
	current = SensorValue[S3];
	//SensorValue[S3] - 5 = currentBV; //Current Bottom Value
	//SensorValue[S3] + 5 = currentTV; //Current Top Value
	while(true) {
		unsigned long pgmStart;
		pgmStart = nPgmTime;
		readSensor(&irSeeker1);
		readSensor(&irSeeker2);
		compassValue = SensorValue[S3];
		//lowRange = current - 5;  //(Goal for Current Range) (Figure out how to adjust variable values manually via the code)
		//highRange = current + 5; //(Goal for Current Range)
		ballDir = calculateBallDir(irSeeker1.enhStrength, irSeeker2.enhStrength, irSeeker1.acDirection, irSeeker2.acDirection);
		displayTextLine(1, "D:%4d %4d %3d", irSeeker1.dcDirection, irSeeker1.acDirection, irSeeker1.enhDirection);
		displayTextLine(2, "D:%4d %4d %3d", irSeeker2.dcDirection, irSeeker2.acDirection, irSeeker2.enhDirection);
		displayTextLine(3, "D:%4d %4d %3d", compassValue);
		displayTextLine(4, "D:%4d %4d %3d", current);
		correction = calculateCorrection(target, current);
		if (compassValue > current) {
			while(compassValue > current) {
				motorIn.structure.A = 0;
				motorIn.structure.B = 100;
				motorIn.structure.C = 0;
				motorIn.structure.D = 100;
			}
		}
		if (compassValue < current) {
			while(compassValue < current) {
				motorIn.structure.A = 100;
				motorIn.structure.B = 0;
				motorIn.structure.C = 100;
				motorIn.structure.D = 0;
			}
		}
	}
}
