#pragma config(Sensor, in1,    mgPot,          sensorPotentiometer)
#pragma config(Sensor, in2,    rLiftPot,       sensorPotentiometer)
#pragma config(Sensor, in3,    lLiftPot,       sensorPotentiometer)
#pragma config(Sensor, in4,    yaw,            sensorNone)
#pragma config(Sensor, dgtl1,  rightEn,        sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  leftEn,         sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  rotClaw,        sensorDigitalOut)
#pragma config(Sensor, dgtl6,  clawRelease,    sensorDigitalOut)
#pragma config(Sensor, dgtl11, LCD,            sensorNone)
#pragma config(Motor,  port1,           fR,            tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           rLiftMG,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           lTopLift,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           rTopLift,      tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           bL,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           lLiftMG,       tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           rBotLift,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           fL,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           lBotLift,      tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port10,          bR,            tmotorVex393_HBridge, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX2)

#pragma competitionControl(Competition)

#include "Vex_Competition_Includes.c"

//#include "resources\tunes.h"

//Front right drive is in port 1
//Right mobile goal lift is in port 2
//Left lift top is in port 3
//Right lift top is in port 4
//Back left drive is in port 5
//Left mobile goal lift is in port 6
//Right lift bottom is in port 7
//Front left drive is in port 8
//Left lift bottom is in port 9
//Back right drive is in port 10



/******************************************************************
/*
/*											 USER CONSTANTS
/*
*******************************************************************/



const unsigned int STACK_HEIGHT = 42;				//Height for lift to raise with each cone stack
const unsigned int BASE_HEIGHT = 350;				//Height for lift with a mobile goal

const unsigned int THRESHOLD = 35;					//Threshold for motors to work
const unsigned int LIFT_MG_TOL = 2;					//Mobile goal lift tolerance for p-tuning
const unsigned int LIFT_TOL = 30;						//Lift tolerance for p-tuning

const unsigned int LIFT_MG_ACC_TOL = 50;		//The thousandths accuracy of the mobile goal lift
const unsigned int LIFT_ACC_TOL = 50;				//The thousandths accuracy of the lift

const unsigned int LIFT_MG_MAX = 2230;			//The maximum for the left mobile goal lift potentiometer (lift deployed)
const unsigned int LIFT_MG_MIN = 4040;			//The minimum for the left mobile goal lift potentiometer (lift undeployed)

const unsigned int L_LIFT_MAX = 2020;				//The maximum for the left lift potentiometer
const unsigned int L_LIFT_MIN = 480;				//The minimum for the left lift potentiometer
const unsigned int R_LIFT_MAX = 1840;				//The maximum for the right lift potentiometer
const unsigned int R_LIFT_MIN = 450;				//The minimum for the right lift potentiometer

const float P_LIFT_MG = 0.1;								//The p-value for the mobile goal lift's movement
const float P_LIFT = 0.15;									//The p-value for the lift's movement (proportional)
const float I_LIFT = 0;											//The i-value for the lift's movement (integral)
const float D_LIFT = 0;											//The d-value for the lift's movement (derivative)



//Enumerated type used for targets array in autonomous
enum targetTypes {
	CLAW				= 0,
	LIFT_MG			= 1,
	ROT					= 2,
	LIFT				= 3,
	N_TARGETS		= 4
};

//Global variables used for toggle switches
unsigned int stackCountG = 0;
bool skillsToggleStateG = false;				//Variable to check if skills is toggled
//bool skillsPressedG = false;						//Variable to check if the skills button is pressed
bool fieldSideToggleStateG = false;			//Variable to check which field side the robot is on
//bool fieldSidePressedG = false;					//Variable to check if the field side button is pressed



/******************************************************************
/*
/*											 FUNCTIONS
/*
*******************************************************************/



/* A generic conversion function for converting from a thousandths to a potentiometer value
*  for either the mobile goal lift or left/right lift */
float thousandthsToPot(targetTypes type, int thousandths, tDirections side = left) {
	//Stores desired extrema as floats to avoid later unit conversions
	float max = (type == LIFT_MG) ? LIFT_MG_MAX :
	((side == left) ? L_LIFT_MAX : R_LIFT_MAX);
	float min = (type == LIFT_MG) ? LIFT_MG_MIN :
	((side == left) ? L_LIFT_MIN : R_LIFT_MIN);
	//Performs the calcualations and returns the desired value
	return (min + (max - min) * ((float)thousandths / 1000));
}

/* A generic conversion function for converting from a side of a system that uses a
*  potentiometer to a thousandths of a potentiometer value */
float potToThousandths(targetTypes type, tDirections side = left) {
	//Stores desired extrema as floats to avoid later unit conversions
	float max = (type == LIFT_MG) ? LIFT_MG_MAX :
	((side == left) ? L_LIFT_MAX : R_LIFT_MAX);
	float min = (type == LIFT_MG) ? LIFT_MG_MIN :
	((side == left) ? L_LIFT_MIN : R_LIFT_MIN);
	//Stores the current potentiometer value
	float potVal = (type == LIFT_MG) ? SensorValue[mgPot] :
	((side == left) ? SensorValue[lLiftPot] : SensorValue[rLiftPot]);
	//Performs the calculations and returns the desired value
	return 1000 * ((potVal - min) / (max - min));
}

/* Basic conversion for converting inches driven to encoder ticks that can be used for
*  p-tuning */
float inchesToEncoderTicks(float inches) {
	float encoderTicks = 90 * PI * inches;
	return encoderTicks;
}

/* P-tunes the mobile goal lift to a specified mobile goal lift target, which is a thousandths
*  value along the range of the claw.
*  This version of the function is used during autonomous in conjunction with an array */
void pTuneMGLift(int liftMGTarget) {
	if(liftMGTarget > 1000)			//The mobile goal target is corrected if it is above the maximum
		liftMGTarget = 1000;
	else if(liftMGTarget < 0)		//The mobile goal target is corrected if it is below the minimum
		liftMGTarget = 0;
	//Finds the error mutplied by the p-value of the mobile goal lift
	int lLiftMGError = P_LIFT_MG * (thousandthsToPot(LIFT_MG, liftMGTarget, left) - SensorValue[mgPot]);
	int rLiftMGError = P_LIFT_MG * (thousandthsToPot(LIFT_MG, liftMGTarget, right) - SensorValue[mgPot]);
	//The mobile goal lift only p-tunes if the error is above the tolerance
	motor[lLiftMG] = (abs(lLiftMGError) >= LIFT_MG_TOL) ? lLiftMGError : 0;
	motor[rLiftMG] = (abs(rLiftMGError) >= LIFT_MG_TOL) ? rLiftMGError : 0;
}

/* SEE void pTuneMGLift(int liftMGTarget)
*  The equivalent of the above function, but the target is passed by reference rather than by value.
*  This version of the function is used during driver mode in conjunction with a local variable */
void pTuneMGLift(int* liftMGTarget) {
	if(*liftMGTarget > 1000)
		*liftMGTarget = 1000;
	else if(*liftMGTarget < 0)
		*liftMGTarget = 0;
	int lLiftMGError = P_LIFT_MG * (thousandthsToPot(LIFT_MG, *liftMGTarget) - SensorValue[mgPot]);
	int rLiftMGError = P_LIFT_MG * (thousandthsToPot(LIFT_MG, *liftMGTarget) - SensorValue[mgPot]);
	motor[rLiftMG] = (abs(lLiftMGError) >= LIFT_MG_TOL) ? lLiftMGError : 0;
	motor[lLiftMG] = (abs(rLiftMGError) >= LIFT_MG_TOL) ? rLiftMGError : 0;
}

/* This function sets the claw release, assigning the claw release target of an array as well.*/
void setClawRelease(int* targets, bool clawTarget) {
	targets[CLAW] = clawTarget;
	SensorValue[clawRelease] = clawTarget;
}

/* Opens or closes the claw in accordance with the input
* ARGUMENT: clawTarget: true is open, closed is false
* Written as a function to improve readability. */
void setClawRelease(bool clawTarget) {
	SensorValue[clawRelease] = clawTarget;
}

/* This function sets the rotation of the claw, assigning the claw
* target of an array as well. */
void setRot(int* targets, bool rotTarget) {
	targets[ROT] = rotTarget;
	SensorValue[rotClaw] = rotTarget;
}

/* Rotates the claw in accordance with the input
* ARGUMENT: rotTarget: true is facing forward, closed is inward
* Written as a function to improve readability. */
void setRot(bool rotTarget) {
	SensorValue[rotClaw] = rotTarget;
}

/* P-tunes the lift to a specified lift target, which is a thousandths value
*  along the range of the lift.
*  This version of the function is used during autonomous in conjunction with an array */
void pTuneLift(int liftTarget) {
	if(liftTarget > 1000)
		liftTarget = 1000;
	else if(liftTarget < 0)
		liftTarget = 0;

	int lError = thousandthsToPot(LIFT, liftTarget, left) - SensorValue[lLiftPot];
	int rError = thousandthsToPot(LIFT, liftTarget, right) - SensorValue[rLiftPot];

	int lDrive = P_LIFT * lError;
	int rDrive = P_LIFT * rError;

	motor[lTopLift] = motor[lBotLift] = ((abs(lError) >= LIFT_TOL) && (abs(lDrive) > 20)) ? lDrive : 0;
	motor[rTopLift] = motor[rBotLift] = ((abs(rError) >= LIFT_TOL) && (abs(rDrive) > 20)) ? lDrive : 0;
}

/* PID version of the previous function that is used for user control and is passed by reference
*  than by value.
*  This function is used during driver mode in conjunction with a local variable */
void pidTuneLift(int* liftTarget, int* lIntegral, int* rIntegral, int* lPrevError, int* rPrevError) {
	if(*liftTarget > 1000)
		*liftTarget = 1000;
	else if(*liftTarget < 0)
		*liftTarget = 0;

	//Finds the errors between the target and the actual value
	int lError = thousandthsToPot(LIFT, *liftTarget, left) - SensorValue[lLiftPot];
	int rError = thousandthsToPot(LIFT, *liftTarget, right) - SensorValue[rLiftPot];
	//Continuously adds the error to the integral count
	*lIntegral += lError;
	*rIntegral += rError;
	//Keeps track of the difference between the error and the previous error
	int lDerivative = lError - *lPrevError;
	int rDerivative = rError - *rPrevError;
	//Resets the previous error
	*lPrevError = lError;
	*rPrevError = rError;

	//Sets the drive values equal to their respective variable multiplied by the tuning value
	int lDrive = (P_LIFT * lError) + (I_LIFT * (*lIntegral)) + (D_LIFT * lDerivative);
	int rDrive = (P_LIFT * rError) + (I_LIFT * (*rIntegral)) + (D_LIFT * rDerivative);

	//Resets the integral if the drive isn't PID tuning
	if(abs(lError) < LIFT_TOL || (abs(lDrive) > 20)) {
		*lIntegral = 0;
	}
	if(abs(rError) < LIFT_TOL || (abs(lDrive) > 20)) {
		*rIntegral = 0;
	}

	/*writeDebugStreamLine("lDrive = %d ", lDrive);
	writeDebugStream("rDrive = %d ", rDrive);
	writeDebugStream("LKp = %d ", (P_LIFT * lError));
	writeDebugStream("RKp = %d ", (P_LIFT * rError));
	writeDebugStream("LKd = %d ", (D_LIFT * lDerivative));
	writeDebugStream("RKd = %d ", (D_LIFT * rDerivative));*/
	datalogAddValueWithTimeStamp(0, lError);
	datalogAddValueWithTimeStamp(1, lDrive);
	datalogAddValueWithTimeStamp(2, P_LIFT * lError);
	datalogAddValueWithTimeStamp(3, D_LIFT * lDerivative);
	datalogAddValueWithTimeStamp(4, rError);
	datalogAddValueWithTimeStamp(5, rDrive);
	datalogAddValueWithTimeStamp(6, P_LIFT * rError);
	datalogAddValueWithTimeStamp(7, D_LIFT * rDerivative);

	//Doesn't move the motors if they are too small or it's within the tolerance to reduce motor whine
	motor[lTopLift] = motor[lBotLift] = ((abs(lError) >= LIFT_TOL) && (abs(lDrive) > 30)) ? lDrive : 0;
	motor[rTopLift] = motor[rBotLift] = ((abs(rError) >= LIFT_TOL) && (abs(rDrive) > 30)) ? rDrive : 0;
}

/* The robot drives a set distance in encoder ticks, given by enTarget.
*  It uses a formula to drive at a speed scaled to the difference between
*  the current displacement and the final displacement.  It then drives
*  backwards to counteract any additional drift.  It p-tunes while doing so */
void waitDistance(const int* const targets, int enTarget) {
	enTarget = inchesToEncoderTicks(enTarget);

	SensorValue[rightEn] = 0;		//Encoders zeroed
	SensorValue[leftEn] = 0;
	clearTimer(T1);

	int drive = 0;							//Stores the speed the drive should travel at

	int a = 18;									//Variable that reduces drift when increased but also lowers speed

	/* The robot uses a formula to scale speed to position if the velocity would
	*  be within the threshold to start driving, but otherwise sets the speed equal
	*  to the minimum threshold otherwise */
	while(abs(SensorValue[leftEn]) < abs(enTarget) && time1[T1] < 5000) {
		drive = sgn(enTarget) * (a * log(-abs(SensorValue[leftEn]) + abs(enTarget)) + (127 - (a * log(abs(enTarget)))));
		if(sgn(enTarget) > 0 && drive < THRESHOLD) {
			drive = THRESHOLD;
		}
		else if(sgn(enTarget) < 0 && drive > -THRESHOLD) {
			drive = -THRESHOLD;
		}
		motor[bR] = motor[fR] = motor[bL] = motor[fL] = drive;
		pTuneMGLift(targets[LIFT_MG]);		//P-tunes the claw and the lifts while driving
		pTuneLift(targets[LIFT]);
	}

	//Motors drive backward briefly in order to counteract additional drift
	motor[bR] = motor[fR] = motor[bL] = motor[fL] = -sgn(enTarget) * THRESHOLD;
	wait1Msec(110);
	motor[bR] = motor[fR] = motor[bL] = motor[fL] = 0;
}

/* A function used in autonomous that tells the robot to rotate a certain amount
*  in tenths of a degree, using the VEX gyro */
void waitRotate(const int* const targets, int degrees10) {
	//Multiplies the limit switch for the fieldside by the degrees, so that it is on the left side if the switch is down
	SensorValue[yaw] = 0;
	clearTimer(T1);
	degrees10 *= ((fieldSideToggleStateG) ? 1 : -1);

	int drive = 0;							//Contains the drive speed

	int a = 15;									//The intensity of the drive functoin

	/* Enacts the rotation to a set tenths of a degree (degrees10) while tuning the lift
	*  The rotation speed is tuned to decrease on a slope from 127 to the treshold.
	*  If it is past the threshold, it is set equal to the treshold, but with each side
	*  rotating in a different direction */
	if(degrees10 > 0) {
		while(SensorValue[yaw] < degrees10 && time1(T1) < 5000) {
			drive = sgn(degrees10) * (a * log(-abs(SensorValue[yaw]) + abs(degrees10)) + (90 - (a * log(abs(degrees10)))));
			if(abs(drive) > 90) {
				drive = sgn(degrees10) * 90;
			}
			else if(sgn(degrees10) > 0 && drive < THRESHOLD) {
				drive = THRESHOLD;
			}
			else if(sgn(degrees10) < 0 && drive > -THRESHOLD) {
				drive = -THRESHOLD;
			}
			motor[bR] = motor[fR] = drive;		//The right side moves forwards
			motor[bL] = motor[fL] = -drive;		//The left side moves backwards
			pTuneMGLift(targets[LIFT_MG]);		//P-tunes the claws and the lift while rotating
			pTuneLift(targets[LIFT]);
		}
	}
	else {
		while(SensorValue[yaw] > degrees10) {
			drive = sgn(degrees10) * (a * log(-abs(SensorValue[yaw]) + abs(degrees10)) + (90 - (a * log(abs(degrees10)))));
			if(abs(drive) > 90) {
				drive = sgn(degrees10) * 90;
			}
			else if(sgn(degrees10) > 0 && drive < THRESHOLD) {
				drive = THRESHOLD;
			}
			else if(sgn(degrees10) < 0 && drive > -THRESHOLD) {
				drive = -THRESHOLD;
			}
			motor[bR] = motor[fR] = drive;		//The right side moves forwards
			motor[bL] = motor[fL] = -drive;		//The left side moves backwards
			pTuneMGLift(targets[LIFT_MG]);		//P-tunes the claws and the lift while rotating
			pTuneLift(targets[LIFT]);
		}
	}
	//Turns off the drive motors when the rotation is complete
	motor[bR] = motor[fR] = -sgn(degrees10) * THRESHOLD;
	motor[bL] = motor[fL] = sgn(degrees10) * THRESHOLD;
	wait1Msec(110);
	motor[bR]  = motor[fR] = motor[bL] = motor[fL] = 0;
}

/* A somewhat general function used in autonomous that changes the contents
*  of the current target values for the lifts and claw, and then pTunes both
*  the lifts and claw with their new target values */
void waitActions(int* targets, int liftMGTarget, int clawReleaseTarget, int rotTarget, int liftTarget) {
	//Assigns the contents of targets to the new lift and claw target values
	targets[CLAW] = clawReleaseTarget;
	targets[LIFT_MG] = liftMGTarget;
	targets[ROT] = rotTarget;
	targets[LIFT] = liftTarget;
	//P-tunes the lifts and the claw
	setClawRelease(targets[CLAW]);
	pTuneMGLift(targets[LIFT_MG]);
	setRot(targets[ROT]);
	pTuneLift(targets[LIFT]);
}

/* Sets targets equal to the new values, then continues to test to see if the mobile goal lift is within
*  the range specified by tolerance, which is defined by the user or set equal to the default tolerance */
void testMGLift(int* targets, int liftMGTarget, int clawReleaseTarget, int rotTarget, int liftTarget, int accuracy = LIFT_MG_ACC_TOL) {
	do {
		waitActions(targets, liftMGTarget, clawReleaseTarget, rotTarget, liftTarget);
	} while(potToThousandths(LIFT_MG) < liftMGTarget - accuracy || potToThousandths(LIFT_MG) > liftMGTarget + accuracy);
}

/* Specific function for p-tuning just the mobile goal lift */
void setMGLift(int* targets, int liftMGTarget, int accuracy = LIFT_MG_ACC_TOL) {
	testMGLift(targets, liftMGTarget, targets[CLAW], targets[ROT], targets[LIFT], accuracy);
}

/* Sets targets equal to the new values, then continues to test to see if the lift is within
the range specified by tolerance, which is defined by the user or set equal to the default tolerance */
void testLift(int* targets, int liftMGTarget, int clawReleaseTarget, int rotTarget, int liftTarget, int accuracy = LIFT_ACC_TOL) {
	do {
		waitActions(targets, liftMGTarget, clawReleaseTarget, rotTarget, liftTarget);
	} while(potToThousandths(LIFT) < liftTarget - accuracy || potToThousandths(LIFT) > liftTarget + accuracy);
}

/* Specific function for p-tuning just the lift */
void setLift(int* targets, int liftTarget, int accuracy = LIFT_ACC_TOL) {
	testLift(targets, targets[LIFT_MG], targets[CLAW], targets[ROT], liftTarget, accuracy);
}

/* Function for resetting the stackcount */
void stackReset() {
	int* stackCountL = &stackCountG;											//Setup local variable based on the global variable
	*stackCountL = 0;
}

/* Function for auto stacking a cone */
void stackCone(int* targets) {

	int* stackCountL = &stackCountG;												//Setup a local variable and increment it by one
	*stackCountL++;
	int currentStackHeight = *stackCountL * STACK_HEIGHT;		//Create a variable for the current height of the cone stack

	setLift(targets, 0);																		//Lowers the lift to grab the cone
	wait1Msec(.5);
	setLift(targets, BASE_HEIGHT + 400 + currentStackHeight);//Raises the lift to the right height
	setRot(targets, false);																	//Rotates the claw downwards
	setLift(targets, BASE_HEIGHT + currentStackHeight);			//Lowers the cone onto the mobile goal
	setClawRelease(targets, true);													//Releases the cone
	wait1Msec(.5);
	setClawRelease(targets, false);													//Closes the hook again
	setLift(targets, BASE_HEIGHT + 400 + currentStackHeight);//Raises the lift again
	setRot(targets, true);																	//Rotates the claw forwards
	setLift(targets, 0);																		//Lowers the lift

	playTone(1050, 50);																			//Plays a tone when finished
}

/* Eliminates motor whine on the drive during user control */
int axesFix(int remote) {
	if(abs(remote) >= THRESHOLD) 		//Returns 0 if the value does not pass the TRESHOLD constant
		return remote;
	else
		return 0;
}



/******************************************************************
/*
/*											 TASKS
/*
*******************************************************************/


/*
//Task for displaying the battery on the LCD Screen
task displayBattery()
{
bool toggleSwitch = false;					//Toggle switch variable
string mainBattery, backupBattery;	//Setup two strings
while(1) {
//Toggle switch code for turning on and off
if(nLCDButtons) {
toggleSwitch = !toggleSwitch;
wait1Msec(500);
}
if(toggleSwitch) {
bLCDBacklight = true;     		 	//Turn on LCD Backlight
clearLCDLine(0);         		   	//Clear line 1 of the LCD
clearLCDLine(1);          		 	//Clear line 2 of the LCD

//Display the Primary Robot battery voltage
displayLCDString(0, 0, "Primary: ");
sprintf(mainBattery, "%1.2f%c", nImmediateBatteryLevel/1000.0,'V'); //Build the value to be displayed
displayNextLCDString(mainBattery);

//Display the Backup battery voltage
displayLCDString(1, 0, "Backup: ");
sprintf(backupBattery, "%1.2f%c", BackupBatteryLevel/1000.0, 'V');	//Build the value to be displayed
displayNextLCDString(backupBattery);

//Short delay for the LCD refresh rate
wait1Msec(100);
}
else {
bLCDBacklight = false;     		 	//Turn off LCD Backlight
clearLCDLine(0);         		   	//Clear line 1 of the LCD
clearLCDLine(1);          		 	//Clear line 2 of the LCD
}
}
}
*/

/*
task calibrateGyro()
{
SensorType[yaw] = sensorNone;			//Resets the gyroscope
wait1Msec(1100);									//Provides time for calibration
SensorType[yaw] = sensorGyro;			//Reassigns the yaw gyroscope's sensor type
wait1Msec(1100);									//Waits for the gyro to calibrate

SensorValue[rightEn] = 0;					//Zeroes the right encoder
SensorValue[leftEn] = 0;					//Zeroes the left encoder
playTone(1200, 50);								//Plays a tone when calibration is finished to alert the operator
}
*/



/******************************************************************
/*
/*											 AUTONOMOUS CODE
/*
*******************************************************************/



void pre_auton()
{
	bStopTasksBetweenModes = false;
	/*	startTask(calibrateGyro);

	//Sets up local variables based on the global variables for toggle switches
	bool* fieldSideToggleStateL = &fieldSideToggleStateG;
	bool* fieldSidePressedL = &fieldSidePressedG;
	bool* skillsToggleStateL = &skillsToggleStateG;
	bool* skillsPressedL = &skillsPressedG;

	bLCDBacklight = true;			//Turn the backlight for the LCD On

	//Default messasges
	displayLCDString(0, 0, "Auton Settings:");
	displayLCDString(1, 0, "L      NoSk     ");

	while(1) {
	//Check for the left button being pressed if the button isn't already pressed
	if((nLCDButtons & leftBtnVEX) && !(*fieldSidePressedL)) {
	*fieldSideToggleStateL = !(*fieldSideToggleStateL);				//Change the toggle state
	*fieldSidePressedL = true;																//Note that the button is pressed
	}
	//If the left button isn't pressed
	else if (!(nLCDButtons & leftBtnVEX)) {
	*fieldSidePressedL = false;																//Note that the button is not pressed
	}

	//Same toggle switch with the skills button
	if((nLCDButtons & centerBtnVEX) && !(*skillsPressedL)) {
	*skillsToggleStateL = !(*skillsToggleStateL);
	*skillsPressedL = true;
	}
	else if (!(nLCDButtons & centerBtnVEX)) {
	*skillsPressedL = false;
	}

	//Print various messages depending on the button state
	if(*fieldSideToggleStateL)
	displayLCDString(1, 0, "R");
	else if(!(*fieldSideToggleStateL))
	displayLCDString(1, 0, "L");
	if(*skillsToggleStateL)
	displayLCDString(1, 7, "NoSk");
	else if(!(*skillsToggleStateL))
	displayLCDString(1, 7, "Sk  ");
	}
	*/
}

task autonomous()
{
	//targets is an array of integers that stores the target values for the mglift, claw, rotation, and lift
	int targets[N_TARGETS] = {1000, false, false, 0};	//NOTE- CHANGE THESE DEFAULTS LATER!!!

	// Skills Autonomous
	if(skillsToggleStateG) {
	}

	//Regular Autonomous
	else {
	}
}



/******************************************************************
/*
/*											 USER CONTROL
/*
*******************************************************************/



task usercontrol()
{
	//	startTask(manageMusic);														//Plays randomly-chosen music in the last seconds of a match
	//	startTask(displayBattery);												//Display the battery voltage

	int liftTarget = potToThousandths(LIFT);					//The lift target starts wherever the lift is
//int mgLiftTarget = potToThousandths(LIFT_MG);

	//Variables for PID tuning
	int lIntegral = 0;																//Integral tuning variables
	int rIntegral = 0;
	int lPrevError = 0;																//Derivative tuning variables
	int rPrevError = 0;

	int* stackCountL = &stackCountG;									//Creates a local variable for the cones being stacked
	int currentStackHeight;

	int stackPhase = 0;																//Variable to keep track of the stacking phase it's on
	int stackDelay = 0;																//Variable to keep track of the remaining delay in the stack function

	bool incrementPressed = false;

	//Variables for local toggle switches (claw hook and rotation)
	bool clawState = false;
	bool upPressed = false;
	bool clawRotState = false;
	bool downPressed = false;

	while (1)
	{
		//Ch2					=		Right drive
		//Ch3					=		Left drive
		//5U and 5D		=		Lift
		//6U and 6D		=		Cone claw toggle
		//6D					=		Cone claw toggle rotation
		//7L					=		Mobile goal up
		//7D					=		Mobile goal down
		//8D					=		Stack cone
		//8R					=		Reset count

		//

		//Give up function
		if(vexRT[Btn5U] && vexRT[Btn5D] && vexRT[Btn8D]) {
			stackPhase = 0;
			motor[lBotLift] = motor[lTopLift] = motor[rBotLift] = motor[rTopLift] = 0;
			liftTarget = potToThousandths(LIFT);
		}

		//Drive input:
		motor[fR] = motor[bR] = axesFix(vexRT[Ch2]);			//Sets the right motors equal to channel 2
		motor[fL] = motor[bL] = axesFix(vexRT[Ch3]);			//Sets the left motors equal to channel 3

		//Lift input:

		//This button tells the motors to run at full speed downwards to help pick up cones
		if(vexRT[Btn8L] && !stackPhase) {
			motor[lBotLift] = motor[lTopLift] = motor[rBotLift] = motor[rTopLift] = -127;
			liftTarget = potToThousandths(LIFT);
		}
		//This button tells the motors to run at full speed upwards to help it get over a cone stack
		else if(vexRT[Btn8D] && !stackPhase && !(vexRT[Btn5U] && vexRT[Btn5D])) {
			motor[lBotLift] = motor[lTopLift] = motor[rBotLift] = motor[rTopLift] = 127;
			liftTarget = potToThousandths(LIFT);
		}
		//5U and 5D shoulder buttons increment and decrement the lift target
		else if(vexRT[Btn5U] || vexRT[Btn5D] && !stackPhase && !(T1 % 3))
			liftTarget += (vexRT[Btn5U] - vexRT[Btn5D]);
		//This makes sure that it doesn't PID tune while the up and down buttons are pressed
		if(!vexRT[Btn8L] && !vexRT[Btn8D]) {
			pidTuneLift(&liftTarget, &lIntegral, &rIntegral, &lPrevError, &rPrevError);					//PID tunes the lift
		}

		//Claw input:
		if(vexRT[Btn6U] && !(upPressed)) {								//6U toggles the claw
			clawState = !clawState;													//Switches the claw state
			upPressed = true;																//Changes a variable to make sure this doesn't happen twice
		}
		else if(!(vexRT[Btn6U]))													//Sets the pressed variable back once button is released
			upPressed = false;

		setClawRelease(clawState);

		//Claw rotation input:
		if(vexRT[Btn6D] && !(downPressed) && !stackPhase) { //6D toggles the claw rotation
			clawRotState = !clawRotState;											//Same toggle function as the claw release
			downPressed = true;
		}
		else if(!(vexRT[Btn6D]))
			downPressed = false;

		setRot(clawRotState);

		//Mobile goal lift input:
		if(vexRT[Btn7D])
			motor[lLiftMG] = motor[rLiftMG] = -127;
		else if(vexRT[Btn7L])
			motor[lLiftMG] = motor[rLiftMG] = 127;
		else
			motor[lLiftMG] = motor[rLiftMG] = 0;

/*	if(vexRT[Btn7L] || vexRT[Btn7D] && !stackPhase)		//Raises the lift if 7L or 7d is pressed
			mgLiftTarget -= (vexRT[Btn7D] - vexRT[Btn7L]); */

//	pTuneMGLift(&mgLiftTarget);

		/* Stack function and stack reset
		*  See void stackCone(int* targets) */
		if(vexRT[Btn8U] && !stackPhase) {
			stackPhase = 1;
		}

		else if(vexRT[Btn8R] && !stackPhase) {						//Resets the stack count if 8L is pressed
			stackCountG = 0;
			playTone(525, 50);
		}

		if(vexRT[Btn7U] && !incrementPressed) {
			stackCountG++;
			incrementPressed = true;
		}

		else if(vexRT[Btn7R] && !incrementPressed) {
			stackCountG--;
			incrementPressed = true;
		}

		else {
			incrementPressed = false;
		}

		currentStackHeight = *stackCountL * STACK_HEIGHT;

//		writeDebugStreamLine("current = %.3f  ", potToThousandths(LIFT));
//		writeDebugStream("target = %d  ", liftTarget);
//		writeDebugStream("phase = %d  ", stackPhase);
//		writeDebugStream("rms = %d  ", motor[rTopLift]);
//		writeDebugStream("lms = %d  ", motor[lTopLift]);
//		writeDebugStreamLine("stackCount = %d  ", stackCountG);

		/* This is a long function that initiates a series of steps in order to stack a cone
		*  It's written as a switch statement to make sure that in between each command, it can
		*  it can still drive around while each step is happening */
		switch(stackPhase) {
		case 0:
			break;
		/* The next step sets the lift target and then continues
		*  doing the same thing until the lift reaches the tolerance */
		case 1:
			clearTimer(T1);
			liftTarget = 800;
			if(potToThousandths(LIFT) > liftTarget - LIFT_TOL && potToThousandths(LIFT) < liftTarget + LIFT_TOL) {
				stackDelay = 0;															//Resets a delay to use in the next step
				stackPhase = 2;
			}
			if(time1[T1] > 5000)																	//It gives up if 5 seconds pass
				stackPhase = 0;
			break;
		/* The delay in the next step coninuously adds to itself
		*  every loop. This is to avoid using a wait function
		*  which would stop the drive */
		case 2:
			stackDelay++;
			clawRotState = false;
			if(stackDelay > 1000) {
				stackDelay = 0;
				stackPhase = 3;
			}
			break;
		case 3:
			clearTimer(T1);
			liftTarget = BASE_HEIGHT + currentStackHeight;
			if(potToThousandths(LIFT) > liftTarget - LIFT_TOL && potToThousandths(LIFT) < liftTarget + LIFT_TOL)
				stackPhase = 4;
			if(time1[T1] > 5000)
				stackPhase = 0;
			break;
		case 4:
			stackDelay++;
			clawState = false;
			if(stackDelay > 400) {
				stackDelay = 0;
				stackPhase = 5;
			}
			break;
		case 5:
			clearTimer(T1);
			liftTarget = 800;
			if(potToThousandths(LIFT) > liftTarget - LIFT_TOL && potToThousandths(LIFT) < liftTarget + LIFT_TOL)
				stackPhase = 6;
			if(time1[T1] > 5000)
				stackPhase = 0;
			break;
		case 6:
			stackDelay++;
			clawRotState = true;
			if(stackDelay > 400) {
				stackDelay = 0;
				liftTarget = 0;
				stackPhase = 0;
				stackCountG++;
			}
			break;
		}
	wait1Msec(20);
	}
}
