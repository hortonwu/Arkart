#include <DueTimer.h> //get from https://github.com/ivanseidel/DueTimer
#include "Semaphore.h"
#include "Ultrasonic.h"
#include "PwmChannel.h"
//#include <TimerOne.h>

//Throttle
const int THROTTLE_IN_PIN = 3;
const int MOTOR_PIN = 13;
//Steering
const int STEERING_IN_PIN = 2;
const int S_MOTOR_PIN = 12;
const int ULTRASONIC_FRONT = 54;
const int ULTRASONIC_BACK = 56;

const int DIST_BRAKE_FORWARD = 50; // cm
const int DIST_BRAKE_REVERSE = 50; // cm

const int OVERRIDE_PIN = 10;
bool autoOverride = false;

// PWM pulse widths +/- PWM_DEADZONE are ignored
const int PWM_DEADZONE = 30; // us
const int PWM_FUDGE = 25; // us

PwmChannel throttleChannel = PwmChannel(THROTTLE_IN_PIN, MOTOR_PIN, true, PWM_FUDGE, PWM_DEADZONE, 2, true);
PwmChannel steeringChannel = PwmChannel(STEERING_IN_PIN, S_MOTOR_PIN, false, 0, 0, 1, false);

// Raspi - Arduino Serial Commands
const char SOP = '{';
const char EOP = '}';
char PacketType = 0;
char IncomingByteTmp = 0;
char Arry[10];
int i = 0;
int j = 0;
int PacketLength = 0;
int value = 0;
// ----

// Semaphore
Semaphore sem_PWM = FREE;      // Semaphore to disable pwm output

Ultrasonic frontUltrasonic = Ultrasonic(ULTRASONIC_FRONT, sem_PWM);
Ultrasonic backUltrasonic = Ultrasonic(ULTRASONIC_BACK, sem_PWM);

void setup() 
{
  Serial.begin(115200);

  pinMode(OVERRIDE_PIN, INPUT);

//Ultrasonic setup use to be
  
  // Initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards

  for (int j = 4; j <= 8; ++j)
    pinMode(j, INPUT);

    
//  Timer1.initialize(PERIOD_SCALE); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
//  throttleChannel = PwmChannel(THROTTLE_IN_PIN, MOTOR_PIN, true, PWM_FUDGE, PWM_DEADZONE, 2, true);
//  steeringChannel = PwmChannel(STEERING_IN_PIN, S_MOTOR_PIN, false, 0, 0, 1, false);
//  PwmChannel.setSemaphore(sem_PWM);     // Set semaphore for PWM/Ultrasonic


  
//  //Input
  attachInterrupt(THROTTLE_IN_PIN, throttleChannel.channelISR_arr[throttleChannel.getChannelNum()], CHANGE); //Throttle
  attachInterrupt(STEERING_IN_PIN, steeringChannel.channelISR_arr[steeringChannel.getChannelNum()], CHANGE); //Steering

  PwmChannel::initAndSetSemaphore(sem_PWM);
  delay(200);
  
  // FOR SERIAL COMMAND MODE, enable autonomous to disable deadman
  PwmChannel::override_completeAutonomous = true;
    
  Serial.print("Hello World");
}
 
void loop()
{
  delay(5);
//  Serial.print("us = " );
  Serial.print(steeringChannel.getMicroDiff());
  Serial.print("\t");
  Serial.print("\t");
  Serial.println(throttleChannel.getMicroDiff());
 
  // Read average ultrasonic distance for automatic braking
  int avgDistF = frontUltrasonic.getAverageDistance();
  int avgDistB = backUltrasonic.getAverageDistance();
  
  if(avgDistF < DIST_BRAKE_FORWARD)
  {
    //throttleOverride = true;
    PwmChannel::override_throttleForward = true;


    Serial.print("Old duty: "); Serial.print(throttleChannel.getDutyOutput()); Serial.print("\t");
    if (throttleChannel.getDutyOutput() > PwmChannel::DUTY_IDLE){
      Serial.println("*** BRAKING ***");
      brake(500);
    }
    int distance;
    do {
      delay(5);
      Serial.print("Front dist: ");
      Serial.println(frontUltrasonic.readDistance());
    }while(frontUltrasonic.getDistance() < DIST_BRAKE_FORWARD);
  } else if(avgDistB < DIST_BRAKE_REVERSE)
  {
    //throttleOverride = true;
    PwmChannel::override_throttleReverse = true;

    Serial.print("Old duty: "); Serial.print(throttleChannel.getDutyOutput()); Serial.print("\t");
    setDrive(1500, 0);

    do {
      delay(5);
      Serial.print("Reverse dist: ");
      Serial.println(backUltrasonic.readDistance());
    }while(backUltrasonic.getDistance() < DIST_BRAKE_REVERSE);
  }
  
  else
  {
    PwmChannel::override_throttleForward = false;  // Safe to drive forward
    PwmChannel::override_throttleReverse = false;  // Safe to drive reverse
    
    //Check for complete vehicle override or autonomous mode finished
//    if (digitalRead (10) == HIGH || autoOverride == true){  
//      PwmChannel::override_completeAutonomous = false;
//      
//    }
//    // enter autonomouse mode
//    else{  
//      PwmChannel::override_completeAutonomous = true;
//      delay(1000);
//  //    slalom();
//  //    driveThreePointLeft();
//  //    driveCircleLeft();
//      for ( int i = 0; i < 5; i++){
//        autoForward();
//        autoReverse();
//      }
//      autoForward();
//      setSteer(1000, 500);
//      setSteer(2000, 500);
//      setSteer(1500, 0);
//
//      // Observe deadman release for 10 cycles
//      for (int j = 0; j < 10; j++){
//        Serial.print("deadman "); Serial.println(j);
//        while (PwmChannel::deadman == true) delay(10);// Serial.println(PwmChannel.deadman);
////        delay(100);
//      }
      
      //while(1);
      autoOverride = true;
//      PwmChannel::override_completeAutonomous = false;
//    }
  }
  Serial.print("loop: front=");  Serial.print(frontUltrasonic.getDistance());Serial.print(" cm\tback=");Serial.print(backUltrasonic.getDistance());Serial.println(" cm");
}

void serialEvent(){
  while (Serial.available() > 0)
  {
    IncomingByteTmp = Serial.read();
     
    switch(IncomingByteTmp) {
      case SOP:
        i = 0;
        Serial.println("SOP found");
        break;
       
      case EOP:
        PacketType = Arry[0];
        PacketLength = i;
        Arry[i] = '\0';
         
        value = atoi(Arry + 1);
        Serial.print("Packet type: ");
        Serial.println(PacketType);
        Serial.println(value);

        driveHandler(PacketType, value);
        
        break; 
       
      default:
        Arry[i] = IncomingByteTmp;
        i = i + 1; 
        Serial.print("Char found");
        Serial.println(Arry[i-1]);
         
    } 
  }
}

//void serialEvent(){
// Schedule new pulse width
//  dutyRequest = Serial.parseFloat();
//  oldDutyOutput = outputPeriod * dutyRequest / 100;
//  Serial.print("Requested: "); Serial.println(dutyRequest);
//}

/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
//void up()
//{
//  noInterrupts();
//  microlow = micros();
//  heartbeat++;
//  interrupts();
//}

// ISR for negative edge on PWM input

void driveCircleRight(){

Serial.println("Step1");
    steeringChannel.setDuty(1000);
    delay(1000);
    Serial.println("Step2");
    throttleChannel.setDuty(1600);
    delay(3000);
    Serial.println("Step3");

    throttleChannel.setDuty(1500);
}

void driveCircleLeft(){

Serial.println("Step1");
    steeringChannel.setDuty(2000);
    delay(1000);
    Serial.println("Step2");
    throttleChannel.setDuty(1600);
    delay(3000);
    Serial.println("Step3");
    throttleChannel.setDuty(1500);
}



void driveThreePointLeft(){
  // Forward, Back-Left, Forward-Right
    setSteer(1500, 100);
    setDrive(1600, 1000);
    setDrive(1000, 500);  // brake half second
    setDrive(1500, 1500); // idle
    setSteer(2000, 1100);
    setDrive(1400, 750 * 4 / 3 * 2.6);
    setDrive(1500, 0);
    setSteer(1000, 100);
    setDrive(1600, 750 * 1.2);
    setDrive(1500, 0);
    setSteer(1500, 0);
}

void slalom(){
    setDrive(1600, 0);
    setSteer(1000, 3000 / 7);   //RIGHT
    setSteer(2000, 750);  //LEFT
    setSteer(1500, 80);
    setSteer(1000, 750);  //RIGHT
    setSteer(1500, 80);
    setSteer(2000, 750);  //LEFT
    setSteer(1500, 80);
    setSteer(1000, 750);  //RIGHT
    setSteer(2000, 3000 / 9);  //LEFT
    setDrive(1500, 0);
    setSteer(1500, 0);
}

void autoForward(){
  //setSteer(1500, 0)
  unsigned long count = 0;
  const int COUNT_THRESH = 60;   // Threshold in cycles for braking to be enabled
  do {
    
    setDrive(1600, 0);
   
    // Increment count only if less than threshold
    count = (count > COUNT_THRESH ? count : count + 1);

   Serial.print("Forward ultrasonic: "); //Serial.println(distance);

    delay(5);
  } while(frontUltrasonic.getAverageDistance() > DIST_BRAKE_FORWARD);

  // Brake only if driven forward long enough
  if (throttleChannel.getDutyOutput() > PwmChannel::DUTY_IDLE && count >= COUNT_THRESH)
    brake(1500);
}

void autoReverse(){
  //setSteer(1500, 0)
  setDrive(1500, 1100);
//  delay(2500);
  do {
    setDrive(1400, 0);
    Serial.print("Reverse ultrasonic: ");//Serial.println(distance);

    delay(5);
  } while(  backUltrasonic.getAverageDistance() >= DIST_BRAKE_REVERSE);
// TODO: Add brake function for reverse
  setDrive(1500, 0);
}



void setSteer(int us, int dly){
  steeringChannel.setDuty(us);
  delay(dly);
}

void brake (int dly){
  throttleChannel.setDuty(1000);
  delay(dly);
  throttleChannel.setDuty(1500);
}

void setDrive(int us, int dly){
  throttleChannel.setDuty(us);
  delay(dly);
}


void driveHandler(char packetType, int value){
  switch(packetType){
    case 'F': // Forward
      Serial.print("Drive F: "); Serial.println(value);
      setDrive(1500 + value * 5 , 0);                       // Drive range: [ -100, 100 ]
      break;
    case 'B':
      Serial.print("Brake: "); Serial.println(value);
      if (value > 0){             // Demo brake function. not best implementation
        // Then brake                                       // Brake range: [    0, 100 ]
        if (throttleChannel.getDutyOutput() >= 1750) // some arbitrary number for now
          brake(500);             // "brake" if forward enough
        else
          setDrive(1500, 0);      // Idle
      }
      break;
    case 'S':
      Serial.print("Steer: "); Serial.println(value);
      setSteer(1500 + (500/30) * value , 0);                // Steer range: [  -30,  30 ]
      break;
    default:
      Serial.print("Error: Unrecognized command: "); Serial.println(packetType);
  }
  
}




