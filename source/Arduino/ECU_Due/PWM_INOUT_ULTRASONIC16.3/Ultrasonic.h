#ifndef _ULTRASONIC_H_
#define _ULTRASONIC_H_

//Ultrasonic_sensor
class Ultrasonic{
private:
    const int TRIG_OFFSET = 0;
    const int ECHO_OFFSET = 1;
    long distance;
    int lastDist[3] = { 9999, 9999, 9999};
// Base pin number for HC SR04 sensors (added with TRIG and ECHO OFFSETs above)
    int ultrasonicPin;
    

// Distance at which automatic braking starts
     int brakeDist;

    Semaphore sem_PWM;
public: 
    Ultrasonic(int pin, Semaphore &sem_PWM_in);
    int readDistance();
    int getPin();
    int getDistance();
    int getAverageDistance();
};


//Constructor
 Ultrasonic::Ultrasonic(int pin, Semaphore &sem_PWM_in){
  
   ultrasonicPin = pin;
   sem_PWM = sem_PWM_in;
  
   pinMode(TRIG_OFFSET + pin, OUTPUT);
   pinMode(ECHO_OFFSET + pin, INPUT);
 
 }

//Main run method from original program
  int Ultrasonic::readDistance() {
  long duration;
  while (sem_PWM == HELD) Serial.println(sem_PWM);
  noInterrupts();
  sem_PWM = HELD;
  interrupts();
  
//  noInterrupts();
  digitalWrite(TRIG_OFFSET + ultrasonicPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(TRIG_OFFSET + ultrasonicPin, HIGH);
//  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(12); // Added this line
  digitalWrite(TRIG_OFFSET + ultrasonicPin, LOW);
  duration = pulseIn(ECHO_OFFSET + ultrasonicPin, HIGH);
  distance = (duration/2) / 29.1;

  noInterrupts();
  sem_PWM = FREE;
  interrupts();
}

//Returns ultrasonics pin number
int Ultrasonic::getPin(){
  return ultrasonicPin;
}

int Ultrasonic::getDistance(){
  return distance;
}

int Ultrasonic::getAverageDistance(){
  
    // Store last 3
    lastDist[0] = lastDist[1];
    lastDist[1] = lastDist[2];
    lastDist[2] = readDistance();
    
    // average
  return (lastDist[0] + lastDist[1] + lastDist[2])/3;
    
}

#endif
