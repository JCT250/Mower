bool process = false;
bool drive = false;

byte start_byte = 0x1B;
byte end_byte = 0x0A;

unsigned long inTime = 0;

byte inData;
byte inArray[4];

void setup()
{
	Serial.begin(9600);

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

}

void drive_motors() //drives the motors
{

}


void loop()
{
	process = false;
	drive = false;
	serial_check();
	if(process == true) serial_process();
	if(drive == true) drive_motors();

}
