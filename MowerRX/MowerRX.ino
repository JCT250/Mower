bool process = false;
bool drive = false;

byte start_byte = 0x1B;
byte end_byte = 0x0A;

unsigned long inTime = 0;

byte inData;
byte inArray[4];

static int left_ALI = 0; //pwm inputs on H-bridge
static int left_BLI = 0;
static int right_ALI = 0;
static int right_BLI = 0;
static int mosfet1 = 0; //powers 12v reg
static int mosfet2 = 0; //powers relay high side
static int BJT1 = 0; //switches relay low side
static int BJT2 = 0; //switches brake BJT
static int H_Enable = 0; // enable pins on H-Bridge

int left_speed = 0;
int left_direction = 0;
int right_speed = 0;
int right_direction = 0;
volatile int handbrake = 1;

void setup()
{
	Serial.begin(9600);
	
	pinMode(left_ALI, OUTPUT);
	pinMode(left_ALI, OUTPUT);
	pinMode(right_ALI, OUTPUT);
	pinMode(right_BLI, OUTPUT);
	pinMode(mosfet1, OUTPUT);
	pinMode(mosfet2, OUTPUT);
	pinMode(BJT1, OUTPUT);
	pinMode(BJT1, OUTPUT);
	pinMode(H_Enable, OUTPUT);


  /* add setup code here */

}

void serial_check()		//Checks for new data in the serial buffer and if available stores in a string
{
	int i = 0;
	inData = 0xFF;
	if(Serial.available() > 3){ //find the start byte
		while(inData != start_byte){ 
			inData = Serial.read();
		} 
		inArray[i] = inData;
		i++;
		while(inData != end_byte){ //then read data into the incoming array until you reach an end byte
			inData = Serial.read();
			inArray[i] = inData;
			i++; //move to next position in the incoming array
		}
	
		for(i=0; i++; i<5){ //echo the array to the serial port for troubleshooting
			Serial.print(inArray[i]);
			Serial.print(" ");
		}
		inTime = millis(); //set the time that checking and receiving finished
		process = true;
	}
}

void serial_process() //process serial string to give a pwm value ready to send to motor driver board
{ 
	left_speed = 0;
	right_speed = 0;
	handbrake = 1;
	if(inArray[1] < 127) //127 and 128 are full stop commands, less than 127 is reverse, greater than 128 is foreward
	{
		handbrake = 0;
		left_direction = 0;
		left_speed = map(inArray[1], 0,126,0,255);
	}
	if(inArray[1] > 128)
	{
		handbrake = 0;
		left_direction = 1;
		left_speed = map(inArray[1], 129,255,0,255);
	}
	if(inArray[2] < 127)
	{
		handbrake = 0;
		right_direction = 0;
		right_speed = map(inArray[2], 0,126,0,255);
	}
	if(inArray[2] > 128)
	{
		handbrake = 0;
		right_direction = 1;
		right_speed = map(inArray[2], 129,255,0,255);
	}
}

void drive_motors() //drives the motors
{
	if(handbrake == 0){
		digitalWrite(BJT2, HIGH); //disengage brakes
		digitalWrite(H_Enable, LOW); //enable outputs on H-Bridge
		if(left_direction == 0) //if left reverse
		{
			analogWrite(left_ALI, 0);
			analogWrite(left_BLI, left_speed);
		}
		if(left_direction == 1) // if left foreward
		{
			analogWrite(left_ALI, left_speed);
			analogWrite(left_BLI, 0);
		}
		if(right_direction == 0) // if right reverse
		{
			analogWrite(right_ALI, 0);
			analogWrite(right_BLI, right_speed);
		}
		if(right_direction == 1) //if right foreward
		{
			analogWrite(right_ALI, right_speed);
			analogWrite(right_BLI, 0);
		}
	}
	if(handbrake == 1)
	{
		stop();
	}
}

void stop()
{
	digitalWrite(H_Enable, HIGH); //disable the outputs on the H-Bridge
	analogWrite(left_ALI, 0); //write 0 to PWM inputs
	analogWrite(left_BLI, 0);
	analogWrite(right_ALI, 0);
	analogWrite(right_BLI, 0);
	digitalWrite(BJT2, LOW); //apply the motor brakes
}

void interrupt()
{
if(inTime > (millis()+500)) stop();
}


void loop()
{
	process = false;
	drive = false;
	serial_check();
	if(process == true) serial_process();
	if(drive == true) drive_motors();
}
