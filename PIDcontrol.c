//PID controller
const float Kp = 5;
const float Ki = 0.000001;
const float Kd = 75;

//Calculates error to be used in PID control caluclations
int errorCalc(int target, int degrees)
{
	 int error = target - degrees;
	 return error;
}

//Performs calculation for the output value to be applied to motors
float PIDcontroller(int error, int integral, int derivative)
{

	float output = (Kp * error) + ( Ki * integral) + ( derivative* Kd);

	displayString(0, "%f", error);
	displayString(1, "%f", integral);
	displayString(2, "%f", derivative);
	//wait1Msec(2000);
	return output;
}

task main()
{
	//Disabling built-in PID control
	nMotorPIDSpeedCtrl[motorA] = mtrNoReg;

	//Initializing motor speed and encoders
	nMotorEncoder[motorA] = 0;
	motor[motorA] = 75;

	//Target motion is set to 500 degrees
	int target = 360;

	/*--Initialization of variables needed for PID calculations--*/
	//Motor A
	float integralMotorA = 0;
	int derivativeMotorA = 0;
	int degMotorA = 0;
	int prevErrorMotorA = 0;
	int errorMotorA = 0;
	float PIDmotorA = 0;
	float currentTime = 0;

	time1[T1] = 0;

	//while the robot is driving for under 20 sec, PID will be tested and applied
	wait1Msec(20);
	while(1)
	{
		degMotorA = nMotorEncoder[motorA];
		prevErrorMotorA = errorMotorA;
		errorMotorA = errorCalc(target, degMotorA);

		integralMotorA += (errorMotorA) * (currentTime/time1[T1]);
		currentTime = time1[T1];
		derivativeMotorA = errorMotorA - prevErrorMotorA;
		PIDmotorA = PIDcontroller(errorMotorA, integralMotorA, derivativeMotorA);
		motor[motorA] = PIDmotorA;
		displayString(4, "%f", PIDmotorA);
		displayString(5, "%i", nMotorEncoder[motorA]);


	}
}
