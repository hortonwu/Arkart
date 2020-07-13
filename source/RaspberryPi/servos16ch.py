from adafruit_servokit import ServoKit
import time
import constant

kit = ServoKit(channels=16)


def Motor_Spd_Init(ch,zero_spd,test_spd):
    """ initialze Brushless Motor ECS HobbyKing -35A"""
    
    kit.servo[ch].angle = zero_spd
    time.sleep(1)
    kit.servo[ch].angle = test_spd
    time.sleep(2)
    kit.servo[ch].angle = zero_spd
    time.sleep(1)
    print("devices is successfuly initialized")
    
    
def Motor_Spd_Set(ch,spd_n1,target_spd,dead_l,dead_h):
    # check whether motor spin in different direction if yes, then set to zero and brake for 2 seconds at least 
    # global spd_n1
    if ((spd_n1<dead_l) and (target_spd > dead_h)) or ((spd_n1>dead_h) and (target_spd < dead_l)):
         
        kit.servo[ch].angle = 94
        time.sleep(4)
        kit.servo[ch].angle = target_spd
    else:
#         kit.servo[ch].angle = 94
#         time.sleep(3)
        kit.servo[ch].angle = target_spd
    spd_n1 = target_spd
    return spd_n1

def Senvo_Pos_Set(ch,target_pos):
    # set servo angular postion          
    kit.servo[ch].angle = target_pos

   
    
# #front Right Driving Motor
# kit.servo[5].angle = 91
# time.sleep(2)
# kit.servo[5].angle = 88
# time.sleep(3)
# kit.servo[5].angle = 91
# 
# #Front Left Driving Motor 
# kit.servo[4].angle = 91
# time.sleep(3)
# kit.servo[4].angle = 88
# time.sleep(3)
# kit.servo[4].angle = 91
# 
# 
# #rear wheels ro all wheels
# kit.servo[6].angle = 0
# time.sleep(3)
# kit.servo[6].angle = 91
# time.sleep(2)
# kit.servo[6].angle = 88
# time.sleep(5)
# kit.servo[6].angle = 91

# 
# 
# while True:
#     #Front Steering Servo 
# #     kit.servo[0].angle = 120
# #     time.sleep(3)
# #     kit.servo[0].angle = 60
# #     time.sleep(3)
#     
#     #Rear Left Driving Motor 
#     kit.servo[1].angle = 120
#     time.sleep(3)
#     kit.servo[1].angle = 90
#     time.sleep(3)
#     
#     #front Right Driving Motor 
#     kit.servo[5].angle = 90
#     
#     #Front Left Driving Motor 
#     kit.servo[4].angle = 88
#     
#     kit.servo[6].angle = 88
    
    #     i=i+1
    #     if i > 100:
    #         i = 100
    #     time.sleep(3)
    #     print("i:", i)
#     time.sleep(1)
#     kit.servo[5].angle = 89
#     time.sleep(1)
#     kit.servo[5].angle = 91
