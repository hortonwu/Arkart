#include <DueTimer.h>
//#include <TimerOne.h>

//input
const unsigned long PERIOD_SCALE = 50;
const int channel1Pin = 3;
unsigned long microlow = 0; 
unsigned long microhigh = 0;
unsigned long microdiff = 0;
unsigned int heartbeat = 0; 
//output
unsigned long outputCounter = 0;
unsigned long oldDutyOutput = 0;
const unsigned long outputPeriod = 17020 / PERIOD_SCALE; // *10us
const int motorPin = 13;
float dutyRequest = 0;
unsigned long heartbeatCounter = 0;
const unsigned int minHeartbeat = 2;
const unsigned long heartbeatPeriod = (minHeartbeat + 1+2) * outputPeriod;
//
unsigned long newDutyRequest = 0;
unsigned long oldDutyRequest = 0;
const unsigned long dutyTolerance = 100/ PERIOD_SCALE; 
const unsigned long DUTY_IDLE = 1500 / PERIOD_SCALE;
const int MICRO_MAX = 2100; // PERIOD_SCALE;
const int MICRO_MIN = 900; // PERIOD_SCALE;


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


//Servo
//input
const unsigned long S_PERIOD_SCALE = 50;
const int S_channel2Pin = 2;
unsigned long S_microlow = 0; 
unsigned long S_microhigh = 0;
unsigned long S_microdiff = 0;
unsigned int S_heartbeat = 0; 
//output
unsigned long S_outputCounter = 0;
unsigned long S_oldDutyOutput = 0;
const unsigned long S_outputPeriod = 17020 / S_PERIOD_SCALE; // *10us
const int S_motorPin = 12;
float S_dutyRequest = 0;
unsigned long S_heartbeatCounter = 0;
const unsigned int S_minS_heartbeat = 2;
const unsigned long S_heartbeatPeriod = (S_minS_heartbeat + 1) * S_outputPeriod;
//
unsigned long S_newS_dutyRequest = 0;
unsigned long S_oldS_dutyRequest = 0;
const unsigned long S_dutyTolerance = 100/ S_PERIOD_SCALE; 
const unsigned long S_DUTY_IDLE = 1500 / S_PERIOD_SCALE;
bool override = false;
const int overridePin = 10;
bool autoOverride = false;
bool throttleForwardOverride = false;
bool throttleReverseOverride = false;

//Ultrasonic_sensor
const int TRIG_OFFSET = 0;
const int ECHO_OFFSET = 1;
long distance;

// Base pin number for HC SR04 sensors (added with TRIG and ECHO OFFSETs above)
const int ULTRASONIC_FRONT = 54;
const int ULTRASONIC_BACK = 56;

// Distance at which automatic braking starts
const int DIST_BRAKE_FORWARD = 0;//50; // cm
const int DIST_BRAKE_REVERSE = 0;//50; // cm

// PWM pulse widths +/- PWM_DEADZONE are ignored
const int PWM_DEADZONE = 30; // us
const int PWM_FUDGE = 25; // us

bool deadman = false;

// Semaphore
const bool FREE = true;
const bool HELD = false;
bool sem_PWM = FREE;      // Semaphore to disable pwm output

// Distance average
int lastDistF[] = { 9999, 9999, 9999};
int avgDistF = 0;
int lastDistB[] = { 9999, 9999, 9999};
int avgDistB = 0;


void setup() 
{
  //Output
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);
  pinMode(overridePin, INPUT);
  pinMode(S_motorPin, OUTPUT);
  //Ultrasonic Sensor
  pinMode(TRIG_OFFSET + ULTRASONIC_FRONT, OUTPUT);
  pinMode(TRIG_OFFSET + ULTRASONIC_BACK, OUTPUT);
  pinMode(ECHO_OFFSET + ULTRASONIC_FRONT, INPUT);
  pinMode(ECHO_OFFSET + ULTRASONIC_BACK, INPUT);
  
  digitalWrite(S_motorPin, LOW);
  
  Serial.begin(115200);
  // Initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards

  for (int j = 4; j <= 8; ++j)
    pinMode(j, INPUT);

    Timer1.setPeriod(PERIOD_SCALE);
//  Timer1.initialize(PERIOD_SCALE); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
 
  
  //Input
  attachInterrupt(channel1Pin, channel1, CHANGE); //Throttle
  attachInterrupt(S_channel2Pin, channel2, CHANGE); //Steering
  delay(200);
    
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
  Serial.println("hello world!!!");
  Timer1.start();
}
 
void loop()
{
  delay(5);
//  Serial.print("us = " );
  Serial.print(S_microdiff);
  Serial.print("\t");
  Serial.print("\t");
  Serial.println(microdiff);
  Ultrasonic(ULTRASONIC_FRONT);

  // Store last 3
  lastDistF[0] = lastDistF[1];
  lastDistF[1] = lastDistF[2];
  lastDistF[2] = distance;
  
  // average
  avgDistF = (lastDistF[0] + lastDistF[1] + lastDistF[2])/3;

  // Read backwards
  Ultrasonic(ULTRASONIC_BACK);

  // Store last 3
  lastDistB[0] = lastDistB[1];
  lastDistB[1] = lastDistB[2];
  lastDistB[2] = distance;
  
  // average
  avgDistB = (lastDistB[0] + lastDistB[1] + lastDistB[2])/3;
  
  if(avgDistF < DIST_BRAKE_FORWARD)
  {
    //throttleOverride = true;
    throttleForwardOverride = true;


    Serial.print("Old duty: "); Serial.print(oldDutyOutput); Serial.print("\t");
    if (oldDutyOutput > DUTY_IDLE){
      Serial.println("*** BRAKING ***");
      brake(500);
    }

    while (distance < DIST_BRAKE_FORWARD){
      Ultrasonic(ULTRASONIC_FRONT);

      Serial.print("Front dist: ");
      Serial.println(distance);
      delay(5);
    }
  } else if(avgDistB < DIST_BRAKE_REVERSE)
  {
    //throttleOverride = true;
    throttleReverseOverride = true;


    Serial.print("Old duty: "); Serial.print(oldDutyOutput); Serial.print("\t");
//    if (oldDutyOutput > DUTY_IDLE){
//      Serial.println("*** BRAKING ***");
//      brake(500);
//    }
    setDrive(1500, 0);

    while (distance < DIST_BRAKE_REVERSE){
      Ultrasonic(ULTRASONIC_BACK);

      Serial.print("Front dist: ");
      Serial.println(distance);
      delay(5);
    }
  }
  else
  {
    throttleForwardOverride = false;  // Safe to drive forward
    throttleReverseOverride = false;  // Safe to drive reverse
    
    //Check for complete vehicle override or autonomous mode finished
    if (digitalRead (10) == HIGH || autoOverride == true){  
      override = false; 
      
    }
    // enter autonomouse mode
    else{  
      override = true;
      delay(1000);
  //    slalom();
  //    driveThreePointLeft();
  //    driveCircleLeft();
      for ( int i = 0; i < 5; i++){
        autoForward();
        autoReverse();
      }
      autoForward();
      setSteer(1000, 500);
      setSteer(2000, 500);
      setSteer(1500, 0);

      // Observe deadman release for 10 cycles
      for (int j = 0; j < 10; j++){
        Serial.print("deadman "); Serial.println(j);
        while (deadman == true) delay(10);// Serial.println(deadman);
//        delay(100);
      }
      
      //while(1);
      autoOverride = true;
      override = false;
    }
  }
  Serial.print("\t");
  Serial.print(distance);
  Serial.println(" cm");
}


// Parsing Raspi packets

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


// void serialEvent(){
// Schedule new pulse width
//  dutyRequest = Serial.parseFloat();
//  oldDutyOutput = outputPeriod * dutyRequest / 100;
//  Serial.print("Requested: "); Serial.println(dutyRequest);

// }

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
    noInterrupts();
    S_oldDutyOutput = 1000 / PERIOD_SCALE;
    interrupts();
    delay(1000);
    Serial.println("Step2");
    noInterrupts();
    oldDutyOutput = 1600 / PERIOD_SCALE;
    interrupts();
    delay(3000);
    Serial.println("Step3");

    noInterrupts();
    oldDutyOutput = DUTY_IDLE;
    interrupts();
}

void driveCircleLeft(){

Serial.println("Step1");
    noInterrupts();
    S_oldDutyOutput = 2000 / PERIOD_SCALE;
    interrupts();
    delay(1000);
    Serial.println("Step2");
    noInterrupts();
    oldDutyOutput = 1600 / PERIOD_SCALE;
    interrupts();
    delay(3000);
    Serial.println("Step3");

    noInterrupts();
    oldDutyOutput = DUTY_IDLE;
    interrupts();
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
  int lastDist[] = { 9999, 9999, 9999};
  int avg = 0;
  
  do {
    
    setDrive(1600, 0);
    Ultrasonic(ULTRASONIC_FRONT);

    // Store last 3
    lastDist[0] = lastDist[1];
    lastDist[1] = lastDist[2];
    lastDist[2] = distance;
    
    // average
    avg = (lastDist[0] + lastDist[1] + lastDist[2])/3;

    // Increment count only if less than threshold
    count = (count > COUNT_THRESH ? count : count + 1);

    Serial.print("Forward ultrasonic: "); Serial.println(distance);

    delay(5);
  } while(avg > DIST_BRAKE_FORWARD);

  // Brake only if driven forward long enough
  if (oldDutyOutput > DUTY_IDLE && count >= COUNT_THRESH)
    brake(1500);
}

void autoReverse(){
  int lastDist[] = { 9999, 9999, 9999};
  int avg = 0;
  //setSteer(1500, 0)
  setDrive(1500, 1100);
//  delay(2500);
  do {
    setDrive(1400, 0);
    Ultrasonic(ULTRASONIC_BACK);

    // Store last 3
    lastDist[0] = lastDist[1];
    lastDist[1] = lastDist[2];
    lastDist[2] = distance;
    
    // average
    avg = (lastDist[0] + lastDist[1] + lastDist[2])/3;

    Serial.print("Reverse ultrasonic: "); Serial.println(distance);

    delay(5);
  } while(avg >= DIST_BRAKE_REVERSE);
// TODO: Add brake function for reverse
  setDrive(1500, 0);
}



void setSteer(int us, int dly){
  noInterrupts();
  S_oldDutyOutput = us / PERIOD_SCALE;
  interrupts();
  delay(dly);
}

void brake(int dly){
  noInterrupts();
  oldDutyOutput = 1000 / PERIOD_SCALE;
  interrupts();
  delay(dly);
  noInterrupts();
  oldDutyOutput = 1500 / PERIOD_SCALE;
  interrupts();
}

void setDrive(int us, int dly){
  noInterrupts();
  oldDutyOutput = us / PERIOD_SCALE;
  interrupts();
  delay(dly);
}

// Read Throttle
void channel1()
{
//  // Disable interrupts
//  noInterrupts();
//
////  if (override == false ){ //&& throttleOverride == false){
//    
//    if (digitalRead(channel1Pin) == HIGH)
//    {
//        microlow = micros();
//        heartbeat++;
//    }
//    else 
//    {
//      // Grab current time, increment hearbeat
//      microhigh = micros();
//      heartbeat++;
//    
//      // Calculate new positive pulse width
//      microdiff = microhigh - microlow + PWM_FUDGE;      
//      //newDutyRequest = microdiff / PERIOD_SCALE;
//      if (microdiff > 1500 + PWM_DEADZONE){
//        newDutyRequest = ((((int)microdiff - PWM_DEADZONE) - 1500) / 2 + 1500) / PERIOD_SCALE;
//
//        // Deadman switch enable
//        deadman = true; 
//      } else if (microdiff < 1500 - PWM_DEADZONE){
//        newDutyRequest = ((((int)microdiff + PWM_DEADZONE) - 1500) / 2 + 1500) / PERIOD_SCALE;
//
//        // Deadman switch disable
//        deadman = false; 
//      } else {
//        newDutyRequest = DUTY_IDLE;
//
//        // Deadman switch disable
//        deadman = false; 
//      }
//
////      newDutyRequest = ((((int)microdiff) - 1500) / 2 + 1500) / PERIOD_SCALE;
////      newDutyRequest = ((((int)microdiff) - 1500) / (microdiff > 1500 ? 2 : 1) + 1500) / PERIOD_SCALE;
////      newDutyRequest = ((((int)microdiff) - 1500) / 1 + 1500) / PERIOD_SCALE;
//
//      // Check for directional override
//      // Don't save result if forward override and trying to go forward
//      if (override || throttleForwardOverride && newDutyRequest > DUTY_IDLE
//          || throttleReverseOverride && newDutyRequest < DUTY_IDLE){
//        interrupts(); // Reenable interrupts and bail
//        return;
//      }
//      
//      // Check for bounds 
//      if (microdiff > MICRO_MAX ||
//          microdiff < MICRO_MIN){
//        oldDutyOutput = oldDutyRequest;
//      }
//      else
//      {
//        // Update output if new request is within range from previous request
//        if (newDutyRequest >= (oldDutyRequest - dutyTolerance) &&
//            newDutyRequest <= (oldDutyRequest + dutyTolerance))
//        {
//            oldDutyOutput = oldDutyRequest;
//        }
//      
//        // Save new request as old
//        oldDutyRequest = newDutyRequest;
//      }
//    }
////  }
//  // Re-enable interrupts
//  interrupts(); 
}

//Steering
void channel2()
{
//  // Disable interrupts
//  noInterrupts();
//
//  if (override == false){
//
//    if (digitalRead(S_channel2Pin) == HIGH)
//    {
//        S_microlow = micros();
//        S_heartbeat++;
//        //Serial.println(S_microlow);
//    }
//    else 
//    {
//      // Grab current time, increment hearbeat
//      S_microhigh = micros();
//      S_heartbeat++;
//    
//      // Calculate new positive pulse width
//      S_microdiff = S_microhigh - S_microlow;
//      //S_newS_dutyRequest = S_microdiff / S_PERIOD_SCALE;
//      S_newS_dutyRequest = (((int)S_microdiff - 1500) / 1 + 1500) / S_PERIOD_SCALE;
//      
//      // Check for bounds
//      //Serial.println(S_microhigh);
//      //Serial.println(S_microdiff);
//      if (S_microdiff > MICRO_MAX ||
//          S_microdiff < MICRO_MIN){
//        S_oldDutyOutput = S_oldS_dutyRequest;
//      }
//      
//      else
//      {
//        // Update output if new request is within range from previous request
//        if (S_newS_dutyRequest >= (S_oldS_dutyRequest - S_dutyTolerance) &&
//            S_newS_dutyRequest <= (S_oldS_dutyRequest + S_dutyTolerance))
//        {
//            S_oldDutyOutput = S_oldS_dutyRequest;
//        }
//      
//        // Save new request as old
//        S_oldS_dutyRequest = S_newS_dutyRequest;
//      }
//    }
//  }
//  // Re-enable interrupts
//  interrupts();
  
}

void timerIsr()
{
    int currDutyOutput; // current output duty cycle
  
    noInterrupts();

    // Don't do anything if in autonmous mode and deadman == false
    if (override && deadman == false){
      currDutyOutput = DUTY_IDLE;
    } else {
      currDutyOutput = oldDutyOutput;
    }

    // Check for turn off
    if (outputCounter >= currDutyOutput){
      digitalWrite(motorPin, LOW);
      //Serial.println(oldDutyOutput);

      sem_PWM = FREE;   // Free semaphore
    }
    
    // Check for turn on/reset
    if (outputCounter >= outputPeriod)
    {
      if (sem_PWM == HELD){
        interrupts();
        return;
      }
      sem_PWM = HELD;   // Grab semaphore
      digitalWrite(motorPin, HIGH);
      outputCounter = 0;
    }

    
    // Increment counter
    
    outputCounter++;
//    if (override == false){
//      heartbeatCounter++;
//      if (heartbeatCounter >= heartbeatPeriod){
//        if (heartbeat < (minHeartbeat * 2)){
//           oldDutyOutput = oldDutyRequest = DUTY_IDLE;// / 50;
//           digitalWrite(motorPin, LOW);
//           Serial.print("E");
//        }
//        heartbeatCounter = 0;
//        heartbeat = 0;
//      }
//    }
    if (S_outputCounter >= S_outputPeriod)
    {
      digitalWrite(S_motorPin, HIGH);
      S_outputCounter = 0;
    }

    // Check for turn off
    if (S_outputCounter == S_oldDutyOutput)
      digitalWrite(S_motorPin, LOW);

    // Increment counter
    
    S_outputCounter++;
//    if (override == false){
//      S_heartbeatCounter++;
//      if (S_heartbeatCounter >= S_heartbeatPeriod){
//        if (S_heartbeat < (S_minS_heartbeat * 2)){
//           S_oldDutyOutput = S_oldS_dutyRequest = S_DUTY_IDLE;// / 50;
//           digitalWrite(S_motorPin, LOW);
//           Serial.print("S");
//        }
//        S_heartbeatCounter = 0;
//        S_heartbeat = 0;
//      }
//    }
    interrupts();
}

void Ultrasonic(const int basePin) {
  long duration;
  while (sem_PWM == HELD) Serial.println(sem_PWM);
  noInterrupts();
  sem_PWM = HELD;
  interrupts();
  
//  noInterrupts();
  digitalWrite(TRIG_OFFSET + basePin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(TRIG_OFFSET + basePin, HIGH);
//  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(12); // Added this line
  digitalWrite(TRIG_OFFSET + basePin, LOW);
  duration = pulseIn(ECHO_OFFSET + basePin, HIGH);
  distance = (duration/2) / 29.1;

  noInterrupts();
  sem_PWM = FREE;
  interrupts();
}


// Handle Raspi commands

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
        if (oldDutyOutput >= 1750) // some arbitrary number for now
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


