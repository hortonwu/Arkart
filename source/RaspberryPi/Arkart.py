import constant
import servos16ch
import distanceHCSR
import time

front_s =constant.FRONT_STEER
zero_s = constant.ZERO_SPD
test_s = constant.TEST_SPD

rear_a = constant.REAR_A

if __name__ == '__main__':
    try:
        val_n = 0
        val_n1 = 0
        val_n2 =0
        while True:
            dist = distanceHCSR.distance()
            #movingAverage(dist,val_n1,val_n2,val_n)
            print ("Measured Distance = %.1f cm" % dist)
            time.sleep(1)
            #front steering 
            servos16ch.Motor_Spd_Init(front_s, zero_s, test_s)
            #rear axel wheels 
            servos16ch.Motor_Spd_Init(front_s, zero_s, test_s)
 
        # Reset by pressing CTRL + C
    except KeyboardInterrupt:
        print("Measurement stopped by User")
        GPIO.cleanup()
