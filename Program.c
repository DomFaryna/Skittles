#include "NXT_FileIO.c"

struct Skittle{
	int normal;
	int R;
	int G;
	int B;
}

struct A2D{
	int array[5][4];
}

struct A1D{
	int array[4];
}

void homeMotorB();
void homeMotorC();
void moveAndScan();
void moveMotorB(int toDist);
void scanForColour(Skittle read);
void spitSkittle(Skittle read);
void moveAndScan(Skittle colour, bool sort);

task doit(){
	wait1Msec(200);
	homeMotorB();
}

void config(Skittle returnSkittle){

	A2D raw;
	for(int i = 0; i < 4; i++){
		homeMotorC();
		moveAndScan(returnSkittle, false);
		raw.array[i][0] = returnSkittle.normal;
		raw.array[i][1] = returnSkittle.R;
		raw.array[i][2] = returnSkittle.G;
		raw.array[i][3] = returnSkittle.B;
	}

}

void moveAndScan(Skittle colour, bool sort){
	bool done = false;
	nMotorEncoder[motorC] = 0;
	int speed = 2;
	motor[motorC] = speed;
	while(!done){
		if(nMotorEncoder[motorC] > 90){
  		startTask(doit);
  		done = true;
 	 	}

 	 	if(SensorValue[S3] != 6 && speed!=5){
			wait1Msec(100);
			if(SensorValue[S3] != 6) scanForColour(colour);
	 		if(sort)spitSkittle(colour);
			eraseDisplay();
			displayString(0, "%i", colour.R);
			displayString(1, "%i", colour.G);
			displayString(2, "%i", colour.B);
			displayString(3, "%i", colour.normal);
			speed = 5;
			motor[motorC] = speed;
			wait1Msec(500);
		}
	}


}

void spitSkittle(Skittle read){


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

void scanForColour(Skittle read){
		wait10Msec(10);
		motor[motorC] = 0;
		SensorType[S3] = sensorColorNxtRED;
		wait1Msec(1000);
		read.R = SensorValue[S3];

		SensorType[S3] = sensorColorNxtGREEN;
		wait1Msec(1000);
		read.G = SensorValue[S3];
		SensorType[S3] = sensorColorNxtBLUE;
		wait1Msec(1000);
		read.B = SensorValue[S3];

		SensorType[S3] = sensorColorNxtFULL;
		wait1Msec(1000);
		read.normal = SensorValue[S3];

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



void testMotorC(){
	Skittle colour;
	while(1){
	homeMotorC();
	moveAndScan(colour, true);

	}

}


void displayStatus(){
	while(1)
	displayBigTextLine(0, "%i", SensorValue[S3]);
}

task main()
{
	SensorType[S1] = sensorTouch;
	SensorType[S2] = sensorTouch;
	SensorType[S3] = sensorColorNxtFULL;
	homeMotorB();


	//displayStatus();
	Skittle lol;
	config(lol);

	//testMotorC();


}
