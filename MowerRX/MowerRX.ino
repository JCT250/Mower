
#include <SoftwareSerial.h>
#include <Sabertooth.h>


bool process = false;
bool drive = false;

byte start_byte = 0x1B;

byte inData;
byte inArray[4];

static int brakes = 0;

int left_speed = 0;
int right_speed = 0;
volatile int handbrake = 1;

SoftwareSerial SWSerial(NOT_A_PIN, 11); // RX on no pin (unused), TX on pin 11 (to S1).
Sabertooth ST(128, SWSerial);

void setup()
{
	Serial.begin(9600);
	SWSerial.begin(9600);
	//ST.setRamping(0);
	//ST.setMinVoltage(30);
	ST.setTimeout(2000);

	pinMode(brakes, OUTPUT);
	digitalWrite(brakes, LOW);
}

void serial_check()		//Checks for new data in the serial buffer and if available stores in a string
{
	int i = 0;
	inData = 0x7F; // fill the array with a value that represents a 0 throttle value (just in case there are problems reading the serial string and odd values are ending up in the array)
	if(Serial.available() > 3){ //find the start byte
		while(inData != start_byte){ 
			inData = Serial.read();
		} 
		inArray[0] = inData;
		inArray[1] = Serial.read();
		inArray[2] = Serial.read();
	
		for(i=0; i++; i<4){ //echo the array to the serial port for troubleshooting
			Serial.print(inArray[i]);
			Serial.print(" ");
		}

		Serial.println();
		process = true;
	}
}

void serial_process() //process serial string to give a value ready to send to motor controller
{ 
	left_speed = 0;
	right_speed = 0;
	handbrake = 1;
	if(inArray[1] < 127) //127 and 128 are full stop commands, less than 127 is reverse, greater than 128 is foreward
	{
		handbrake = 0;
		left_speed = map(inArray[1], 0,127,127,0);
	}
	if(inArray[1] > 128)
	{
		handbrake = 0;
		left_speed = map(inArray[1], 128,255,0,-127);
	}
	if(inArray[2] < 127)
	{
		handbrake = 0;
		right_speed = map(inArray[2], 0,127,127,0);
	}
	if(inArray[2] > 128)
	{
		handbrake = 0;
		right_speed = map(inArray[2], 128,255,0,-127);
	}
	drive = true;
}

void drive_motors() //drives the motors
{
	if(handbrake == 0){
			
		digitalWrite(brakes, HIGH); //make sure brakes are off
		ST.motor(1, left_speed);
		ST.motor(2, right_speed);
	}

	if(handbrake == 1)
	{
		stop();
	}
}

void stop()
{
	ST.motor(1, 0);
	ST.motor(2, 0);
	delay(50);
	digitalWrite(brakes, LOW);
}

void loop()
{
	process = false;
	drive = false;
	serial_check();
	if(process == true) serial_process();
	if(drive == true) drive_motors();
}
