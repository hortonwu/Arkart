"""
NXP 9DOF Accelerometer and Magnetometer, gyroscope
adafruit breakout box:https://learn.adafruit.com/nxp-precision-9dof-breakout/python-circuitpython

"""
import board
import busio
import adafruit_fxos8700
import adafruit_fxas21002c
import time
def Accgyro_Init():
#initalize I2C for fxos8700 and fxas21002c 
    i2c = busio.I2C(board.SCL, board.SDA)

    fxos = adafruit_fxos8700.FXOS8700(i2c)
    fxas = adafruit_fxas21002c.FXAS21002C(i2c)
    print('Acceleration (m/s^2): ({0:0.3f},{1:0.3f},{2:0.3f})'.format(*fxos.accelerometer))
    print('Magnetometer (uTesla): ({0:0.3f},{1:0.3f},{2:0.3f})'.format(*fxos.magnetometer))
    print('Gyroscope (radians/s): ({0:0.3f},{1:0.3f},{2:0.3f})'.format(*fxas.gyroscope))
#     time.sleep(1)
    return (fxos,fxas)
    
def Acc_Read(fxos):
    acc_val=fxos.accelerometer
    return acc_val

def Mag_Read(fxos):
    mag_val=fxos.magnetometer
    return mag_val

def Gyro_Read(fxas):
    gyro_val=fxas.gyroscope
    return gyro_val
             
# while True:
# 
#     print('Acceleration (m/s^2): ({0:0.3f},{1:0.3f},{2:0.3f})'.format(*fxos.accelerometer))
#     print('Magnetometer (uTesla): ({0:0.3f},{1:0.3f},{2:0.3f})'.format(*fxos.magnetometer))
#     print('Gyroscope (radians/s): ({0:0.3f},{1:0.3f},{2:0.3f})'.format(*fxas.gyroscope))
#     time.sleep(1)