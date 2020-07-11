import constant
import servos16ch
import distanceHCSR
import time

front_s =constant.FRONT_STEER
zero_s = constant.ZERO_SPD
test_s = constant.TEST_SPD
dead_l = constant.DEAD_L
dead_h = constant.DEAD_H

rear_a = constant.REAR_A

#front servo position 105 degree
test_p = 105

#target motor speed
target_s= 87

if __name__ == '__main__':
    try:
        val_n = 0
        val_n1 = 0
        val_n2 =0
        while True:
            dist = distanceHCSR.distance()
            #movingAverage(dist,val_n1,val_n2,val_n)
            print ("Measured Distance = %.1f cm" % dist)
            time.sleep(3)
            #front steering 
            servos16ch.Motor_Spd_Init(front_s, zero_s, test_s)
            #rear axel wheels 
            servos16ch.Motor_Spd_Init(rear_a, zero_s, test_s)
            
            
            servos16ch.Senvo_Pos_Set(front_s, test_p)
            time.sleep(2)
            servos16ch.Motor_Spd_Set(rear_a,target_s,dead_l,dead_h)
            time.sleep(5)
 
        # Reset by pressing CTRL + C
    except KeyboardInterrupt:
        print("Measurement stopped by User")
        GPIO.cleanup()
