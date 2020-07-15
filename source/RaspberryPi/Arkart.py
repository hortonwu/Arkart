import sys
sys.path.insert(0, "../opcuapy")

import constant
import servos16ch
import distanceHCSR
import time
import opcua_server
import accgyro9dof

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
test_pl = 90
test_pr = 90
#target motor speed
target_sf= 100
target_sr= 88

if __name__ == '__main__':
    try:
#         val_n = 0
#         val_n1 = 0
#         val_n2 =0
        # previous step velocity 
        velocity_n1= 0
        #motor drive duration
        time_T = 0.2
        time_Tb = 0.4
        #rear axle previous speed
        spd_n1_ra = target_sf
        #rear axle previous speed
        spd_n1_fl = zero_s
        #rear axle previous speed
        spd_n1_fr = zero_s
        ###Initialization Devices 
        #front steering 
        servos16ch.Motor_Spd_Init(front_s, zero_s, test_s)
        servos16ch.Motor_Spd_Init(rear_s, zero_s, test_s)
        #rear axel wheels 
        servos16ch.Motor_Spd_Init(rear_a, zero_s, test_s)
#         servos16ch.Motor_Spd_Init(front_rw, zero_s, test_s)
#         servos16ch.Motor_Spd_Init(front_lw, zero_s, test_s)
        #ACC and GYRO
        fxos_fxas=accgyro9dof.Accgyro_Init()
        while True:
            dist = distanceHCSR.distance()
            #movingAverage(dist,val_n1,val_n2,val_n)
            print ("Measured Distance = %.1f cm" % dist)
            print('Acceleration (m/s^2): ({0:0.3f},{1:0.3f},{2:0.3f})'.format(*accgyro9dof.Acc_Read(fxos_fxas[0])))
#             print('Magnetometer (uTesla): ({0:0.3f},{1:0.3f},{2:0.3f})'.format(*accgyro9dof.Mag_Read(fxos_fxas[0])))
            print('Gyroscope (radians/s): ({0:0.3f},{1:0.3f},{2:0.3f})'.format(*accgyro9dof.Gyro_Read(fxos_fxas[1])))
#             time.sleep(1)
#             time.sleep(3)

#             opcua_server.reg_namespace(server,dist)
            
            
            servos16ch.Senvo_Pos_Set(front_s, test_pl)
            servos16ch.Senvo_Pos_Set(rear_s, test_pl)
            spd_n1_ra = servos16ch.Motor_Spd_Set(rear_a,spd_n1_ra,target_sf,dead_l,dead_h)
            acc_n1=accgyro9dof.Acc_Read(fxos_fxas[0])
            
            time.sleep(time_T)        

            acc_n=accgyro9dof.Acc_Read(fxos_fxas[0])
            velocity_n = velocity_n1 + (acc_n[0]+acc_n1[0])*time_T/2 
            velocity_n1 = velocity_n
            acc_n1=acc_n
            
            while velocity_n < -0.4: #(acc[0] < -0.4) or (acc[1] < -0.4):
                spd_n1_ra = servos16ch.Motor_Spd_Set(rear_a,spd_n1_ra,zero_s,dead_l,dead_h)
                print("Forward too fast, Brake! Acc_X: %0.3f, velocity:%0.3f" % (acc_n[0],velocity_n))
                time.sleep(time_Tb)
                acc_n=accgyro9dof.Acc_Read(fxos_fxas[0])
                velocity_n = velocity_n1 + (acc_n[0]+acc_n1[0])*time_Tb/2 
                velocity_n1 = velocity_n
                acc_n1=acc_n
                if acc_n1[0] < 0.3:
                    velocity_n1=0
            
            #spd_n1_fr = servos16ch.Motor_Spd_Set(front_rw,spd_n1_fr,target_sf,dead_l,dead_h)
            #spd_n1_fl = servos16ch.Motor_Spd_Set(front_lw,spd_n1_fl,target_sf,dead_l,dead_h)
#             time.sleep(0.5)
            acc_n1 = 0
            dist = distanceHCSR.distance()
            #movingAverage(dist,val_n1,val_n2,val_n)
#           print ("Measured Distance = %.1f cm" % dist)
            
            while dist <80:
                
                servos16ch.Senvo_Pos_Set(front_s, test_pr)
                servos16ch.Senvo_Pos_Set(rear_s, test_pr)
          
                spd_n1_ra = servos16ch.Motor_Spd_Set(rear_a,spd_n1_ra,target_sr,dead_l,dead_h)
    #             spd_n1_fr = servos16ch.Motor_Spd_Set(front_rw,spd_n1_fr,target_sr,dead_l,dead_h)
    #             spd_n1_fl = servos16ch.Motor_Spd_Set(front_lw,spd_n1_fl,target_sr,dead_l,dead_h)
                acc_n1=accgyro9dof.Acc_Read(fxos_fxas[0])
            
                time.sleep(time_T)        

                acc_n=accgyro9dof.Acc_Read(fxos_fxas[0])
                velocity_n = velocity_n1 + (acc_n[0]+acc_n1[0])*time_T/2 
                velocity_n1 = velocity_n
                acc_n1=acc_n
                
                while velocity_n > 0.6 :#(acc[0] < -0.4) or (acc[1] < -0.4):
                    spd_n1_ra = servos16ch.Motor_Spd_Set(rear_a,spd_n1_ra,zero_s,dead_l,dead_h)
                    print("Reverse too fast, Brake! Acc_X: %0.3f, velocity:%0.3f" % (acc_n[0],velocity_n))
                    time.sleep(time_Tb)
                    acc_n=accgyro9dof.Acc_Read(fxos_fxas[0])
                    velocity_n = velocity_n1 + (acc_n[0]+acc_n1[0])*time_Tb/2 
                    velocity_n1 = velocity_n
                    acc_n1=acc_n
                  
                dist = distanceHCSR.distance()
                print ("Measured Distance = %.1f cm" % dist)
                acc_n1=0
    
                
 
        # Reset by pressing CTRL + C
    except KeyboardInterrupt:
        print("Measurement stopped by User")
        GPIO.cleanup()
