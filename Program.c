#include "NXT_FileIO.c"

enum SkittleColour{
	RED, ORANGE, YELLOW, GREEN, PURPLE, UNDEFINED
};

struct Skittle{	// a struct used to store skittle information
	int normal; // The colour the nxt thinks the skittle is. E.g. 5 = red, 4 = yellow, 1 = black, etc
	int R; // the R value of the skittle
	int G; // the G value of the skittle
	int B; // the B value of the skittle
	int sum; // The sum of the RGB  values
}

struct A2D{		// a sturct used to create and pass 2D arrays throughout the rest of the program
	int array[5][13];
}

struct A1D{		// a struct used to create and pass 1D arrays throughout the rest of the program
	int array[4];
}

const float motorSpeed = 2.5; // constant speed for motors
void homeMotorB();
void homeMotorC();
void moveMotorB(int toDist);
bool scanForColour(Skittle read);
void spitSkittle(SkittleColour colour);
//void spitSkittle(Skittle read, A2D config);
void moveAndScan(Skittle colour, bool sort, A2D config);
void moveMotorPID(int toDist, int motorNum);

task doit(){	// seperate thread used to home the slide motor to save time.
	wait1Msec(200); // need the delay to start thread. No idea why, but it works.
	homeMotorB();
}


task moveMotorA(){
		motor[motorA] = 20;
		wait1Msec(500);
		motor[motorA] = 0;
		//motor[motorA] = -20;
		//wait1Msec(1500);
}
void reset2DArray(A2D reset){  // reset the 2D array to be populted with zeros.
	for(int i = 0; i < 5; i++)
	{
		for(int j = 0; j < 13; j++)
		{
			reset.array[i][j] = 0;
		}
	}
}

void sortSkittle(A2D config, Skittle scan)
{
	enum SkittleColour check;
	check = UNDEFINED;
	int match = 0; // Using an int to find a match
	bool correct[5] = {false, false, false, false, false};
	for(int i = 0; i < 5; i++)
	{
		for(int j = 0; j < config.array[i][8]; j++){
			if(config.array[i][j+9] == scan.normal)
			{
				match++;
				correct[i] = true;
			}

		}
	}

	eraseDisplay();
	displayString(0, "%i", correct[0]);
	displayString(1, "%i", correct[1]);
	displayString(2, "%i", correct[2]);
	displayString(3, "%i", correct[3]);
	displayString(4, "%i", correct[4]);
	displayString(5, "%i", match);
	wait10Msec(100);

	if(match == 1)
	{
		if(correct[0]) check = RED;
		else if(correct[1]) check = ORANGE;
		else if(correct[2]) check = YELLOW;
		else if(correct[3]) check = GREEN;
		else if(correct[4]) check = PURPLE;
	}

	else if(match > 1)
	{
		int matches[5] = {0, 0, 0, 0, 0};
		for(int i = 0; i < 5; i++){
			if(scan.normal == 5){
				check = (scan.R > ((config.array[0][4] + config.array[0][4] + config.array[0][4] + config.array[0][4] + config.array[4][0]+ config.array[4][0])/6)?RED:PURPLE;
				i = 5;
			}
			else if(scan.normal == 4){
				check = (scan.R > (config.array[2][4] + config.array[3][0])/2)?YELLOW:GREEN;
				i = 5;
			}

			else if(scan.normal == 2)
			{
				if(scan.B > (config.array[1][5] + config.array[1][5] + config.array[4][1])/3){
					if(scan.G > (config.array[1][6] + config.array[1][6] + config.array[4][2])/3){
						check = ORANGE;
					}
					else
					{
						check = PURPLE;
					}
				}
				else{
					check = PURPLE;
				}
			}
			else if(correct[i])
				{ /*
					if(scan.R <= config.array[i][0] && scan.R >= config.array[i][4]){ matches[i]++;}
					if(scan.G <= config.array[i][1] && scan.G >= config.array[i][5]){ matches[i]++;}
					if(scan.B <= config.array[i][2] && scan.B >= config.array[i][6]){ matches[i]++;} */
					if(scan.sum <= config.array[i][3] && scan.sum >= config.array[i][7]){ matches[i]++;}
					if(matches[i] == 1)
					{
						check = (i == 0)?RED:(i == 1)?ORANGE:(i == 2)?YELLOW:(i == 3)?GREEN:PURPLE;
						i = 5;
					}
			}
		}
	}

	spitSkittle(check);

}

void config(){


	Skittle returnSkittle;
	returnSkittle.R = 0;
	returnSkittle.G = 0;
	returnSkittle.B = 0;
	returnSkittle.normal = 0;
	A2D config;
	TFileHandle fout;
	homeMotorC();

	bool fileOkay = openWritePC(fout, "CONFIG.txt", 1000); // opening the file, so that the full config can go through, thus never having a blank config
	for(int k = 0; k < 5; k++){
		int maxR = 0, maxG = 0, maxB = 0, maxSum = 0, minR = 0, minG = 0, minB = 0, minSum = 0, maxNormal = 0; // values to be put into the config file.
		int normals[5] = {0, 0, 0, 0, 0}; // int for each possible colour
		//moveAndScan(returnSkittle, false, config);
		for(int i = 0; i < 5; i++){ // test a skittle colour and record its RGB values.
			returnSkittle.sum = 0;
			homeMotorC();
			moveAndScan(returnSkittle, false, config);
			if(returnSkittle.sum != 0 || !(i == 0 && k == 0))
			{
				if(maxR < returnSkittle.R) maxR = returnSkittle.R;
				if(maxG < returnSkittle.G) maxG = returnSkittle.G;
				if(maxB < returnSkittle.B) maxB = returnSkittle.B;
				if(maxSum < returnSkittle.sum) maxSum = returnSkittle.sum;

				if(minR == 0) minR = maxR;
				if(minG == 0) minG = maxG;
				if(minB == 0) minB = maxB;
				if(minSum == 0) minSum = maxSum;

				if(minR > returnSkittle.R) minR = returnSkittle.R;
				if(minG > returnSkittle.G) minG = returnSkittle.G;
				if(minB > returnSkittle.B) minB = returnSkittle.B;
				if(minSum > returnSkittle.sum) minSum = returnSkittle.sum;

				// This part checks to see if the default colour has been recorded already. If so, add it to the list. Otherwise, get rid of it.
				for(int j = 0; j < 5; j++){
					if(normals[j] == returnSkittle.normal){
						j = 5;
					}
					else if(normals[j] == 0){
						normals[j] = returnSkittle.normal;
						maxNormal++;
						j = 5;
					}
				}
			}
		}
		// Time to write the data to the text file.
		writeLongPC(fout, maxR);
		writeTextPC(fout, " ");
		writeLongPC(fout, maxG);
		writeTextPC(fout, " ");
		writeLongPC(fout, maxB);
		writeTextPC(fout, " ");
		writeLongPC(fout, maxSum);
		writeTextPC(fout, " ");
		writeLongPC(fout, minR);
		writeTextPC(fout, " ");
		writeLongPC(fout, minG);
		writeTextPC(fout, " ");
		writeLongPC(fout, minB);
		writeTextPC(fout, " ");
		writeLongPC(fout, minSum);
		writeTextPC(fout, " ");
		writeLongPC(fout, maxNormal);
		writeTextPC(fout, " ");
		for(int i = 0; i < maxNormal; i++){
			writeLongPC(fout, normals[i]);
			writeTextPC(fout, " ");
		}
		writeEndlPC(fout);
		homeMotorC();
	}

	closeFilePC(fout);
}

void loadConfig(A2D loadTo) // load data from the config file and add it to the 2D array
{
	TFileHandle fin;
	bool okay = openReadPC(fin, "CONFIG.txt");
	int normals = 0;

	for(int i = 0; i < 5; i++)
	{
		for(int j = 0; j < 8; j++){
			readIntPC(fin, loadTo.array[i][j]);
			loadTo.array[i][j] += (j < 4)?5:-5;
		}

		readIntPC(fin, normals);
		loadTo.array[i][8] = normals;
		for(int j = 0; j < normals; j++)
		{
			readIntPC(fin, loadTo.array[i][j+9]);
		}

	}

	closeFilePC(fin);


}

void moveAndScan(Skittle colour, bool sort, A2D config){
	bool done = false, scanned = false, once = false;
	nMotorEncoder[motorC] = 0;
	motor[motorC] = motorSpeed;
	time1[T1] = 0;
	moveMotorPID(48, motorC);
	wait1Msec(100);
	if(scanForColour(colour))
			{
			 		if(sort)sortSkittle(config, colour);
					eraseDisplay();
					displayString(0, "%i", colour.R);
					displayString(1, "%i", colour.G);
					displayString(2, "%i", colour.B);
					displayString(3, "%i", colour.normal);
					displayString(4, "%i", colour.sum);
					scanned = true;
			}

		moveMotorPID(43, motorC);
		homeMotorC();


	// keep on movig the skittle loader until the encoders have gotten far enough.
	//while(!done){
		/*

		if(nMotorEncoder[motorC] > 90){
  		startTask(doit);
  		done = true;
 	 	}
		/*
 	 	if(nMotorEncoder[motorC] >= 49 && !scanned){
 	 		if(once){
 	 			scanned = true;
 	 		}

 	 		if(nMotorEncoder[motorC] > 49 && !once){
 	 			motor[motorC] = -motorSpeed;
 	 			while(nMotorEncoder[motorC] > 35);
 	 			motor[motorC] = motorSpeed;
 	 			once = true;
 	 		}


			if(sensorValue[S3] != 6 && nMotorEncoder[motorC] < 70) { // double check colour to remove false positivies
				if(scanForColour(colour))
					{
			 		if(sort)sortSkittle(config, colour);
					eraseDisplay();
					displayString(0, "%i", colour.R);
					displayString(1, "%i", colour.G);
					displayString(2, "%i", colour.B);
					displayString(3, "%i", colour.normal);
					displayString(4, "%i", colour.sum);
					motor[motorC] = motorSpeed;
					scanned = true;
				}
			}
			if(nMotorEncoder[motorC] >= 58 && once){
				scanned = true;
			}
		}
		*/
//	}


}

void spitSkittle(SkittleColour colour){

		if(colour == ORANGE){
			eraseDisplay();
			displayString(5, "ORANGE");
			moveMotorPID(62, motorB);
		}
		else if(colour == RED){
			eraseDisplay();
			displayString(5, "RED");
			moveMotorPID(80, motorB);
		}
		else if(colour == PURPLE){
			eraseDisplay();
			displayString(5, "PURPLE");
			moveMotorPID(0, motorB);
		}
		else if(colour == YELLOW){
			eraseDisplay();
			displayString(0, "YELLOW");
			moveMotorPID(25, motorB);
	}
	else if(colour == GREEN){
			eraseDisplay();
			displayString(5, "GREEN");
			moveMotorPID(42, motorB);
	}else
	{
		eraseDisplay();
		displayString(5, "I'm Confused");
		moveMotorPID(100, motorB);
	}
}

/*
void spitSkittle(Skittle read, A2D config){

		if(read.normal == 5 && read.R >= 40){
			eraseDisplay();
			displayString(5, "ORANGE");
			moveMotorB(90);
		}
		else if(read.normal == 5 && read.R >= 30 && read.R < 40){
			eraseDisplay();
			displayString(5, "RED");
			moveMotorB(110);
		}
		else if((read.normal == 5 && read.R < 30) || read.R == 1){
			eraseDisplay();
			displayString(5, "PURPLE");
			moveMotorB(5);
		}
		else if(read.normal == 4 && read.R >= 40){
			eraseDisplay();
			displayString(0, "YELLOW");
			moveMotorB(35);
	}
	else if((read.normal == 4 && read.R > 20 && read.R < 40) || (read.normal == 3)){
			eraseDisplay();
			displayString(5, "GREEN");
			moveMotorB(60);
	}else
	{
		eraseDisplay();
		displayString(5, "I'm Confused");
	}
}

*/

bool scanForColour(Skittle read){
		//while(nMotorEncoder[motorC] <= 50;
		motor[motorC] = 0;
		wait1Msec(200);
		bool done = false;
		time1[T1] = 0;
		int count = 0;

		while(!done)
		{
		if(SensorValue[S3] != 6){
			/*
			SensorType[S3] = sensorColorNxtRED;
			wait1Msec(500);
			read.R = SensorRaw[S3];

			SensorType[S3] = sensorColorNxtGREEN;
			wait1Msec(500);
			read.G = SensorRaw[S3];
			SensorType[S3] = sensorColorNxtBLUE;
			wait1Msec(500);
			read.B = SensorRaw[S3];

			SensorType[S3] = sensorColorNxtFULL;
			wait1Msec(500);
			getColorSensorData();
			read.normal = SensorValue[S3];
			read.sum = read.R + read.G + read.B;
*/

		short nRawValue[4];
		read.R = 0;
		read.G = 0;
		read.B = 0;
		read.normal = 0;
		read.sum = 0;

		for(int i = 0; i < 3; i++)
			{
			getColorSensorData(S3, colorRaw, nRawValue);
			read.R += nRawValue[0];
			read.G += nRawValue[1];
			read.B += nRawValue[2];
			read.normal += sensorValue[S3];
			read.sum = read.R + read.G + read.B;
		}

		read.R /= 3;
		read.G /= 3;
		read.B /= 3;
		read.normal /= 3;
		read.sum /= 3;

		for(int i = 0; i < 4; i++)
		{
			displayString(i, "%i", nRawValue[i]);
		}
		//wait1Msec(3000);

			return true;
			}
			else if(time1[T1] > 500){
				homeMotorC();
				moveMotorPID(48, motorC);
				time1[T1] = 0;
				count++;
			}

			if(count == 1){
				startTask(moveMotorA);
			}

			if(count >=5) {
				done = true;
				read.R = -1;
				read.G = -1;
				read.B = -1;
				read.normal = -1;
				read.sum = -1;
			}
		}

			return false;
}

void moveMotorB(int toDist){
	nMotorEncoder[motorB] = 0;
	motor[motorB] = 10;
	while(nMotorEncoder[motorB] < toDist);
	motor[motorB] = 0;
}

float PIDcontroller(int error, int integral, int derivative, float Kp, float Ki, float Kd)
{
	return(Kp * error) + ( Ki * integral) + ( derivative* Kd);
}

void moveMotorPID(int toDist, int motorNum){
	nMotorEncoder[motorNum] = 0;
	float integralMotor = 0;
	int derivativeMotor = 0;
	int degMotor = 0;
	int prevErrorMotor = 0;
	int errorMotor = 0;
	float PIDmotor = 0;
	float currentTime = 0;
	bool done = false;
	time1[T1] = 0;
	wait1Msec(20);

	while(!done){
		degMotor = nMotorEncoder[motorNum];
		prevErrorMotor = errorMotor;
		errorMotor = toDist - degMotor;
		integralMotor += (errorMotor) * (currentTime/time1[T1]);
		currentTime = time1[T1];
		derivativeMotor = errorMotor - prevErrorMotor;
		PIDmotor = PIDcontroller(errorMotor, integralMotor, derivativeMotor, 0.1, 0.0001, 75);
		motor[motorNum] = PIDmotor;
		if(errorMotor == 0 && derivativeMotor == 0) done = true;
	}

	motor[motorNum] = 0;
}

void homeMotorB(){
	motor[motorB] = -5;
	while(SensorValue[S2] != 1);
	motor[motorB] = 0;
}

void homeMotorC(){
	motor[motorC] = -15;
	while(SensorValue[S1] != 1);
	motor[motorC] = 0;
}



task main()
{
	A2D holdConfig;
	SensorType[S1] = sensorTouch; // homing sensor for motor C
	SensorType[S2] = sensorTouch; // homing sensor for motor B
	SensorType[S3] = sensorColorNxtFULL; // colour sensor
	homeMotorB();		// Motor B is skittle slide
	homeMotorC();		// Motor C is the drop table
	reset2DArray(holdConfig);

	wait1Msec(1000);
	startTask(moveMotorA);
	//startTask(showEncoder);

	displayString(0, "Arrows config, Orange sort");
	while(nNxtButtonPressed == -1);
	if(nNxtButtonPressed == 3)
	{
		loadConfig(holdConfig);
		Skittle test;
		while(1)
		{
			moveAndScan(test, true, holdConfig);
			startTask(doit);
			homeMotorC();

		}
	}
	else
	{
		config();
	}


	/*omeMotorB();
	moveMotorPID(25, motorB); // 80 62 42 25
	wait1Msec(1000);
	homeMotorB();
	moveMotorPID(42, motorB); // 80 62 42 25
	wait1Msec(1000);
		homeMotorB();
	moveMotorPID(62, motorB); // 80 62 42 25
	wait1Msec(1000);
		homeMotorB();
	moveMotorPID(80, motorB); // 80 62 42 25
	wait1Msec(1000);

	/*
	 */

	//nMotorPIDSpeedCtrl[motorC] = mtrNoReg;

	//sort(holdConfig);
	//testMotorC();
}
