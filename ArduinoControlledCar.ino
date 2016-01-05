// Code taken from the 'Arduino Controlled Toy Car' Instructables & Hacks @ http://www.instructables.com/id/Autonomous-Control-of-RC-Car-Using-Arduino/, http://mods-n-hacks.wonderhowto.com/how-to/new-arduino-start-with-simple-rc-car-controlled-by-your-android-device-0141495/ 
// The code enables Arduino Microcontroller to act as the remote for the toy car

//The wiring connections are determined by using a multimeter and identifying the forward, reverse, left and right pins on the RX chip of RC car

int forward = 11; // Green wire on RC Car
int reverse = 10; // Blue wire on RC Car
int left = 6; // Yellow wire on RC Car
int right =7; // Black wire on RC Car

// The setup() method runs once, when the sketch starts 
void setup() 
{ 
// initialize the digital pins as outputs 
     pinMode(forward, OUTPUT); 
     pinMode(reverse, OUTPUT); 
     pinMode(left, OUTPUT); 
     pinMode(right, OUTPUT);
     digitalWrite(reverse,LOW);     // turn revers motor off
     digitalWrite(forward,LOW);     // turn forward motor off
     digitalWrite(left,LOW);        // turn left motor off
     digitalWrite(right,LOW);       // turn right motor off
 } 

void go_forward()
{ 
     digitalWrite(reverse,LOW); // turn revers motor off 
     digitalWrite(forward,HIGH); // turn forward motor on 
     digitalWrite(left,LOW); 
     digitalWrite(right,LOW); 
} 

void go_reverse() 
{ 
     digitalWrite(forward,LOW); // turn forward motor off
     digitalWrite(reverse,HIGH); // turn reverse motor on
     digitalWrite(left,LOW); 
     digitalWrite(right,LOW); 
} 

void go_left() 
{ 
     digitalWrite(right,LOW); // turn right motor off 
     digitalWrite(left,HIGH); // turn left motor on 
     digitalWrite(reverse,LOW); // turn revers motor off 
     digitalWrite(forward,LOW); // turn forward motor off 
} 

void go_right() 
{ 
     digitalWrite(left,LOW); // tune left motor off 
     digitalWrite(right,HIGH); // turn right motor on 
     digitalWrite(reverse,LOW); // turn revers motor off 
    digitalWrite(forward,LOW); // turn forward motor off 
} 

// the loop() method runs over and over again, // as long as the Arduino has power 
void loop()
{ 
     go_forward(); 
     delay(1000); 
     go_right(); 
     delay(3000); 
     go_reverse(); 
     delay(1000); 
     go_left(); 
     delay(3000);  
}

