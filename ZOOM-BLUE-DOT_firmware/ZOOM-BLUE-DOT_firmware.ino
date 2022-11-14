/*
 * Robot Control Firmware for Bull.Miletic's
 * ZOOM BLUE DOT Installation
 * 
 * Developed by Kuan-Ju Wu & Chris Myers
 * https://wukuanju.com
 * Nov 14 2022 
 * 
*/

//Includes required to use Roboclaw library
#include <SoftwareSerial.h>
#include "RoboClaw.h"

//See limitations of Arduino SoftwareSerial
SoftwareSerial serial(10,11);  
RoboClaw roboclaw(&serial,10000);

#define address 0x80
#define defaultSpeed 32;

int forwardSpeed = 32; 
int backwardSpeed= 32; 
int turnSpeed =32;

//0:A | 1:A->B | 2:B | 3:B->A
#define stationA 0
#define AtoB 1
#define stationB 2
#define BtoA 3

#define Idle 0
#define ForwardOnly 1
#define ForwardAndBack 2

int whereAmI = AtoB; 

//Sensor Pins, "Back"is where the wheels are
#define FLsensor 2
#define FRsensor 3
#define BLsensor 5
#define BRsensor 4

//Wall Detection IR Sensor Pins
#define FwallSensor 8
#define BwallSensor 7

//Reset Button
#define resetBtnPin 12
#define resetBtnLed A4

#define ForwardOnlyButton 6
#define ForwardAndBackButton 9

#define manualFWD A0
#define manualBWD A1
#define speedDial A2
#define buzzer A3

int lastManualMode;



long lastResetTime;

int mode; // 0: idle, 1:forward only, 2:forward+backward
int resetMode = 0; // 0: untriggered, 1: blinking, 2:solid

long stopTimer = 3000;
long stopAndWaitTiggeredTime;

float LSpeed;
float RSpeed;

int speedAdj;


void setup() {

  pinMode(FLsensor, INPUT_PULLUP);
  pinMode(FRsensor, INPUT_PULLUP);
  pinMode(BLsensor, INPUT_PULLUP);
  pinMode(BRsensor, INPUT_PULLUP);
  pinMode(FwallSensor, INPUT_PULLUP);
  pinMode(BwallSensor, INPUT_PULLUP);
  pinMode(resetBtnPin, INPUT_PULLUP);
  pinMode(ForwardOnlyButton, INPUT_PULLUP);
  pinMode(ForwardAndBackButton, INPUT_PULLUP);
  pinMode(manualFWD, INPUT_PULLUP);
  pinMode(manualBWD, INPUT_PULLUP);
  pinMode(speedDial, INPUT_PULLUP);
  pinMode(buzzer, INPUT_PULLUP);


  
  pinMode(resetBtnLed, OUTPUT);

  Serial.begin(9600);
  
  //Open roboclaw serial ports
  roboclaw.begin(38400);

  stopAndWaitTiggeredTime = millis();
  lastResetTime = millis();
  while(millis()-stopAndWaitTiggeredTime<stopTimer){
    sharpStopMotors();
    Serial.println("wait");
  }

}

void loop() {
//  Serial.print("lspd: ");
//  Serial.print(LSpeed);
//  Serial.print(" rspd: ");
//  Serial.println(RSpeed);
   
  speedAdj = constrain(map(analogRead(speedDial),0,1024,-32,32),-32,32);
  forwardSpeed = defaultSpeed+speedAdj;
  backwardSpeed= defaultSpeed+speedAdj; 
  turnSpeed = defaultSpeed+speedAdj;

  if(!digitalRead(ForwardOnlyButton)){
    mode = ForwardOnly;
   }else if(!digitalRead(ForwardAndBackButton)){

    mode =  ForwardAndBack;
   }else{
    mode = Idle;
   }


    
//  mode = ForwardOnly;

  switch(mode){  
    //=============idle mode=============//  
    case Idle:
      resetMode = 0;
      if(!digitalRead(manualFWD)){
            Serial.println("manual FWD");
            lastManualMode = 1;

        goForward(forwardSpeed);
      }else if(!digitalRead(manualBWD)){
            Serial.println("manual BWD");
            lastManualMode = -1;

        goBackward(backwardSpeed);
      }else{
        sharpStopMotors();
      }
       
      digitalWrite(resetBtnLed, LOW);
      digitalWrite(buzzer, LOW);


    break;
    //=============forward only mode=============//  
    case ForwardOnly: 
      if(!digitalRead(FwallSensor)){     //if hit wall,the robot halts, the buzzer sounds, and the reset button starts to blink
        Serial.println("hit wall");
        digitalWrite(buzzer, HIGH);
        if(millis()%1000>500){
          digitalWrite(resetBtnLed, HIGH);
        }else{
          digitalWrite(resetBtnLed, LOW);

        }

        sharpStopMotors();
        resetMode = 1;
      }else{
        switch(resetMode){
          case 0:
            Serial.println("moving forward");
            digitalWrite(buzzer, LOW);
            digitalWrite(resetBtnLed, LOW);

            followLineForward(); // if robotClaw is not connected, this step will cause a delay after the reset button is pressed
            if(!digitalRead(resetBtnPin) && (millis()-lastResetTime>1000)){
              Serial.println("reset button pressed");
              lastResetTime = millis();
              sharpStopMotors();
              resetMode = 2;
             }

          break;
          
          case 1:
            sharpStopMotors();

            if(!digitalRead(resetBtnPin) && (millis()-lastResetTime>1000)){
              lastResetTime = millis();
              Serial.println("blinking stop, reset to moving");
              digitalWrite(buzzer, LOW);
               digitalWrite(resetBtnLed, LOW);

              resetMode = 0;
            }else{
              Serial.println("hit wall, halts, buzz, blink");
               digitalWrite(buzzer, HIGH);

              sharpStopMotors();

              if(millis()%1000>500){
                digitalWrite(resetBtnLed, HIGH);
              }else{
                digitalWrite(resetBtnLed, LOW);
              }

            }
          break;

          case 2:
          
            sharpStopMotors();
            if(!digitalRead(resetBtnPin) && millis()-lastResetTime>1000){
              lastResetTime = millis();
              Serial.println("solid lighht  stop, reset to moving");
              
              digitalWrite(resetBtnLed, LOW);
              digitalWrite(buzzer, LOW);

              resetMode = 0;
            }else{
              Serial.println("solid light, stop");
              digitalWrite(resetBtnLed, HIGH);
              digitalWrite(buzzer, LOW);

            }
          break;
        }
      }
    break;
    //=============back and forth mode=============//  
    case ForwardAndBack: 
      digitalWrite(buzzer, LOW);
      Serial.println(resetMode);
      if(resetMode == 2){
        if(!digitalRead(resetBtnPin) && millis()-lastResetTime>1000){
          lastResetTime = millis();
          Serial.println("reset button pressed again to resume moving");
          digitalWrite(resetBtnLed, LOW);
            resetMode = 0;
          }else{
            sharpStopMotors();
            Serial.println("solid light");
            digitalWrite(resetBtnLed, HIGH);
          }
      }else if(resetMode == 0){
        if(!digitalRead(resetBtnPin) && millis()-lastResetTime>1000){
          lastResetTime = millis();
          Serial.println("reset button pressed");
          sharpStopMotors();
          resetMode = 2;
        }else{
          switch(whereAmI){
            /*******************/   
            case stationA:
              Serial.println("Arrived Station A");
              if(millis()-stopAndWaitTiggeredTime<stopTimer){
//                stopMotors(-1);
            sharpStopMotors();

              }else{
                whereAmI = AtoB;
              }
                
            break; 
            /*******************/
            case AtoB:
              Serial.println("moving A to B");
              if(!digitalRead(FwallSensor)){     //if hit wall,change Mode to stationB
                whereAmI = stationB;
                stopAndWaitTiggeredTime = millis();
              }else{
                followLineForward();
              }
            break;
            /*******************/
            case stationB:
              Serial.println("Arrived Station B");
        
              if(millis()-stopAndWaitTiggeredTime<stopTimer){
//                stopMotors(1);
            sharpStopMotors();

              }else{
                whereAmI = BtoA;
              }
              
            break;
            /*******************/
            case BtoA:
              Serial.println("moving B to A");

              if(!digitalRead(BwallSensor)){      //if hit wall,change Mode to stationA
                whereAmI = stationA;
                stopAndWaitTiggeredTime = millis();
              }else{
                followLineBackward();  
              } 
            break; 
          }
        }
      
      }else{ // other non defined resetMode

      }
     break;
  }

}


void followLineForward(){
  //check line sensors
  boolean Fleft = digitalRead(FLsensor);
  boolean Fright = digitalRead(FRsensor);
  boolean Bleft = digitalRead(BLsensor);
  boolean Bright = digitalRead(BRsensor);
  
 if((!Fleft&&!Fright)){
    Serial.println("both sensors triggered, stop!");
//    stopMotors(1);
            sharpStopMotors();

  }else if(!Fleft ){
    Serial.println("left triggered, turn forward left!");
    turnForwardLeft(turnSpeed); //stop left motor  
  }else if(!Fright ){
    Serial.println("right triggered, turn forward right!");
    turnForwardRight(turnSpeed); //stop right motor  
  }else{
    goForward(forwardSpeed);
  }
}

void followLineBackward(){
  //check line sensors
  boolean Fleft = digitalRead(FLsensor);
  boolean Fright = digitalRead(FRsensor);
  boolean Bleft = digitalRead(BLsensor);
  boolean Bright = digitalRead(BRsensor);
  
 if(!Bleft&&!Bright){ 
    Serial.println("both sensor triggered, stop!");
//    stopMotors(-1);
            sharpStopMotors();

  }else if(!Bleft){
    Serial.println("left triggered, turn back left!");
    turnBackwardLeft(turnSpeed); //stop left motor  
  }else if(!Bright){
    Serial.println("right triggered, turn back right!");
    turnBackwardRight(turnSpeed); //stop right motor  
  }else{
    goBackward(backwardSpeed);
  }
}
