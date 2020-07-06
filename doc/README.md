Autonomus Arkart Project
===============================

This is the official repository of the hardware team on the AVL Autonomous Vehicles Project.

Primary contact for this project is Yang, Hanlong [EXP] AVL/US <hanlong.yang@avl.com>



Previous development:
---------------------
11/30/2015: PWM_INOUT_ULTRASONIC16.2
	Incorporated RasPi/Arduino Serial Packet interface. DISABLED remote control with comments. TESTING ONLY.
	Ultrasonic sensors NOT workind.

11/30/2015: PWM_INOUT_ULTRASONIC16.1
	Incorporated RasPi/Arduino Serial Packet interface. Remote control is still enabled and overwrites the serial command.
	Ultrasonic sensors NOT workind.



09/02/2015: PWM_INOUT_ULTRASONIC16.0
	Presently newest code for autonomous car with DUE ECU.



08/21/2015: PWM_INOUT_ULTRASONIC15.3
	Presently newest code for autonomous car wtih Uno ECU. Code needs to be consolidated.
	Ultrasonic forward/reverse detection, collision avoidance/auto braking. Arduino Uno ECU.
	Implemented semaphore.


	Definitions & Notes:
	----------------------
	- Channel 1: Throttle
	- Channel 2: Steering
		- These are opposite of the RC channels used on the car. Should be switched in next version
	- "S_" is for steering. Variables/functions with the S are for steering.
	- Duplicate code from the above notes should be abstractted into a library in next version.


	Hardware pin assignments:
	---------------------------

	Pin	Connection		Use
	------	----------		------
	D0	RX/TX			Serial
	D1	RX/TX			Serial
	D2	channel1Pin		Input for Channel 1 edge detection (throttle pulse width in)
	D3	channel2Pin		Input for Channel 2 edge detection (steering pulse width in)
	D4	Encoder Pin		Input for tractive motor encoder
	D5	Encoder Pin		Input for tractive motor encoder
	D6	Encoder Pin		Input for tractive motor encoder
	D7	Encoder Pin		Input for tractive motor encoder
	D8	Encoder Pin		Input for tractive motor encoder
	D9	
	D10	
	D11	
	D12	S_motorPin		Output for steering servo PWM
	D13	motorPin		Output for motor/inverter PWM
	A1/D14	ULTRASONIC_FRONT TRIG	Output Trigger pin for front ultrasonic sensor
	A2/D15	ULTRASONIC_FRONT ECHO	Output Echo pin for front ultrasonic sensor
	A3/D16	ULTRASONIC_BACK TRIG	Output Trigger pin for rear ultrasonic sensor
	A4/D17	ULTRASONIC_BACK ECHO	Output Echo pin for rear ultrasonic sensor
	A5/D18	

	Bugs:
	-------
	*** channel1Pin and channel2Pin are incorrect! One should be pin 2 and the other pin 3



08/05/2015: motorPWM
	Tool for RC PWM output on Arduino Uno.
	Will output desired pulse width on specified pin, based on serial input.


08/05/2015: motorPWM2.0
	Tool for RC PWM output on Arduino Uno + Encoder reading tool.
	Will output desired pulse width on specified pin, based on serial input.
	Also, reads encoder on pins [4, 8] and outputs values via serial.


07/29/2015: PWM_INOUT
	Tool for reading RC PWM from receiver and prints pulse width in (us) over serial.


08/06/2015 to
08/19/2015: PWM_INOUT_COMBO series
	Combining both channels of RC input and output. Superceded by PWM_INOUT_ULTRASONIC


07/24/2015: RCCarPWM
	Superceded by PWM_INOUT

07/28/2015: RCCarPWM/PWMout
	Early version of trying to do the RC PWM output to control motor using hardware PWM.
	Unsuccessful. Hardware PWM doesn't provide enough resolution for motor control.

08/13/2015: PWM_INOUT_SONAR_COMBO10.0
	Initial integration of sonar sensor with car. Basic collision avoidance.

