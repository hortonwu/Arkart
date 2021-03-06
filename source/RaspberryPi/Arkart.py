import sys
sys.path.insert(0, "../opcuapy")

import constant
import servos16ch
import distanceHCSR
import time
import opcua_server
import accgyro9dof
import cameraPi
# from datetime import datetime

server=opcua_server.server_setup()

# constant configurations 
front_s = constant.FRONT_STEER
rear_s = constant.REAR_STEER
zero_s = constant.ZERO_SPD
test_s = constant.TEST_SPD
dead_l = constant.DEAD_L
dead_h = constant.DEAD_H

rear_a = constant.REAR_A
front_rw = constant.FRONT_RW
front_lw = constant.FRONT_LW
#camera rotation
rotation =270
directory_pic = '/home/pi/Desktop/Arkart_image.jpg'
directory_video = '/home/pi/Desktop/Arkart_video.h264'

#front servo position 105 degree
#clockwise circle foward
steer_fr = 56
steer_rl = 124
#clockwise circle reverse
steer_fl = 124 # left >90 55 is the min, right <90
steer_rr = 56

#target motor speed
target_sf= 100
target_sr= 87
brake_sf=95

obstacle_dist = 40
if __name__ == '__main__':
    try:
#         val_n = 0
#         val_n1 = 0
#         val_n2 =0
        # switch 0 and 1 to for one time reset
        switch = True
        # previous step velocity 
        velocity_n1= 0.0
        #motor drive duration
        time_Tf = 0.1
        time_Tr = 0.5
        time_Tbf = 0.25
        time_Tbr = 0.05
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
        camera = cameraPi.CameraInit(rotation)
#         camera.annotate_text = "Hello Jing, The Arkart is circuling, Enjoy!!!-{0:%Y}-{0:%m}-{0:%d}".format(datetime.now())
        cameraPi.Video(True,directory_video,camera)
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
            
            
            servos16ch.Senvo_Pos_Set(front_s, steer_fr)
            servos16ch.Senvo_Pos_Set(rear_s, steer_rl)
            spd_n1_ra = servos16ch.Motor_Spd_Set(rear_a,spd_n1_ra,target_sf,dead_l,dead_h)
            acc_n1=accgyro9dof.Acc_Read(fxos_fxas[0])
            
            time.sleep(time_Tf)        

            acc_n=accgyro9dof.Acc_Read(fxos_fxas[0])
            velocity_n = velocity_n1 + (acc_n[0]+acc_n1[0])*time_Tf/2.0 
            velocity_n1 = velocity_n
            acc_n1=acc_n
            
            while (velocity_n < -0.4) or (acc_n[0]<-1.2): #(acc[0] < -0.4) or (acc[1] < -0.4):
                spd_n1_ra = servos16ch.Motor_Spd_Set(rear_a,spd_n1_ra,brake_sf,dead_l,dead_h)
                print("Forward too fast, Brake! Acc_X: %0.3f, Acc_Y:%0.3f velocity:%0.3f" % (acc_n[0],acc_n[1],velocity_n))
                time.sleep(time_Tbf)
                acc_n=accgyro9dof.Acc_Read(fxos_fxas[0])
                velocity_n = velocity_n1 + (acc_n[0]+acc_n1[0])*time_Tbf/2 
                velocity_n1 = velocity_n
                acc_n1=acc_n

            
            #spd_n1_fr = servos16ch.Motor_Spd_Set(front_rw,spd_n1_fr,target_sf,dead_l,dead_h)
            #spd_n1_fl = servos16ch.Motor_Spd_Set(front_lw,spd_n1_fl,target_sf,dead_l,dead_h)
#             time.sleep(0.5)
            
            dist = distanceHCSR.distance()
            #movingAverage(dist,val_n1,val_n2,val_n)
#           print ("Measured Distance = %.1f cm" % dist)
            
            while dist < obstacle_dist:
                
                servos16ch.Senvo_Pos_Set(front_s, steer_fl)
                servos16ch.Senvo_Pos_Set(rear_s, steer_rr)
          
                spd_n1_ra = servos16ch.Motor_Spd_Set(rear_a,spd_n1_ra,target_sr,dead_l,dead_h)
    #             spd_n1_fr = servos16ch.Motor_Spd_Set(front_rw,spd_n1_fr,target_sr,dead_l,dead_h)
    #             spd_n1_fl = servos16ch.Motor_Spd_Set(front_lw,spd_n1_fl,target_sr,dead_l,dead_h)
#                 acc_n1=accgyro9dof.Acc_Read(fxos_fxas[0])
            
                time.sleep(time_Tr)        

                acc_n=accgyro9dof.Acc_Read(fxos_fxas[0])
                velocity_n = velocity_n1 + (acc_n[0]+acc_n1[0])*time_Tr*0.5 
                velocity_n1 = velocity_n
                acc_n1=acc_n
                
                while velocity_n > 0.8 :#(acc[0] < -0.4) or (acc[1] < -0.4):
                    spd_n1_ra = servos16ch.Motor_Spd_Set(rear_a,spd_n1_ra,zero_s,dead_l,dead_h)
                    print("Reverse too fast, Brake! Acc_X: %0.3f, velocity:%0.3f" % (acc_n[0],velocity_n))
                    time.sleep(time_Tbr)
                    acc_n=accgyro9dof.Acc_Read(fxos_fxas[0])
                    velocity_n = velocity_n1 + (acc_n[0]+acc_n1[0])*time_Tbr*0.5
                    velocity_n1 = velocity_n
                    acc_n1=acc_n
                    if acc_n1[0]<0.5:
                        velocity_n1 = 0.0
                  
                dist = distanceHCSR.distance()
                print ("Measured Distance = %.1f cm" % dist)
                acc_n1=(0.0,0.0,0.0)
                if switch is True:
                    switch = False
                    cameraPi.Video(False,directory_video,camera)
    
                
 
        # Reset by pressing CTRL + C
    except KeyboardInterrupt:
        print("Measurement stopped by User")
        GPIO.cleanup()
