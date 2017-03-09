#include "Arduino.h"

//globals for both functions
#define choosepin 2
#define chooseled 10
//globals for chronograph
#define AbuttonPin 4
#define BbuttonPin 3


float startTime; // start time for stop watch
float endTime;
float elapsedTime; // elapsed time for stop watch

// globals for display
const int latchPin = 12; //Pin 12 connected to ST_CP of 74HC595
const int clockPin = 8; //Pin 8 connected to SH_CP of 74HC595
const int dataPin = 11; //Pin 11 connected to DS of 74HC595
const int decipin = 9;
const int readpin1 = 6;
const int readpin2 = 7;
const int switchpin = 5;
#define pentometerpin A0

int startnum = 25;
int num = 0;

//display 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F
int datArray[17] = { 252, 96, 218, 242, 102, 182, 190, 224, 254, 246, 238, 62,
		156, 122, 158, 142, B0000000 };

int check_dial() {

	int dial_reading = analogRead(pentometerpin);
	int add = 0;
	if (dial_reading > 870) { // 5 stars
		add = 5;
	} else if (dial_reading > 750) { // 4 stars
		add = 4;
	} else if (dial_reading > 450) { // 3 stars
		add = 3;
	} else if (dial_reading > 350) { // 2 stars
		add = 2;
	} else if (dial_reading > 250) { // 1 stars
		add = 1;
	} else if (dial_reading > 0) { // 0 stars
		add = 0;
	}
	return add;
}

void disp_sev_seg(int num7) {

	digitalWrite(latchPin, LOW); //ground latchPin and hold low for as long as you are transmitting
	shiftOut(dataPin, clockPin, MSBFIRST, datArray[num7]);
	//return the latch pin high to signal chip that it
	//no longer needs to listen for information
	digitalWrite(latchPin, HIGH); //pull the latchPin to save the data
	delay(10);
}

// Chronograph function code
void chronograph() {

	disp_sev_seg(16); // sev seg off

	while (!digitalRead(AbuttonPin)) {
		if (digitalRead(choosepin)) {
			return;
		}
	}

	startTime = micros();

	do {

	} while (!digitalRead(BbuttonPin));

	endTime = micros();

	elapsedTime = endTime - startTime;
	float fps = (5000000 / (elapsedTime));
	Serial.print(fps);
	Serial.println(" fps");

	int digit1 = (int) fps % (10);
	int digit2 = (int) fps % (100);
	int digit3 = (int) fps % (1000);


	disp_sev_seg((digit3 - digit2)/100);
	delay(1400);
	disp_sev_seg(16); // sev-seg off
	delay(600);

	disp_sev_seg((digit2 - digit1)/10);
	delay(1400);
	disp_sev_seg(16); // sev-seg off
	delay(600);

	disp_sev_seg(digit1);
	delay(1400);
	disp_sev_seg(16); // sev-seg off

	return;
}

// display function
void display() {
	while (true) {

		if (!digitalRead(choosepin)) {
			return;
		}
		while (digitalRead(switchpin)) {

			startnum = 20 + check_dial();
			num = startnum;

			disp_sev_seg(num % 10);
			digitalWrite(decipin, HIGH);
		}

		if (digitalRead(readpin1)) {

			delay(150); //wait for a second

			if (num > 9) { // for when over nine index num accomadtions have to be made
				disp_sev_seg(num % 10);
				num = num - 1;

				while (digitalRead(readpin1)) {
				} // wait till button is unpushed
				delay(10); // play nice

			} else { // else set number as normal

				disp_sev_seg(num);
				digitalWrite(decipin, LOW);
				num = num - 1;

				while (digitalRead(readpin1)) {
				} // wait till button is unpushed
				delay(10); // play nice
			}

		}

		if (digitalRead(readpin2)) { //reset the number back to F (full) on pressing mag release
			num = startnum;

			disp_sev_seg(15);

			digitalWrite(decipin, HIGH);
			num = num - 1;
		}

		while (!digitalRead(readpin2) && num < 0 && !digitalRead(switchpin)) { // flash a zero on empty mag

			disp_sev_seg(16);
			if (digitalRead(readpin2)) {
				break;
			} //mid-function interrupt
			delay(200);

			disp_sev_seg(14);
			if (digitalRead(readpin2)) {
				break;
			} //mid-function interrupt
			delay(200);
		}
	}
}

void setup() {
	//Chronograph setup
	pinMode(AbuttonPin, INPUT);
	pinMode(BbuttonPin, INPUT);

	digitalWrite(AbuttonPin, HIGH);
	digitalWrite(BbuttonPin, HIGH);

	//Display setup
	//set pins to output
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);

	pinMode(readpin1, INPUT);
	pinMode(readpin2, INPUT);

	pinMode(pentometerpin, INPUT);
	pinMode(switchpin, INPUT);
	pinMode(choosepin, INPUT);

	//Serial port setup
	Serial.begin(9600);

}

void loop() {

	if (digitalRead(choosepin)) {
		Serial.println("starting display");
		digitalWrite(chooseled, LOW);
		display();
	} else {
		Serial.println("starting chronograph");
		digitalWrite(chooseled, HIGH);
		chronograph();
	}
}

