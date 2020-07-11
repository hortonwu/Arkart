"""
Distance Sensor Tutorial 
https://tutorials-raspberrypi.com/raspberry-pi-ultrasonic-sensor-hc-sr04/
"""
 
#Libraries
import RPi.GPIO as GPIO
import time
 
#GPIO Mode (BOARD / BCM)
GPIO.setmode(GPIO.BCM)
 
#set GPIO Pins
GPIO_TRIGGER = 23
GPIO_ECHO = 24
GPIO.setwarnings(False)

#set GPIO direction (IN / OUT)
GPIO.setup(GPIO_TRIGGER, GPIO.OUT)
GPIO.setup(GPIO_ECHO, GPIO.IN)

#set global variable for moving average
val_n = 0
val_n1 = 0
val_n2 =0

def distance():
    # set Trigger to HIGH
    GPIO.output(GPIO_TRIGGER, True)
 
    # set Trigger after 0.01ms to LOW
    time.sleep(0.00001)
    GPIO.output(GPIO_TRIGGER, False)
 
    StartTime = time.time()
    StopTime = time.time()
 
    # save StartTime
    while GPIO.input(GPIO_ECHO) == 0:
        StartTime = time.time()
 
    # save time of arrival
    while GPIO.input(GPIO_ECHO) == 1:
        StopTime = time.time()
 
    # time difference between start and arrival
    TimeElapsed = StopTime - StartTime
    # multiply with the sonic speed (34300 cm/s)
    # and divide by 2, because there and back
    distance = (TimeElapsed * 34300) / 2
 
    return distance

def movingAverage(arg,val_n1,val_n2,val_n):
    """
    moving average for last 3 time series value
    """
    val_n2 = val_n1
    val_n1 = val_n
    val_n=arg
    val = (val_n2 + val_n1 + val_n)/3
    return val

     
# if __name__ == '__main__':
#     try:
#         val_n = 0
#         val_n1 = 0
#         val_n2 =0
#         while True:
#             dist = distance()
#             #movingAverage(dist,val_n1,val_n2,val_n)
#             print ("Measured Distance = %.1f cm" % dist)
#             time.sleep(1)
#  
#         # Reset by pressing CTRL + C
#     except KeyboardInterrupt:
#         print("Measurement stopped by User")
#         GPIO.cleanup()
