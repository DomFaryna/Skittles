#include "NXT_FileIO.c"

void homeMotorB();
void moveMotorB(int toDist);

struct Skittle{
	int normal;
	int R;
	int G;
	int B;
}

struct A2D{
	int array[4][4];
}

struct A1D{
	int array[4];
}
void config(Skittle returnSkittle){

	int raw[4][3];
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 3; j++){
			raw[i][j] = 0;
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

task doit(){
	wait1Msec(200);
	homeMotorB();
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


void testMotorC(){
	int go = -15;
	Skittle colour;
	while(1){
	motor[motorC] = go;
	if(SensorValue[S1] == 1){
		go = 0;
		nMotorEncoder[motorC] = 0;
		motor[motorC] = go;
		wait1Msec(1000);
		go = 2;
		motor[motorC] = go;
		while(SensorValue[S1] == 1);
	}
	if(nMotorEncoder[motorC] > 90){
		go = -20;
		startTask(doit);
	}

	if(SensorValue[S3] != 6 && go > 0){
		wait1Msec(100);
		if(SensorValue[S3] != 6)	scanForColour(colour);
		go = 5;

 		spitSkittle(colour);
		eraseDisplay();
		displayString(0, "%i", colour.R);
		displayString(1, "%i", colour.G);
		displayString(2, "%i", colour.B);
		displayString(3, "%i", colour.normal);

		motor[motorC] = go;
		wait1Msec(500);

	}



	}

}

void testMotorB(){
	int go = -15;
	int pos[5];
	int count = 0;
	const int zero = 0;
	for(int i = 0; i < 5; i++){
	pos[i] = (i+1)*20;
}

	while(1){
	motor[motorB] = go;
	if(SensorValue[S2] == 1){
		go = 0;
		nMotorEncoder[motorB] = 0;
		motor[motorB] = go;
		wait1Msec(1000);
		go = 15;
		motor[motorB] = go;
		wait1Msec(200);
	}
	if(nMotorEncoder[motorB] > pos[count]){
		go = -15;
		count++;
		if(count == 5){ count = zero;}
		wait1Msec(10);
	}


	displayString(0, "%i", count);

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
	//config();

	testMotorC();


}
