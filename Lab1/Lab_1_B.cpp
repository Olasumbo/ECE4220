#include <iostream>
#include <unistd.h>     // sleep, usleep functions
#include <wiringPi.h>   // needed for the wiringPi functions
using namespace std;


#define Speaker     22      // wiringPi number corresponding to GPI31.
#define Button1     27   
#define Button2     0
#define Button3     1
#define Button4     24
#define Button5     28


int main()
{
	wiringPiSetup();

	// The program will turn off the red LED, sleep for a while, then on, sleep, off, on and off.
	// You could use loops, if you wanted/needed.

	pinMode(Speaker, OUTPUT);	// Configure GPIO2, which is the one connected to the red LED.
        
        int x;
        cout << "Enter an interger between 1 to 5" << endl;
        cin >> x;
        while(x < 1 || x > 5)
        {
          cout << " You suck, Read the damn intructions" << endl;
          cout << "Enter an interger between 1 to 5" << endl;
          cin >> x;
        }
        
        int btn = 0;
        switch(x)
        {
            case 1:
                btn = Button1;
                break;
            case 2:
                btn = Button2;
                break;
            case 3:
                btn = Button3;
                break;
            case 4:
                btn = Button4;
                break;
            case 5:
                btn = Button5;
                break;
        }
        pullUpDnControl( btn , PUD_DOWN); //it decides whether the actual button is on or off (down or up)
        
        cout << "STUFFLES: " << btn << " | " << x  << endl;
        
        while( digitalRead(btn) == 0)  // This compared the singnal in btn to see if it is off
        {
            //waiting for the button to turnoff
            //PS digitialRead only reads in 0 or 1
        }
        
        while(1)
        {
                digitalWrite(Speaker, HIGH);
                usleep( 750 );
                digitalWrite(Speaker, LOW);
                usleep( 750 );          
        }
        
      
	return 0;
}