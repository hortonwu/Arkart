import sys
sys.path.insert(0, "../opcuapy")

import constant
import servos16ch
import distanceHCSR
import time
import opcua_server

server=opcua_server.server_setup()

front_s = constant.FRONT_STEER
rear_s = constant.REAR_STEER
zero_s = constant.ZERO_SPD
test_s = constant.TEST_SPD
dead_l = constant.DEAD_L
dead_h = constant.DEAD_H

rear_a = constant.REAR_A
front_rw = constant.FRONT_RW
front_lw = constant.FRONT_LW

#front servo position 105 degree
test_pl = 110
test_pr = 70
#target motor speed
target_sf= 99
target_sr= 86

if __name__ == '__main__':
    try:
        val_n = 0
        val_n1 = 0
        val_n2 =0
        #rear axle previous speed
        spd_n1_ra = zero_s
        #rear axle previous speed
        spd_n1_fl = zero_s
        #rear axle previous speed
        spd_n1_fr = zero_s
        #front steering 
        servos16ch.Motor_Spd_Init(front_s, zero_s, test_s)
        servos16ch.Motor_Spd_Init(rear_s, zero_s, test_s)
        #rear axel wheels 
        servos16ch.Motor_Spd_Init(rear_a, zero_s, test_s)
#         servos16ch.Motor_Spd_Init(front_rw, zero_s, test_s)
#         servos16ch.Motor_Spd_Init(front_lw, zero_s, test_s)
        while True:
#             dist = distanceHCSR.distance()
#             #movingAverage(dist,val_n1,val_n2,val_n)
#             print ("Measured Distance = %.1f cm" % dist)
#             time.sleep(3)

            opcua_server.reg_namespace(server,test_pl)
            
            
            servos16ch.Senvo_Pos_Set(front_s, test_pl)
            servos16ch.Senvo_Pos_Set(rear_s, test_pl)
            time.sleep(2)
            
         
            spd_n1_ra = servos16ch.Motor_Spd_Set(rear_a,spd_n1_ra,target_sf,dead_l,dead_h)
            #spd_n1_fr = servos16ch.Motor_Spd_Set(front_rw,spd_n1_fr,target_sf,dead_l,dead_h)
            #spd_n1_fl = servos16ch.Motor_Spd_Set(front_lw,spd_n1_fl,target_sf,dead_l,dead_h)
            time.sleep(3)

            servos16ch.Senvo_Pos_Set(front_s, test_pr)
            servos16ch.Senvo_Pos_Set(rear_s, test_pr)
      
            spd_n1_ra = servos16ch.Motor_Spd_Set(rear_a,spd_n1_ra,target_sr,dead_l,dead_h)
#             spd_n1_fr = servos16ch.Motor_Spd_Set(front_rw,spd_n1_fr,target_sr,dead_l,dead_h)
#             spd_n1_fl = servos16ch.Motor_Spd_Set(front_lw,spd_n1_fl,target_sr,dead_l,dead_h)
           
            time.sleep(3)
            
            
 
        # Reset by pressing CTRL + C
    except KeyboardInterrupt:
        print("Measurement stopped by User")
        GPIO.cleanup()
