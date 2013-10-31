bool process = false;
bool drive = false;

byte start_byte = 0x1B;
byte end_byte = 0x0A;

unsigned long inTime = 0;

byte inData;
byte inArray[4];

static int left_speed_output = 0;
static int left_direction_output = 0;
static int right_speed_output = 0;
static int right_direction_output = 0;
static int brakes = 0;

int left_speed = 0;
int left_direction = 0;
int left_direction_old = 0;
int right_speed = 0;
int right_direction = 0;
int right_direction_old = 0;
volatile int handbrake = 1;

void setup()
{
	Serial.begin(9600);


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

		inTime = millis(); //set the time that checking and receiving finished

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
		left_direction = 0;
		left_speed = map(inArray[1], 0,126,255,0);
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
		right_speed = map(inArray[2], 0,126,255,0);
	}
	if(inArray[2] > 128)
	{
		handbrake = 0;
		right_direction = 1;
		right_speed = map(inArray[2], 129,255,0,255);
	}
	drive = true;
}

void drive_motors() //drives the motors
{
	if(handbrake == 0){
			
			digitalWrite(brakes, HIGH); //make sure brakes are off
			
			if(left_direction != left_direction_old) //if we need to change direction 
			{
				analogWrite(left_speed_output, 0); //stop the bot
				delay(10); //see what happens with this, we may not need it
				digitalWrite(left_direction_output, left_direction); //set the new direction
				left_direction_old = left_direction; // update the stored value
				delay(10); //ditto with this one, may not be needed
			}

			analogWrite(left_speed_output, left_speed); //write the new speed to the controller
		
			if(right_direction != right_direction_old)
			{
				analogWrite(right_speed_output, 0);
				delay(10);
				digitalWrite(right_direction_output, right_direction);
				right_direction_old = right_direction;
				delay(10);
			}

			analogWrite(right_speed_output, right_speed);
		
	}
	if(handbrake == 1)
	{
		stop();
	}
}

void stop()
{
	
	analogWrite(left_speed_output, 0);
	analogWrite(right_speed_output, 0);
	digitalWrite(brakes, LOW);


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
