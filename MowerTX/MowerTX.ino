#include <PS2X_lib.h>  //for v1.6

PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning 
//you must always either restart your Arduino after you conect the controller, 
//or call config_gamepad(pins) again after connecting the controller.
int error = 0; 
byte type = 0;
byte vibrate = 0;

static byte start_byte = 0x1B;
static byte stop_byte = 0x0A;

bool handbrake = true;

int left_stick = 0;
int right_stick = 0;

static int upper_deadzone = 0;
static int lower_deadzone = 0;

void setup()
{
Serial.begin(57600);

 //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************
  
 error = ps2x.config_gamepad(13,11,10,12, true, true);   //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
 
 if(error == 0){
   Serial.println("Found Controller, configured successful");
   Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
   Serial.println("holding L1 or R1 will print out the analog stick values.");
   Serial.println("Go to www.billporter.info for updates and to report bugs.");
 }
   
  else if(error == 1)
   Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
  else if(error == 2)
   Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
   
  else if(error == 3)
   Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
   
   //Serial.print(ps2x.Analog(1), HEX);
   
   type = ps2x.readType(); 
     switch(type) {
       case 0:
        Serial.println("Unknown Controller type");
       break;
       case 1:
        Serial.println("DualShock Controller Found");
       break;
       case 2:
         Serial.println("GuitarHero Controller Found");
       break;
     }
}

void transmit_values()
{
	Serial.write(start_byte);
	Serial.write(left_stick);
	Serial.write(right_stick);
	Serial.write(stop_byte);
}


void loop()
{

 /* You must Read Gamepad to get new values
   Read GamePad and set vibration values
   ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
   if you don't enable the rumble, use ps2x.read_gamepad(); with no values
   you should call this at least once a second
   */
    
 if(error == 1) //skip loop if no controller found
  return; 
  
    ps2x.read_gamepad();          //read controller
      
    if(ps2x.ButtonPressed(PSB_PINK))             //Press Square to set the Handbrake
         handbrake = true;     
    
    if(ps2x.ButtonPressed(PSB_BLUE)) { //Press X to release the handbrake as long as the joysticks are centered
        if(ps2x.Analog(PSS_LY) < upper_deadzone && ps2x.Analog(PSS_LY) > lower_deadzone){
			if(ps2x.Analog(PSS_RY) < upper_deadzone && ps2x.Analog(PSS_RY) > lower_deadzone){
				if(ps2x.Analog(PSS_LX) < upper_deadzone && ps2x.Analog(PSS_LX) > lower_deadzone){
					if(ps2x.Analog(PSS_RX) < upper_deadzone && ps2x.Analog(PSS_RX) > lower_deadzone){
						handbrake = false;
					}
				}
			}
		}
	}

  if(handbrake == false){
	left_stick = ps2x.Analog(PSS_LY);
	right_stick = ps2x.Analog(PSS_RY);
  }
  else{
	  left_stick = 127;
	  right_stick = 127;
  }

  transmit_values();

 delay(50);

}
