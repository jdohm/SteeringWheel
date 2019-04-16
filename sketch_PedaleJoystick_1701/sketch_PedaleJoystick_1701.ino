//Selfmade Gaming Controlller
//Uses one Potentiometer as Gas_Pedal, one as Break_Pedal and one as steering wheal
//

#include <Joystick.h>
#include <Keyboard.h>

Joystick_ Joystick;

//helpfunction
int16_t subT(uint16_t a, uint16_t b); //returns a-b if a>b, else returns 0
int16_t posdif(uint16_t a, uint16_t b); 

//Values for the pedal
uint16_t BP_NP; 		//Break_Pedal_not_pushed
uint16_t BP_P; 			//Break_Pedal_pushed
uint16_t GP_NP; 		//Gas_Pedal_not_pushed
uint16_t GP_P;			//Gas_Pedal_pushed
uint16_t BP;			  //Brake_Pedal value 0-511
uint16_t GP;				//Gas_Pedal Value 0-511

//Values for the steering wheel
uint16_t n;
int16_t SW_N;      //steering wheel neutral position
int16_t SW_L;      //left end
int16_t SW_R;      //right end
int16_t SW_V;      //steering wheel value

char m1; //marker
char m2; //marker

void setup() {
	// put your setup code here, to run once:
  n = 511;
  m1=0;
  m2=0;
  Serial.begin(1115200); 
  pinMode(2 , INPUT_PULLUP); //Rote Knopf
  pinMode(3 , INPUT_PULLUP); //knopf unten Links
  pinMode(4 , INPUT_PULLUP); //unten rechts
  pinMode(5 , INPUT_PULLUP); //oben links
  // initialize digital pin LED_BUILTIN as an output.
	pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);//LED is conected to digital pin 13

  //Read pedal values to determine pedal start and stop position
	delay(500);			  			    //wait 0.5 seconds
  BP_NP = analogRead(A5);			//read Break_Pedal_not_pushed BP_NP
  GP_NP = analogRead(A4);			//read Gas_Pedal_not_pushed GP_NP
	SW_N  = analogRead(A3);			//read Steering_wheel_neutral SW_N
  	 
  digitalWrite(LED_BUILTIN, HIGH);//set LED to HIGH, to signal the player to press both Pedals and turn the steering wheel all the way to the left
  
  while (m1 == 0)
  {
  	if( digitalRead(3) == 0){		 //Press Button connected to Pin 9 on Arduino Board
    	BP_P = analogRead(A5);				//read Break_Pedal_pushed BP_P
     	GP_P = analogRead(A4);				//read Gas_Pedal_pushed GP_NP
    	SW_L = analogRead(A3);				//read Steering_wheel_left SW_L
      m1 = 1;
      digitalWrite(LED_BUILTIN, LOW);
      delay(500); 
      Serial.println(BP_P);
  	}
  } 
  

  while (m2 ==0)
  {
    if( digitalRead(4) == 0){    //Press Button connected to Pin 10 on Arduino Board
  	  SW_R = analogRead(A3);				//read Steering_wheel_right SW_R
  	  m2 = 1;
     digitalWrite(LED_BUILTIN, HIGH);
     delay(500);
    }
  }
	  
	//blink LED, to signal finished reading of values
  if((m1 == 1) && (m2 == 1)){
  	digitalWrite(LED_BUILTIN, HIGH); 
    delay(500);
    digitalWrite(LED_BUILTIN, LOW); 
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
  }

  //steering wheel should have the same range, wheater turned to the left or the right
  //if you can rotate further into one direction, the maximum valid rage is set to the smaller range
  if(  abs(SW_R - SW_N)  > abs(SW_N - SW_L) ){ 
      SW_R = SW_N + abs(SW_N - SW_L);
      SW_L = SW_N - abs(SW_N - SW_L);
  }
  else if(  abs(SW_R - SW_N)  < abs(SW_N - SW_L) ){
      SW_R = SW_N + abs(SW_N - SW_R);
      SW_L = SW_N - abs(SW_N - SW_R);
  }
    
	Joystick.setZAxisRange(-n, n); //drive
	Joystick.setXAxisRange(SW_L-SW_N, SW_R-SW_N); //steer 
  
	Joystick.begin();

 Serial.println(abs(BP_P - BP_NP));
}

void loop() {
  // put your main code here, to run repeatedly:

    //reads, calculates and saves the pedal position.
    //not pressed returns 0, pressed returns a value linear between 0 and n.
    //uses subT to avoid overflow
  //  BP = n-(uint32_t)n*(subT(analogRead(A5),BP_P))/(BP_NP - BP_P);
    GP = n- (n*(subT(analogRead(A4),GP_P)))/ posdif(GP_NP, GP_P);
	    BP =n-(n*(subT(analogRead(A5),BP_P)))/ posdif(BP_NP, BP_P);
    //read the position of the steering wheel
    SW_V = analogRead(A3);
 Serial.println(analogRead(A5));
 
   //Send Data 
//Serial.println(abs(BP_NP - BP_P));
    Serial.println(GP-BP);
   Joystick.setZAxis(GP-BP);
   Joystick.setXAxis((1)*(SW_V-SW_N)); // -1 if steering is inverted

   Joystick.setButton(0, !digitalRead(2) );
   Joystick.setButton(1, !digitalRead(3) );
   Joystick.setButton(2, !digitalRead(4) ); //angepasst an die Pedals des Lenkrad
   if(!digitalRead(5)){
     Keyboard.write(218); //Pfeiltaste nach oben
     delay(400);
   }
}

int16_t subT(uint16_t a, uint16_t b){
    if (BP_NP>BP_P){
      if (a>b) return a-b;
      return 0;
    }
    else{
      if (b>a) return b-a;
      return 0;
    }
}

int16_t posdif(uint16_t a, uint16_t b){
  if (a>b) return a-b;
  else return b-a;
}
