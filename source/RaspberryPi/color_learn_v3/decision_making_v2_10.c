#include <stdio.h>
#include <math.h>
#include "decision_making.h"
#include "Blob.h"
//include OpenCV libraries so that we could get Pos and RGB and compile


//global variables




// Global coordinate of moments
/*---------------------------------- int blob_currX; -------------------------------*/
/*---------------------------------- int blob_currY; -------------------------------*/
/*-----------------------------------int blob_currA; -------------------------------*/ 
//Areas

// Global current color detected
/*---------------------------------- enum CommandType blob_currCommand; --------------------------*/


//enum CommandType{GO, STOP};   




//Outputs
//steer, drive and brake
//steer and drive ranges [-100 100], brake [0 100] 
float CntlCom[3] = 	{0,0,0};

// States
#define STATE_UNINIT	0
#define STATE_TRACKING	1
#define STATE_BRAKING	2
//0 nothing, 1 tracking, 2 braking
unsigned int CntlState = STATE_UNINIT;


//CAL parameters
//Targets
int X_Target = 0; //steer factor
int R_Target = 5; //distance factor

//steer
//PID
float K_x[3] = {2,0.2,0.5};

//drive
//PID
float K_r[3] = {2,0.2,0.5};


void CamSigPro(void){

//read color and position from OpenCV


}



float Cntrl_PID_1(int x_desr, int x_fb, float dt, float *K){

//PID controller for one channel
//Could add piecewise non-linear terms in the future

	static float err_old = 0;
	static float err_I = 0;
	static float err_D = 0;
	float err = 0;


	err = x_desr - x_fb;

	err_I = err_I + err;

	//Anti-Windup
	if (err_I > 1000){
		err_I = 1000;
	}
	else if(err_I<-1000){
		err_I = -1000;
	}
	
	//Low pass filter the derivative term
	err_D = 0.2*err_D + 0.8*(err - err_old);
	//err_D = (err - err_old);

	err_old = err;

	return (K[0]*err + K[1]*err_I*dt + K[2]*err_D/dt) ;


}

float Cntrl_PID_2(int x_desr, int x_fb, float dt, float *K){

//PID controller for one channel
//Could add piecewise non-linear terms in the future

	static float err_old = 0;
	static float err_I = 0;
	static float err_D = 0;
	float err = 0;


	err = x_desr - x_fb;

	err_I = err_I + err;

	//Anti-Windup
	if (err_I > 1000){
		err_I = 1000;
	}
	else if(err_I<-1000){
		err_I = -1000;
	}
	
	//Low pass filter the derivative term
	err_D = 0.2*err_D + 0.8*(err - err_old);
	//err_D = (err - err_old);

	err_old = err;

	return (K[0]*err + K[1]*err_I*dt + K[2]*err_D/dt) ;


}



void CamTracking(void){//10ms

	static float Pos_Old[3] = {0,0,0};

	if( blob_currCommand == STOP){//color red
		CntlCom[2] = 100; //brake, only takes Positive
		CntlCom[0] = 0;
		CntlCom[1] = 0; 
		CntlState = STATE_BRAKING;//Braking

	}
	else if( blob_currCommand == GO){//color green

		CntlCom[2] = 0;

		//Steer
		CntlCom[0] = Cntrl_PID_1(X_Target,blob_currX,0.01,K_x);

		if (CntlCom[0] > 100){//Left and Right
			CntlCom[0] = 100;
		}
		else if(CntlCom[0] < -100){
			CntlCom[0] = -100;
		}

		//Drive
		CntlCom[1] = Cntrl_PID_2(R_Target,blob_currA,0.01,K_r);

		if (CntlCom[1] > 100){//Forward and Reverse
			CntlCom[1] = 100;
		}
		else if(CntlCom[1] < -100){
			CntlCom[1] = -100;
		}

		Pos_Old[0] = Pos[0];
		Pos_Old[1] = Pos[1];
		Pos_Old[2] = Pos[2];

		CntlState = STATE_TRACKING;//Tracking or Searching
	}
	else{//color others (no command)
		
		//TODO: If no command found, should stop after a certain # of cycles.

		if (STATE_TRACKING == CntlState){//Were in tracking, fall into searching
			// TODO: Just steering? No driving?
			if(Pos_Old[0]>0){
				CntlCom[0] = 30;//Steer Positive to Search
				CntlCom[1] = 0;//Do not drive
			}
			else{
				CntlCom[0] = -30;//Steer Negative to Search
				CntlCom[1] = 0;//Do not drive
			}
		}
		else{//Were in stop mode, fall into initiliazed
			CntlState = STATE_UNINIT;
		
		}

	}

}