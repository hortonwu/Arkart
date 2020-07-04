#include <stdio.h>
#include <math.h>
#include "decision_making.h"
#include "blob.h"
#include "mylimits.h"


//Outputs
//steer, drive and brake
//steer and drive ranges [-100 100], brake [0 100] 
int CntlCom[3] = 	{0,0,0};
#define IDX_STEER	0	// Range: [ -20 ?, 20 ? ]	degrees
#define IDX_DRIVE	1	// Range: [ -100, 100 ]		%
#define IDX_BRAKE	2	// Range: [ 0, 100 ]		%

//0 nothing, 1 tracking, 2 braking
#define STATE_UNINIT	0
#define STATE_TRACKING	1
#define STATE_BRAKING	2
unsigned int CntlState = STATE_UNINIT;

//CAL parameters
//Targets
int X_Target = 0; //steer factor
int R_Target = 30000; //distance factor




// PID controller for one channel
template<class type>
class PID{
	Limit<type> antiwindupLim;
	type err_old;
	type err_I;
	type err_D;
	type* K;
	
public:
	// Constructor for PID object
	PID(type* inK, type antiwindupMin, type antiwindupMax)
		: K(inK) {
		// Initializations
		err_old = 0;
		err_I = 0;
		err_D = 0;
		
		antiwindupLim = Limit<type>(antiwindupMin, antiwindupMax);
	}
	
	// Do PID control given desired value, feedback value, and time step
	type control(type desired, type feedback, float dt){
	//Could add piecewise non-linear terms in the future
		type err = desired - feedback;						// Calculate error on this step
		err_I = err_I + err;								// Integrate error
		saturate(err_I, antiwindupLim);						// Anti-Windup
		
		err_D = 0.2*err_D + 0.8*(err - err_old);			//Low pass filter the derivative term
		//err_D = (err - err_old);

		err_old = err;										// Store this error as previous

		return (K[0]*err + K[1]*err_I*dt + K[2]*err_D/dt);	// return PID
	}
};


//steer
//PID
float K_x[3] = {2,0.2,0.5};
PID<float> xPID = PID<float>(K_x, -1000, 1000);

//drive
//PID
float K_r[3] = {2,0.2,0.5};
PID<float> rPID = PID<float>(K_r, -1000, 1000);




// Count since last paddle seen
int countSinceLastPaddle = 0;

float Pos[3] = {0,0,0};
float Pos_Old[3] = {0,0,0};



// Function declarations
void trackingLogic(void);												// Main function for proccessing camera data
template<class type> void saturate(type& value, Limit<type> limits);		// Saturates value to provided limits
bool txCommand(int command, int value);							// Sends the character command followed by value in packet


void trackingLogic(void){//10ms


	switch(blob_currCommand){
		case STOP:
			CntlCom[IDX_BRAKE] = BrakeLim.max; //brake, only takes Positive
			CntlCom[IDX_DRIVE] = 0;
			CntlCom[IDX_STEER] = 0; 
			CntlState = STATE_BRAKING;//Braking

			
			// Send brake command over serial
			txCommand(IDX_BRAKE, CntlCom[IDX_BRAKE]);
			txCommand(IDX_DRIVE, CntlCom[IDX_DRIVE]);
			txCommand(IDX_STEER, CntlCom[IDX_STEER]);
			
			// Reset counter
			countSinceLastPaddle = 0;
			break;
			
		case GO:
			CntlCom[IDX_BRAKE] = BrakeLim.min;		// Disable braking

			//Steer + saturate limits
			CntlCom[IDX_STEER] = xPID.control(X_Target, blob_currX, DT);
			saturate(CntlCom[IDX_STEER], SteerLim);	//Left and Right

			//Drive + saturate limits
			CntlCom[IDX_DRIVE] = rPID.control(R_Target, blob_currArea, DT);
			saturate(CntlCom[IDX_DRIVE], DriveLim);	//Forward and Reverse

			Pos_Old[0] = Pos[0];
			Pos_Old[1] = Pos[1];
			Pos_Old[2] = Pos[2];

			CntlState = STATE_TRACKING;//Tracking or Searching
			
			// Send commands over serial
			txCommand(IDX_BRAKE, CntlCom[IDX_BRAKE]);
			txCommand(IDX_DRIVE, CntlCom[IDX_DRIVE]);
			txCommand(IDX_STEER, CntlCom[IDX_STEER]);
			
			// Reset counter
			countSinceLastPaddle = 0;
			break;
			
		default:	//color others (no command)
			
			//TODO: If no command found, should stop after a certain # of cycles.
			if(++countSinceLastPaddle > MAX_COUNT_SINCE_LAST_PADDLE){
				// Brake the car, we've lost the paddle!
				CntlCom[IDX_DRIVE] = 0;
				CntlCom[IDX_BRAKE] = BrakeLim.max; //brake, only takes Positive
				cerr << "Lost track of paddle for " << countSinceLastPaddle << " frames" << endl;
			}

	/* 		if (STATE_TRACKING == CntlState){//Were in tracking, fall into searching
				// TODO: Just steering? No driving?
				if(Pos_Old[0]>0){
					CntlCom[IDX_STEER] = 30;//Steer Positive to Search
					CntlCom[IDX_DRIVE] = 0;//Do not drive
				}
				else{
					CntlCom[IDX_STEER] = -30;//Steer Negative to Search
					CntlCom[IDX_DRIVE] = 0;//Do not drive
				}
			}
			else{//Were in stop mode, fall into initiliazed
				CntlState = STATE_UNINIT;
			
			} */
			
			
			// Send commands over serial
			txCommand(IDX_BRAKE, CntlCom[IDX_BRAKE]);
			txCommand(IDX_DRIVE, CntlCom[IDX_DRIVE]);
//			txCommand(IDX_STEER, CntlCom[IDX_STEER]);
			
	}
}

template<class type>
void saturate(type& value, Limit<type> limits){
	if (value > limits.max){
		value = limits.max;
	} else if(value < limits.min){
		value = limits.min;
	}
}


bool txCommand(int command, int value){
	

	char commandChar;

	switch (command){
		case IDX_BRAKE:
			commandChar = 'B';
			break;
		case IDX_DRIVE:
			commandChar = 'D';
			break;
		case IDX_STEER:
			commandChar = 'S';
			break;
		default:
			commandChar = '?';
	}

	cout << "SENDING CMD: " << '{' << commandChar << value << '}' << endl;
	try {
		ofstream fout (serialPath.c_str());
		fout << '{' << commandChar << value << "}\n";
		fout.close();
	} catch (...) {
		return false;
	}
	return true;
}

