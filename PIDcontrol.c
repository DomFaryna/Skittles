//PID controller
#define Kp = 0.05;
#define Ki = 0.0000000075;
#define Kd = 0.04;

//Calculates error to be used in PID control caluclations
int errorCalc(int target, int degrees)
{
	 int error = target - degrees;
	 return error;
}

//Performs calculation for the output value to be applied to motors
int PIDcontroller(int error, int integral, int derivative)
{
	int output = Kp * (error + Ki) * (integral + Kd) * derivative;
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
	int target = 500;

	/*--Initialization of variables needed for PID calculations--*/
	//Motor A
	int integralMotorA = 0;
	int derivativeMotorA = 0;
	int degMotorA = 0;
	int prevErrorMotorA = 0;
	int errorMotorA = 0;
	int PIDmotorA = 0;

	time1[T1] = 0;

	//while the robot is driving for under 20 sec, PID will be tested and applied
	while(time1[T1] < 20000)
	{
		//Motor A
		degMotorA = nMotorEncoder[motorA];
		errorMotorA = errorCalc(target, degMotorA);
		prevErrorMotorA = errorMotorA;
		integralMotorA = integralMotorA + errorMotorA;
		derivativeMotorA = errorMotorA - prevErrorMotorA;
		PIDmotorA = PIDcontroller(errorMotorA, integralMotorA, derivativeMotorA);
		motor[motorA] = PIDmotorA;
	}

}
