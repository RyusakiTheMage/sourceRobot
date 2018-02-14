#pragma config(UART_Usage, UART1, uartVEXLCD, baudRate19200, IOPins, None, None)
#pragma config(Sensor, in1,    lLiftPot,       sensorNone)
#pragma config(Sensor, in2,    mgPot,          sensorPotentiometer)
#pragma config(Sensor, in3,    clawPot,        sensorPotentiometer)
#pragma config(Sensor, in4,    rLiftPot,       sensorPotentiometer)
#pragma config(Sensor, in5,    mgIn,           sensorLineFollower)
#pragma config(Sensor, in6,    yaw,            sensorNone)
#pragma config(Sensor, dgtl1,  rightEn,        sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  leftEn,         sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  Sonar,          sensorSONAR_inch)
#pragma config(Sensor, dgtl11, LCD,            sensorNone)
#pragma config(Motor,  port1,           cR,            tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           liftMG,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           claw,          tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           lLift,         tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           fL,            tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port6,           fR,            tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           bL,            tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port8,           bR,            tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port9,           rLift,         tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port10,          cL,            tmotorVex393_HBridge, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX2)

#pragma competitionControl(Competition)

#include "Vex_Competition_Includes.c"
#include "resources/getLCDButtons.h"

//#include "resources\tunes.h"

//Left claw is in port 1
//Left mobile goal lift is in port 2
//Right mobile goal lift is in port 3
//Left lift is in port 4
//Front left drive is in port 5
//Front right drive is in port 6
//Back left drive drive is in port 7
//Back right drive is in port 8
//Right lift is in port 9
//Right claw is in port 10



/******************************************************************
/*
/*											 USER CONSTANTS
/*
*******************************************************************/



const unsigned int MG_HEIGHT = 53;			//Height the first cone should be at
const unsigned int CONE_HEIGHT = 53;		//Height for lift to raise with each cone stack
const unsigned int STACK_HEIGHT = 300;	//Height lift should be to grab a cone
const unsigned int LOAD_HEIGHT = 500;		//Height lift should be at to stack auto loads

const unsigned int DOWN_ADD_HEIGHT = 150;	//Height the lift should subtract from the target when it stops going
const unsigned int UP_ADD_HEIGHT = 150;		//Height the lift should add to the target when it stops going up

const unsigned int WHEEL_TO_CENTER = 7;	//Width from average x-location of wheels to the center of the robot
const unsigned int GIVE_UP_TIME = 3000;	//Time to give functions before it gives up on it's target

const unsigned int THRESHOLD = 20;			//Threshold for drive motors to work
const unsigned int CLAW_TOL = 30;				//Claw tolerance for p-tuning
const unsigned int LIFT_MG_TOL = 20;		//Mobile goal lift tolerance for p-tuning
const unsigned int LIFT_TOL = 20;				//Lift tolerance for p-tuning

const unsigned int CLAW_ACC_TOL = 50;		//The thousandths accuracy of the claw
const unsigned int LIFT_MG_ACC_TOL = 50;//The thousandths accuracy of the mobile goal lift
const unsigned int LIFT_ACC_TOL = 50;		//The thousandths accuracy of the lift

const unsigned int CLAW_MAX = 2325;			//The maximum for the claw potentiometer
const unsigned int CLAW_MIN = 775;			//The minimum for the claw potentiometer

const unsigned int LIFT_MG_MAX = 3220;	//The maximum for the left mobile goal lift potentiometer (lift deployed)
const unsigned int LIFT_MG_MIN = 780;		//The minimum for the left mobile goal lift potentiometer (lift undeployed)

const unsigned int L_LIFT_MAX = 2930;		//The maximum for the left lift potentiometer
const unsigned int L_LIFT_MIN = 1410;		//The minimum for the left lift potentiometer
const unsigned int R_LIFT_MAX = 2875;		//The maximum for the right lift potentiometer
const unsigned int R_LIFT_MIN = 1290;		//The minimum for the right lift potentiometer

const float P_LIFT_MG = 0.1;						//The p-value for the mobile goal lift's movement
const float P_LIFT = 0.3;								//The p-value for the lift's movement during autonomous
const float P_CLAW = 0.15;							//The p-value for the claw's rotation
const float Kp_LIFT = 0.3;							//PID values for tuning the lift during user control
const float Ki_LIFT = 0;
const float Kd_LIFT = 10;



//Enumerated type used for targets array in autonomous
enum targetTypes {
	CLAW				= 0,
	LIFT_MG			= 1,
	LIFT				= 2,
	N_TARGETS		= 3
};

//Static variable used for stack count
static int stackCount = 0;								//Variable to count the amount of cones stacked
//Static variables for toggle switches
static int autonTypeToggleState = 0;			//Variable to check which autonomous to run
static bool autonTypePressed = false;			//Variable to check if the skills button is pressed
static bool fieldSideToggleState = false;	//Variable to check which field side the robot is on
static bool fieldSidePressed = false;			//Variable to check if the field side button is pressed
static bool autonToggleState = true;			//Variable to check if auton should run
static bool autonPressed = false;					//Variable to check if the auton button is pressed



/******************************************************************
/*
/*											 FUNCTIONS
/*
*******************************************************************/



/* A generic conversion function for converting from a thousandths to a potentiometer value
*  for either the mobile goal lift or left/right lift */
float thousandthsToPot(targetTypes type, int thousandths, tDirections side = right) {
	//Stores desired extrema as floats to avoid later unit conversions
	float max = (type == LIFT_MG) ? LIFT_MG_MAX :
	((type == CLAW) ? CLAW_MAX :
	((side == left) ? L_LIFT_MAX : R_LIFT_MAX));
	float min = (type == LIFT_MG) ? LIFT_MG_MIN :
	((type == CLAW) ? CLAW_MIN :
	((side == left) ? L_LIFT_MIN : R_LIFT_MIN));
	//Performs the calcualations and returns the desired value
	return (min + (max - min) * ((float)thousandths / 1000));
}

/* A generic conversion function for converting from a side of a system that uses a
*  potentiometer to a thousandths of a potentiometer value */
float potToThousandths(targetTypes type, tDirections side = left) {
	//Stores desired extrema as floats to avoid later unit conversions
	float max = (type == LIFT_MG) ? LIFT_MG_MAX :
	((type == CLAW) ? CLAW_MAX :
	((side == left) ? L_LIFT_MAX : R_LIFT_MAX));
	float min = (type == LIFT_MG) ? LIFT_MG_MIN :
	((type == CLAW) ? CLAW_MIN :
	((side == left) ? L_LIFT_MIN : R_LIFT_MIN));
	//Stores the current potentiometer value
	float potVal = (type == LIFT_MG) ? SensorValue[mgPot] :
	((type == CLAW) ? SensorValue[clawPot] :
	((side == left) ? SensorValue[lLiftPot] : SensorValue[rLiftPot]));
	//Performs the calculations and returns the desired value
	return 1000 * ((potVal - min) / (max - min));
}

/* Basic conversion for converting inches driven to encoder ticks
*  that can be used for p-tuning */
float inchesToEncoderTicks(float inches) {
	float encoderTicks = 90 * PI * inches;
	return encoderTicks;
}

/* Sets the claw equal to a motor speed depending on the
*  position. It also makes sure that the potentiometer
*  isn't at it's max or min */
void pTuneClaw(int clawTarget) {
	clawTarget = (clawTarget > 1000) ? 1000 : clawTarget;
	clawTarget = (clawTarget < 0) ? 0 : clawTarget;

	int clawError = P_CLAW * (thousandthsToPot(CLAW, clawTarget) - SensorValue[clawPot]);

	clawError = (clawError > 127) ? 127 : clawError;
	clawError = (clawError < -60) ? 60 : clawError;

	motor[claw] = (abs(clawError) >= CLAW_TOL) ? clawError : 0;
}

/* SEE void pTuneClaw(int clawTarget)
*  The equivalent of the above function, but the target is passed by
*  reference rather than by value. This version of the function is
*  used during driver mode in conjunction with a local variable */
void pTuneClaw(int* clawTarget) {
	if(*clawTarget > 1000)
		*clawTarget = 1000;
	else if(*clawTarget < 0)
		*clawTarget = 0;

	int clawError = P_CLAW * (thousandthsToPot(CLAW, *clawTarget) - SensorValue[clawPot]);

	clawError = (clawError > 127) ? 127 : clawError;
	clawError = (clawError < -60) ? 60 : clawError;

	motor[claw] = (abs(clawError) >= CLAW_TOL) ? clawError : 0;
}

/* P-tunes the mobile goal lift to a specified mobile goal lift
*  target, which is a thousandths value along the range of the
*  claw. This version of the function is used during autonomous
*  in conjunction with an array */
void pTuneMGLift(int liftMGTarget) {
	//The mobile goal target is corrected if it is above the maximum
	liftMGTarget = (liftMGTarget > 1000) ? 1000 : liftMGTarget;
	//The mobile goal target is corrected if it is below the minimum
	liftMGTarget = (liftMGTarget < 0) ? 0 : liftMGTarget;

	//Finds the error mutplied by the p-value of the mobile goal lift
	int liftMGError = P_LIFT_MG * (thousandthsToPot(LIFT_MG, liftMGTarget) - SensorValue[mgPot]);

	liftMGError = (liftMGError > 127) ? 127 : liftMGError;
	liftMGError = (liftMGError < -127) ? -127 : liftMGError;

	//The mobile goal lift only p-tunes if the error is above the tolerance
	motor[liftMG] = (abs(liftMGError) >= LIFT_MG_TOL) ? liftMGError : 0;
}

/* SEE void pTuneMGLift(int liftMGTarget)
*  The equivalent of the above function, but the target is passed by
*  reference rather than by value. This version of the function is
*  used during driver mode in conjunction with a local variable */
void pTuneMGLift(int* liftMGTarget) {
	*liftMGTarget = (*liftMGTarget > 1000) ? 1000 : *liftMGTarget;
	*liftMGTarget = (*liftMGTarget < 0) ? 0 : *liftMGTarget;

	int liftMGError = P_LIFT_MG * (thousandthsToPot(LIFT_MG, *liftMGTarget) - SensorValue[mgPot]);

	liftMGError = (liftMGError > 127) ? 127 : liftMGError;
	liftMGError = (liftMGError < -127) ? -127 : liftMGError;

	motor[liftMG] = (abs(liftMGError) >= LIFT_MG_TOL) ? liftMGError : 0;
}

/* P-tunes the lift to a specified lift target, which is a
*  thousandths value along the range of the lift.
*  This version of the function is used during autonomous
*  in conjunction with an array */
void pTuneLift(int liftTarget) {
	liftTarget = (liftTarget > 1000) ? 1000 : liftTarget;
	liftTarget = (liftTarget < 0) ? 0 : liftTarget;

	int lError = thousandthsToPot(LIFT, liftTarget, left) - SensorValue[lLiftPot];
	int rError = thousandthsToPot(LIFT, liftTarget, right) - SensorValue[rLiftPot];

	int lDrive = P_LIFT * lError;
	int rDrive = P_LIFT * rError;

	lDrive = (lDrive > 127) ? 127 : lDrive;
	lDrive = (lDrive < -127) ? -127 : lDrive;
	rDrive = (rDrive > 127) ? 127 : rDrive;
	rDrive = (rDrive < -127) ? -127 : rDrive;

	motor[lLift] = (abs(lError) >= LIFT_TOL) ? lDrive : 0;
	motor[rLift] = (abs(rError) >= LIFT_TOL) ? rDrive : 0;
}

/* PID version of the previous function that is used for user control
*  and is passed by reference than by value.
*  This function is used during driver mode in conjunction with a
*  local variable */
void pidTuneLift(int* liftTarget) {
	*liftTarget = (*liftTarget > 1000) ? 1000 : *liftTarget;
	*liftTarget = (*liftTarget < 0) ? 0 : *liftTarget;

	//Sets up static integers to keep track of the integral and previous error
	static int lIntegral = 0;
	static int rIntegral = 0;
	static int lPrevError = 0;
	static int rPrevError = 0;

	//Finds the error between the target and the actual value
	int lError = thousandthsToPot(LIFT, *liftTarget, left) - SensorValue[lLiftPot];
	int rError = thousandthsToPot(LIFT, *liftTarget, right) - SensorValue[rLiftPot];
	//Continuously adds the error to the integral count
	lIntegral += lError;
	rIntegral += rError;
	//Keeps track of the difference between the error and the previous error
	int lDerivative = lError - lPrevError;
	int rDerivative = rError - rPrevError;
	//Resets the previous error
	if(!(time1(T2) % 1)) {
		lPrevError = lError;
		rPrevError = rError;
	}

	//Sets the drive values equal to their respective variable multiplied by the tuning value
	int lDrive = (Kp_LIFT * lError) + (Ki_LIFT * lIntegral) + (Kd_LIFT * lDerivative);
	int rDrive = (Kp_LIFT * rError) + (Ki_LIFT * rIntegral) + (Kd_LIFT * rDerivative);

	lDrive = (lDrive > 127) ? 127 : lDrive;
	lDrive = (lDrive < -127) ? -127 : lDrive;
	rDrive = (rDrive > 127) ? 127 : rDrive;
	rDrive = (rDrive < -127) ? -127 : rDrive;

	//Resets the integral if the drive isn't PID tuning
	if(abs(lError) < LIFT_TOL) {
		lIntegral = 0;
	}
	if(abs(rError) < LIFT_TOL) {
		rIntegral = 0;
	}
	//Doesn't move the motors if they are too small or it's within the tolerance to reduce motor whine
	motor[lLift] = lDrive;
	motor[rLift] = rDrive;
}

/* The robot drives a set distance in encoder ticks, given by
*  enTarget. It uses a formula to drive at a speed scaled to
*  the difference between the current displacement and the
*  final displacement.  It then drives backwards to counteract
*  any additional drift.  It p-tunes while doing so */
void waitDistance(const int* const targets, int enTarget) {
	SensorValue[rightEn] = 0;		//Encoders zeroed
	SensorValue[leftEn] = 0;
	clearTimer(T1);							//Timer is cleared to make it give up if it takes too long

	int drive = 0;							//Variable for storing the speed the drive should travel at

	int a = 18;									//Variable that reduces drift when increased but also lowers speed

	/* The robot uses a formula to scale speed to position if
	*  the velocity would be within the threshold to start
	*  driving, but otherwise sets the speed equal to the
	*  minimum threshold otherwise */
	while(abs(SensorValue[leftEn]) < abs(enTarget) && time1[T1] < GIVE_UP_TIME) {
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
	motor[bR] = motor[cR] = motor[fR] = motor[bL] = motor[cL] = motor[fL] = -sgn(enTarget) * THRESHOLD;
	wait1Msec(110);
	motor[bR] = motor[cR] = motor[fR] = motor[bL] = motor[cL] = motor[fL] = 0;
}

/* A function used in autonomous that tells the robot to rotate a
*  certain amount in tenths of a degree, using the VEX gyro */
void waitRotate(const int* const targets, int degrees10) {
	//Multiplies the limit switch for the fieldside by the degrees, so that it is on the left side if the toggle variable is false
	degrees10 *= ((fieldSideToggleState) ? 1 : -1);
	SensorValue[yaw] = 0;				//Encoders zeroed
	clearTimer(T1);							//Timer is cleared to make it give up if it takes too long

	int drive = 0;							//Variable that contains the drive speed

	int a = 15;									//The intensity of the drive functoin

	/* Enacts the rotation to a set tenths of a degree (degrees10)
	*  while tuning both lifts. The rotation speed is tuned to
	*  decrease on a slope from 127 to the treshold. If it is past
	*  the threshold, it is set equal to the treshold, but with each
	*  side rotating in a different direction */
	if(degrees10 > 0) {
		while(SensorValue[yaw] < degrees10 && time1(T1) < GIVE_UP_TIME) {
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
	motor[bR] = motor[cR] = motor[fR] = -sgn(degrees10) * THRESHOLD;
	motor[bL] = motor[cL] = motor[fL] = sgn(degrees10) * THRESHOLD;
	wait1Msec(110);
	motor[bR]  = motor[cR] = motor[fR] = motor[bL] = motor[cL] = motor[fL] = 0;
}

/* A function used in autonomous that tells the robot to move in a
*  curved path to a new coordinate that's outTargetX feet away
*  horizontally and outTargetY feet away vertically and the plane
*  is oriented depending on the rotation of the robot. It also has
*  the option to move backwards in a curved motion. The first part
*  of this function calculates the values that it will use and the
*  second part implements them */
void waitDistanceCurve(const int* const targets, int outTargetX, int outTargetY, bool backwards = false) {
	//Sets up a variable in so that it can easily flip the direction if commanded to go backwards
	int direction = (backwards) ? -1 : 1;

	//Sets a new location depending on if the robot is supposed to go left or right
	int location = sgn(outTargetX) * WHEEL_TO_CENTER;

	//Sets up targets for the wheels on the inner side of the curve and gives them default values
	int inTargetX = 0;
	int inTargetY = outTargetY;

	/* Sets up a variable for the portion of the circle being
	*  calculated between the two points. To calculate it, the
	*  distance formula, arcsine, and the fact that all angles in
	*  a triangle add up to 180 are used. This also makes sure
	*  that the target coordinates aren't equal to values that
	*  would result in the function dividing by zero. If it's
	*  ordered to go backwards, the input value of outTargetY is
	*  flipped so that it still calculates it as if it were going
	*  forward */
	float theta = (outTargetX == - 2 * location && !outTargetY) ? 0 :
	180 - 2 * radiansToDegrees(asin((direction * outTargetY) / sqrt((pow(outTargetX + 2 * location, 2)) + (pow(outTargetY, 2)))));

	/* Calculates and sets the inner wheel targets using
	*  trigonometry. The value also depends on the direction
	*  that the robot is traveling */
	if(outTargetX > 0) {
		inTargetX = outTargetX + 2 * location * cosDegrees(theta);
		inTargetY = outTargetY - 2 * location * sinDegrees(theta);
	}
	else if(outTargetX < 0) {
		inTargetX = outTargetX + 2 * location * cosDegrees(theta);
		inTargetY = outTargetY + 2 * location * sinDegrees(theta);
	}

	/* Calculates the radios of the circle each path finds
	*  using the law of sines and makes sure that that it
	*  doesn't divide by zero */
	int outRadius = (theta) ? abs(outTargetX / (sin(theta))) : 0;
	int inRadius = (theta) ? abs(inTargetX / (sin(theta))) : 0;

	/* Calculates the distance it has to travels using the
	*  equation for the circumference of the circle dividing
	*  it by the potion of the circle being traveled in order
	*  to give each encoder a target value. It also converts
	*  the answer into encoder ticks so that it won't round the
	*  variable very much considering that it's an integer */
	int outDistance = 2 * PI * outRadius * (theta / 360);
	int inDistance = 2 * PI * inRadius * (theta / 360);

	/* Calculates the ratio between the target distances for the
	*  encoders so that it can multiply the inner motors by this
	*  speed. This will make sure that each side takes the same
	*  amount of time to cover their target distances. It also
	*  makes sure that it doesn't divide by zero */
	int speedRatio = (outDistance) ? inDistance / outDistance : 0;

	SensorValue[rightEn] = 0;	//Encoders zeroed
	SensorValue[leftEn] = 0;
	clearTimer(T1);						//Timer is cleared to make it give up if it takes too long

	int lDrive = 0;						//Variables for storing the speed the drives should travel at
	int rDrive = 0;

	int a = 18;								//Variable that reduces drift when increased but also lowers speed

	/* If the target involves turning right, the left encoder's
	*  target is the outer distnace and the inner encoder's target
	*  is the inner distance. The values are multiplied by the
	*  direction so that they flip if the robot is ordered to move
	*  backwards. It uses the same logarithmic tuner as the
	*  previous two functions and sets the motors equal to the
	*  threshold if the fall below the threshold. It then multiplies
	*  the right motor speed by the speed ratio since that's on the
	*  inner side so it'll have to move slower. It also p-tunes both
	*  lifts while moving. */
	if(outTargetX > 0) {
		while(SensorValue[leftEn] < outDistance && SensorValue[rightEn] < inDistance && time1[T1] < GIVE_UP_TIME) {
			lDrive = direction * a * log(-abs(SensorValue[leftEn]) + outDistance) + (127 - (a * log(outDistance)));
			rDrive = direction * a * log(-abs(SensorValue[rightEn]) + inDistance) + (127 - (a * log(inDistance)));
			if(lDrive < THRESHOLD)
				lDrive = direction * THRESHOLD;
			else if(rDrive < THRESHOLD)
				rDrive = direction * THRESHOLD;
			motor[bR] = motor[cR] = motor[fR] = speedRatio * rDrive;
			motor[bL] = motor[cL] = motor[fL] = lDrive;
			pTuneMGLift(targets[LIFT_MG]);
			pTuneLift(targets[LIFT]);
		}
	}
	/* If the target involves turning left, the encoder sides are
	*  set to the opposite configuration. This has the same commands
	*  but it accounds for the inner distance being on the left. */
	else if(outTargetX < 0) {
		while(SensorValue[leftEn] < inDistance && SensorValue[rightEn] < outDistance && time1[T1] < 5000) {
			lDrive = direction * a * log(-abs(SensorValue[leftEn]) + inDistance) + (127 - (a * log(inDistance)));
			rDrive = direction * a * log(-abs(SensorValue[rightEn]) + outDistance) + (127 - (a * log(outDistance)));
			if(lDrive < THRESHOLD)
				lDrive = direction * THRESHOLD;
			else if(rDrive < THRESHOLD)
				rDrive = direction * THRESHOLD;
			motor[bR] = motor[cR] = motor[fR] = rDrive;
			motor[bL] = motor[cL] = motor[fL] = speedRatio * lDrive;
			pTuneMGLift(targets[LIFT_MG]);
			pTuneLift(targets[LIFT]);
		}
	}

	//Motors drive backward briefly in order to counteract additional drift
	motor[bR] = motor[cR] = motor[fR] = motor[bL] = motor[cL] = motor[fL] = -direction * THRESHOLD;
	wait1Msec(110);
	motor[bR] = motor[cR] = motor[fR] = motor[bL] = motor[cL] = motor[fL] = 0;
}

/* A somewhat general function used in autonomous that changes the
*  contents of the current target values for the lifts and claw,
*  and then pTunes both the lifts and claw with their new target
*  values */
void waitActions(int* targets, int liftMGTarget, int clawTarget, int liftTarget) {
	//Assigns the contents of targets to the new lift and claw target values
	targets[CLAW] = clawTarget;
	targets[LIFT_MG] = liftMGTarget;
	targets[LIFT] = liftTarget;
	//P-tunes the lifts and the claw
	pTuneClaw(targets[CLAW]);
	pTuneMGLift(targets[LIFT_MG]);
	pTuneLift(targets[LIFT]);
}

/* Sets targets equal to the new values, then continues to test to see
*  if the claw is within the range specified by tolerance, which is
*  defined by the user or set equal to the default tolerance */
void testClaw(int* targets, int liftMGTarget, int clawTarget, int liftTarget, int accuracy = CLAW_ACC_TOL) {
	do {
		waitActions(targets, liftMGTarget, clawTarget, liftTarget);
	} while(potToThousandths(CLAW) < clawTarget - accuracy || potToThousandths(CLAW) > clawTarget + accuracy);
}

/* Specific function for p-tuning just the claw */
void setClaw(int* targets, int clawTarget, int accuracy = CLAW_ACC_TOL) {
	testClaw(targets, targets[LIFT_MG], clawTarget, targets[LIFT], accuracy);
}

/* Sets targets equal to the new values, then continues to test to see
*  if the mobile goal lift is within the range specified by tolerance,
*  which is defined by the user or set equal to the default
*  tolerance */
void testMGLift(int* targets, int liftMGTarget, int clawTarget, int liftTarget, int accuracy = LIFT_MG_ACC_TOL) {
	do {
		waitActions(targets, liftMGTarget, clawTarget, liftTarget);
	} while(potToThousandths(LIFT_MG) < liftMGTarget - accuracy || potToThousandths(LIFT_MG) > liftMGTarget + accuracy);
}

/* Specific function for p-tuning just the mobile goal lift */
void setMGLift(int* targets, int liftMGTarget, int accuracy = LIFT_MG_ACC_TOL) {
	testMGLift(targets, liftMGTarget, targets[CLAW], targets[LIFT], accuracy);
}

/* Sets targets equal to the new values, then continues to test to see
*  if the lift is within the range specified by tolerance, which is
*  defined by the user or set equal to the default tolerance */
void testLift(int* targets, int liftMGTarget, int clawTarget, int liftTarget, int accuracy = LIFT_ACC_TOL) {
	do {
		waitActions(targets, liftMGTarget, clawTarget, liftTarget);
	} while(potToThousandths(LIFT) < liftTarget - accuracy || potToThousandths(LIFT) > liftTarget + accuracy);
}

/* Specific function for p-tuning just the lift */
void setLift(int* targets, int liftTarget, int accuracy = LIFT_ACC_TOL) {
	testLift(targets, targets[LIFT_MG], targets[CLAW], liftTarget, accuracy);
}

/* Eliminates motor whine on the drive during user control */
int axesFix(int remote) {
	if(abs(remote) >= THRESHOLD) 		//Returns 0 if the value does not pass the TRESHOLD constant
		return remote;
	else
		return 0;
}

void LCDAutonSelection()
{
	//Clears LCD and turn on backlight
	clearLCDLine(0);
	clearLCDLine(1);
	bLCDBacklight = true;

	//Displays default choices
	displayLCDString(0, 0, "Auton Settings:");
	displayLCDString(1, 0, "L     10pt    ON");

	while(bIfiRobotDisabled) {
		//Checks for the left button being pressed if the button isn't already pressed
		if((nLCDButtons & leftBtnVEX) && !(fieldSidePressed)) {
			fieldSideToggleState = !(fieldSideToggleState);	//Change the toggle state
			fieldSidePressed = true;												//Note that the button is pressed
		}
		//If the left button isn't pressed
		else if (!(nLCDButtons & leftBtnVEX)) {
			fieldSidePressed = false;												//Note that the button is not pressed
		}

		//Same toggle switch with the skills button
		if((nLCDButtons & centerBtnVEX) && !(autonTypePressed)) {
			autonTypeToggleState = (autonTypeToggleState + 1) % 3;
			autonTypePressed = true;
		}
		else if (!(nLCDButtons & centerBtnVEX)) {
			autonTypePressed = false;
		}

		//Same toggle switch for whether or not the auton will activate
		if((nLCDButtons & rightBtnVEX) && !(autonPressed)) {
			autonToggleState = !(autonToggleState);
			autonPressed = true;
		}
		else if (!(nLCDButtons & rightBtnVEX)) {
			autonPressed = false;
		}

		//Print various messages depending on the button state
		if(fieldSideToggleState)
			displayLCDString(1, 0, "R");
		else if(!(fieldSideToggleState))
			displayLCDString(1, 0, "L");
		if(!(autonTypeToggleState))
			displayLCDString(1, 6, "10pt");
		else if(autonTypeToggleState == 1)
			displayLCDString(1, 6, "20pt");
		else if(autonTypeToggleState == 2)
			displayLCDString(1, 6, "Sk  ");
		if(autonToggleState)
			displayLCDString(1, 13, "ON ");
		else if(!(autonToggleState))
			displayLCDString(1, 13, "OFF");
	}
}



/******************************************************************
/*
/*											 TASKS
/*
*******************************************************************/



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

//Task for calibrating the gyro
task calibrateGyro()
{
	SensorType[yaw] = sensorNone;	//Resets the gyroscope
	wait1Msec(1100);							//Provides time for calibration
	SensorType[yaw] = sensorGyro;	//Reassigns the yaw gyroscope's sensor type
	wait1Msec(1100);							//Waits for the gyro to calibrate

	SensorValue[rightEn] = 0;			//Zeroes the right encoder
	SensorValue[leftEn] = 0;			//Zeroes the left encoder
	playTone(1200, 50);						//Plays a tone when calibration is finished to alert the operator
}

task drive()
{
	while(1) {
		motor[fR] = motor[cR] = motor[bR] = axesFix(vexRT[Ch2]);			//Sets the right motors equal to channel 2
		motor[fL] = motor[cL] = motor[bL] = axesFix(vexRT[Ch3]);			//Sets the left motors equal to channel 3
	}
}



/******************************************************************
/*
/*											 AUTONOMOUS CODE
/*
*******************************************************************/



void pre_auton()
{
	bStopTasksBetweenModes = true;

	//Calibrates the gyro and turns on the LCD
	startTask(calibrateGyro);
	LCDAutonSelection();
}

task autonomous()
{
	/* targets is an array of integers that stores the target
	*  values for the mglift, claw, rotation, and lift */
	int targets[N_TARGETS] = {potToThousandths(CLAW), potToThousandths(LIFT_MG), potToThousandths(LIFT)};

	//Only starts the autonomous if the togglestate is true
	if(autonToggleState) {
		// Skills Autonomous
		if(autonTypeToggleState == 2) {
			setLift(targets, 500, 150);
			setMGLift(targets, 1000);
			waitDistance(targets, 1300);
			setMGLift(targets, 0);		// pick up first goal
			setLift(targets, 25);
			waitDistance(targets, -1100);
			waitRotate(targets, -450);
			waitDistance(targets, -550);
			waitRotate(targets, -850);
			waitDistance(targets, 250);
			setClaw(targets, 1000);
			setLift(targets, 400, 100);
			setClaw(targets, 0);
			setMGLift(targets, 1000);		// pick up second goal
			waitDistance(targets, -200);
			wait1Msec(500);
			waitRotate(targets, 1860);
			waitDistance(targets, 425);
			setMGLift(targets, 0);
			setLift(targets, 50);
			waitDistance(targets, -500);
			waitRotate(targets, 875);
		}

		//Regular Autonomous
		else {
			//Scores a mobile goal in the 20 point zone
			if(autonTypeToggleState == 1) {
				setLift(targets, 500, 150);
				setMGLift(targets, 1000);
				waitDistance(targets, 1300);
				setMGLift(targets, 0);
				setLift(targets, MG_HEIGHT);
				setClaw(targets, 1000);
				setLift(targets, 550, 100);
				setClaw(targets, 0);
				waitDistance(targets, -1100);
				waitRotate(targets, -450);
				waitDistance(targets, -800);
				waitRotate(targets, -500);
				motor[fL] = motor[cL] = motor[bL] = motor[fR] = motor[cR] = motor[bR] = 127;
				wait1Msec(1300);
				motor[fL] = motor[cL] = motor[bL] = 0;
				wait1Msec(300);
				motor[fR] = motor[cR] = motor[bR] = 0;
				setMGLift(targets, 1000);
				setLift(targets, 0);
				motor[fL] = motor[cL] = motor[bL] = motor[fR] = motor[cR] = motor[bR] = -127;
				wait1Msec(1000);
				motor[fL] = motor[cL] = motor[bL] = motor[fR] = motor[cR] = motor[bR] = 0;
			}
			//Scores a mobile goal in the 10 point zone
			else if(!(autonTypeToggleState)) {
				setLift(targets, 500);
				setMGLift(targets, 1000);
				waitDistance(targets, 1300);
				setMGLift(targets, 0);
				setLift(targets, MG_HEIGHT);
				motor[claw] = 127;
				wait1Msec(500);
				motor[claw] = 0;
				setLift(targets, 400);
				motor[claw] = -127;
				wait1Msec(500);
				motor[claw] = 0;
				waitDistance(targets, -1100);
				waitRotate(targets, -450);
				waitDistance(targets, -550);
				waitRotate(targets, -850);
				waitDistance(targets, 100);
				setMGLift(targets, 1000);
				waitDistance(targets, -200);
			}
		}
	}
}



/******************************************************************
/*
/*											 USER CONTROL
/*
*******************************************************************/



task usercontrol()
{
//startTask(manageMusic);				//Plays randomly-chosen music in the last seconds of a match
	startTask(displayBattery);		//Displays the battery voltage
	startTask(drive);

	clearTimer(T2);								//Clears the timer for the PID

	//Sets the targets equal to the current positions
	int liftTarget = potToThousandths(LIFT);
	int mgLiftTarget = potToThousandths(LIFT_MG);
	int clawTarget = potToThousandths(CLAW);
	int clawPosition = 1;

	//Creates variables for keeping track of stack height
	int currentStackHeight = stackCount * CONE_HEIGHT + MG_HEIGHT;
	int tempStackHeight = currentStackHeight;

	bool incrementPressed = false;		//Variable for incrementing the stack count
	bool addCone = false;							//Variable for adding to the stack count
	bool stackCancel = false;					//Variable for cancelling the autostack
	bool autoStack = true;						//Variable for turning the autostack on and off

	while (1)
	{
		//Ch2					=		Right drive
		//Ch3					=		Left drive
		//5U and 5D		=		Lift
		//6U and 6D		=		Claw
		//7L					=		Mobile goal up
		//7D					=		Mobile goal down
		//8L					=		Auto load height
		//8U					=		Enable/disable autostack
		//8D					=		Stack height
		//8R					=		Reset count

		//Give up function
		if(vexRT[Btn5U] && vexRT[Btn5D] && vexRT[Btn8D]) {
			allMotorsOff();
			liftTarget = potToThousandths(LIFT);
			mgLiftTarget = potToThousandths(LIFT_MG);
		}

		//Update the current stack height
		currentStackHeight = stackCount * CONE_HEIGHT + MG_HEIGHT;

		//Claw input:
		if(vexRT[Btn6U]) {
			//Sets the target position inward
			clawPosition = -1;
			//Cancels the autoStack if that's in progress
			stackCancel = true;
			addCone = false;
		}
		else if(vexRT[Btn6D]) {
			//Sets the target position outward
			clawPosition = 1;
			//Cancels the autostack
			stackCancel = true;
			addCone = false;
		}
		//Button 6U and 6D take priority over autostack
		else {
			/* Sets up a temporary height for lift to stack to so
			*  it gives the driver more of a tolarance whithout
			*  messing up the autostack. */
			tempStackHeight = (currentStackHeight + 150 > 400) ? currentStackHeight + 150 : 400;
			//Resets the cancel if the potentiometer goes below a certain height
			stackCancel = (potToThousandths(LIFT) < tempStackHeight) ? false : stackCancel;

			//If the autostack isn't canceled and is enabled
			if(!stackCancel && autoStack) {
				//Rotates inward if the lift is above the stack count and registers there is a cone
				if(potToThousandths(LIFT) > tempStackHeight) {
					clawPosition = -1;
					addCone = true;
				}
				//Rotates outward if the claw is in and the lift is right above the last cone
				else if(potToThousandths(LIFT) < currentStackHeight + 20 && potToThousandths(CLAW) <= 200) {
					clawPosition = 1;
					//The addcone makes sure that it only increments the stack count once
					if(addCone) {
						stackCount++;
						addCone = false;
					}
				}
			}
		}

		if(clawPosition == -1) {
			//Sets the motors equal to -127 if the potentiometer is greater than the minium
			motor[claw] = (potToThousandths(CLAW) > CLAW_TOL) ? -60 : 0;
		}
		else if(clawPosition == 1) {
			//Sets the motors equal to 127 if the potentiometer is less than the maximum
			motor[claw] = (potToThousandths(CLAW) < 1000 - CLAW_TOL - 50) ? 127 : 0;
		}

		//Lift input:
		if(vexRT[Btn5U]) {
			//Sets the motors equal to 127 if the lift is less than the maximum and updates the lift target as it goes
			motor[rLift] = motor[lLift] = (potToThousandths(LIFT) <= 1000 - LIFT_TOL) ? 127 : 0;
			liftTarget = potToThousandths(LIFT) + UP_ADD_HEIGHT;
		}
		else if(vexRT[Btn5D]) {
			//Sets the motors equal to -127 if the lift is greater than the minimum and updates the lift target as it goes
			motor[rLift] = motor[lLift] = (potToThousandths(LIFT) >= 0) ? -127 : 0;
			liftTarget = potToThousandths(LIFT) - DOWN_ADD_HEIGHT;
		}
		else {
			//Bounds the lift target between 0 and 1000
			liftTarget = (liftTarget < 0) ? 0 : liftTarget;
			liftTarget = (liftTarget > 1000) ? 1000 : liftTarget;

			//PID tunes the lift as long as the up/down buttons aren't pressed
			pidTuneLift(&liftTarget);
		}

		//Button for disabling the autostack
		if(vexRT[Btn8U])
			autoStack = false;

		//Buttons for setting the lift to preset heights
		else if(vexRT[Btn8D]) {
			autoStack = true;
			//Sets the lift to a height for stacking ground cones
			liftTarget = STACK_HEIGHT;
		}
		else if(vexRT[Btn8L]) {
			autoStack = true;
			//Sets the lift to a height for stacking auto load cones
			liftTarget = LOAD_HEIGHT;
		}

		//Mobile goal lift input:
		if(vexRT[Btn7L]) {
			//Sets the mobile goal motors equal to 127 if the mobile goal lift is less than the max
			motor[liftMG] = (potToThousandths(LIFT_MG) <= 1000 - LIFT_MG_TOL) ? 127 : 0;
		}
		else if(vexRT[Btn7D]) {
			//Sets the mobile goal motors equal to -127 if the mobile goal lift is greater than the min
			motor[liftMG] = (potToThousandths(LIFT_MG) >= 0 + LIFT_MG_TOL) ? -127 : 0;
		}
		else {
			//Disables the motors if nothing is pressed
			motor[liftMG] = 0;
		}

		/* These buttons increment the internal stackcount and
		*  have the speaker play tunes to indicate what the
		*  stack count is. */
		if(vexRT[Btn7U] && !incrementPressed) {
			stackCount++;
			incrementPressed = true;
			//Plays a tone as many times as the stack count
			for(int i = 0; i < stackCount; i++) {
				playTone(1000, 5); wait1Msec(5);
				playTone(0, 5); wait1Msec(5);
			}
		}
		else if(vexRT[Btn7R] && !incrementPressed) {
			if(stackCount > 0)
				stackCount--;
			incrementPressed = true;
			for(int i = 0; i < stackCount; i++) {
				playTone(800, 5); wait1Msec(5);
				playTone(0, 5); wait1Msec(5);
			}
		}

		//This button resets the stack count
		else if(vexRT[Btn8R] && !incrementPressed) {
			stackCount = 0;
			incrementPressed = true;
			playTone(525, 75); wait1Msec(75);
		}

		//Makes sure it doesn't increment more than once
		else if (!(vexRT[Btn7U] || vexRT[Btn7R] || vexRT[Btn8R])) {
			incrementPressed = false;
		}
	}
}