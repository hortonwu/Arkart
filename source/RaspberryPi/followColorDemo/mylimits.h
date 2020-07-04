#ifndef __MYLIMITS_H__
#define __MYLIMITS_H__

// Limits type
template<class type>
struct Limit{
	type max;
	type min;
	
	Limit(){
		max = 0;
		min = 0;
	}
	Limit(type limit_min, type limit_max)
		: min(limit_min), max(limit_max) {}
};

// Driving command limits (decision_making.cpp)
const Limit<int> SteerLim(  -20,	20	);
const Limit<int> BrakeLim(    0,	100	);
const Limit<int> DriveLim( -100,	100	);


// Limit for braking since last paddle seen
const int MAX_COUNT_SINCE_LAST_PADDLE = 3;


#endif