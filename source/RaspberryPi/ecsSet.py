from adafruit_servokit import ServoKit
import time
import constant

kit = ServoKit(channels=16)


kit.servo[6].angle = 90
kit.servo[0].angle = 90
time.sleep(10)
kit.servo[6].angle = 120
kit.servo[0].angle = 120
time.sleep(10)
kit.servo[6].angle = 60
kit.servo[0].angle = 60
time.sleep(10)
