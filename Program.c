#include "NXT_FileIO.c"

struct Skittle{	// a struct used to store skittle information
	int normal; // The colour the nxt thinks the skittle is. E.g. 5 = red, 4 = yellow, 1 = black, etc
	int R; // the R value of the skittle
	int G; // the G value of the skittle
	int B; // the B value of the skittle
	int sum; // The sum of the RGB values
}

struct A2D{		// a sturct used to create and pass 2D arrays throughout the rest of the program
	int array[5][13];
}

struct A1D{		// a struct used to create and pass 1D arrays throughout the rest of the program
	int array[4];
}

const int motorSpeed = 3;
void homeMotorB();
void homeMotorC();
void moveMotorB(int toDist);
bool scanForColour(Skittle read);
void spitSkittle(Skittle read, A2D config);
void moveAndScan(Skittle colour, bool sort, A2D config);

task doit(){	// seperate thread used to home the slide motor to save time.
	wait1Msec(200); // need the delay to start thread. No idea why, but it works.
	homeMotorB();
}

task moveMotorA(){
	while(1){
		wait1Msec(100000);
		motor[motorA] = 20;
		wait1Msec(5000);
		motor[motorA] = -20;
		wait1Msec(5000);
		motor[motorA] = 0;
		wait1Msec(20000);
	}
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

void sort(A2D config)
{
	Skittle scan;

}

void config(){

	Skittle returnSkittle;
	A2D config;
	TFileHandle fout;
	bool fileOkay = openWritePC(fout, "CONFIG.txt", 1000); // opening the file, so that the full config can go through, thus never having a blank config
	for(int k = 0; k < 5; k++){
		int maxR = 0, maxG = 0, maxB = 0, maxSum = 0, minR = 0, minG = 0, minB = 0, minSum = 0, maxNormal = 0; // values to be put into the config file.
		int normals[5] = {0, 0, 0, 0, 0}; // int for each possible colour

		for(int i = 0; i < 5; i++){ // test a skittle colour and record its RGB values.
			homeMotorC();
			moveAndScan(returnSkittle, false, config);
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
	bool okay = openReadPc(fin, "CONFIG.txt");
	int normals = 0;

	for(int i = 0; i < 5; i++)
	{
		for(int j = 0; j < 8; j++){
			readIntPC(fin, loadTo.array[i][j]);
		}

		readIntPC(fin, normals);
		for(int j = 0; j < normals; j++)
		{
			readIntPC(fin, loadTo.array[i][j+8]);
		}

	}

	closeFilePC(fin);
}

void moveAndScan(Skittle colour, bool sort, A2D config){
	bool done = false, scanned = false, once = false;;
	nMotorEncoder[motorC] = 0;
	motor[motorC] = motorSpeed;
	// keep on movig the skittle loader until the encoders have gotten far enough.
	while(!done){
		if(nMotorEncoder[motorC] > 90){
  		startTask(doit);
  		done = true;
 	 	}

 	 	if(nMotorEncoder[motorC] >= 47 && !scanned){
 	 		if(nMotorEncoder[motorC] > 47 && !once){
 	 			motor[motorC] = -motorSpeed;
 	 			while(nMotorEncoder[motorC] > 35);
 	 			motor[motorC] = motorSpeed;
 	 			once = true;
 	 		}
			if(SensorValue[S3] != 6) { // double check colour to remove false positivies
				if(scanForColour(colour))
					{
			 		if(sort)spitSkittle(colour, config);
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
		}
	}


}

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

bool scanForColour(Skittle read){
		motor[motorC] = 0;
		wait1Msec(200);
		if(SensorValue[S3] != 6){
			SensorType[S3] = sensorColorNxtRED;
			wait1Msec(500);
			read.R = SensorValue[S3];

			SensorType[S3] = sensorColorNxtGREEN;
			wait1Msec(500);
			read.G = SensorValue[S3];
			SensorType[S3] = sensorColorNxtBLUE;
			wait1Msec(500);
			read.B = SensorValue[S3];

			SensorType[S3] = sensorColorNxtFULL;
			wait1Msec(500);
			read.normal = SensorValue[S3];
			read.sum = read.R + read.G + read.B;

			return true;
		}

		else
		{
			motor[motorC] = motorSpeed;
			return false;
		}
}

void moveMotorB(int toDist){
	nMotorEncoder[motorB] = 0;
	motor[motorB] = 10;
	while(nMotorEncoder[motorB] < toDist);
	motor[motorB] = 0;
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
	SensorType[S1] = sensorTouch; // declare each sensor
	SensorType[S2] = sensorTouch;
	SensorType[S3] = sensorColorNxtFULL;
	homeMotorB();		// home each motor before doing anything ese
	homeMotorC();
	reset2DArray(holdConfig);

	wait1Msec(1000);
	startTask(moveMotorA);

	//displayStatus();
	config();

	sort(holdConfig);
	//testMotorC();


}
