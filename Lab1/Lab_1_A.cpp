//============================================================================
// Name        : Lab_1_A.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <unistd.h>     // sleep, usleep functions
#include <wiringPi.h>   // needed for the wiringPi functions
using namespace std;

#define RedLED      8       // wiringPi number corresponding to GPI03.
#define YellowLED   9       // wiringPi number corresponding to GPI05.
#define GreenLED    7       // wiringPi number corresponding to GPIO7.
#define BlueLED     21      // wiringPi number corresponding to GPI29.
//#define Speaker     22      // wiringPi number corresponding to GPI31.

int main()
{
	wiringPiSetup();

	// The program will turn off the red LED, sleep for a while, then on, sleep, off, on and off.
	// You could use loops, if you wanted/needed.

	pinMode(RedLED, OUTPUT);	// Configure GPIO2, which is the one connected to the red LED.
        pinMode(YellowLED, OUTPUT);
        pinMode(GreenLED, OUTPUT);
        pinMode(BlueLED, OUTPUT);
        
        digitalWrite(RedLED, LOW);
        digitalWrite(YellowLED, LOW);
        digitalWrite(GreenLED, LOW);
        digitalWrite(BlueLED, LOW);
        
        while(1)
	{
                //Event 1
                digitalWrite(RedLED, HIGH);
		digitalWrite(BlueLED, LOW);
                sleep(1);
                
                //Event 2
                digitalWrite(RedLED, LOW);
                digitalWrite(BlueLED, HIGH);
                sleep(1);
	}

	return 0;
}
