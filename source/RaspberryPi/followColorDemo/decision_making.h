#ifndef DECISION_MAKING_H_INCLUDED
#define DECISION_MAKING_H_INCLUDED

#include <string>
#include <fstream>
using namespace std;

//extern float Pos[3];
//extern unsigned long RGB[3];

extern int blob_currX;
extern int blob_currY;
extern unsigned long blob_currArea;

extern void CamTracking(void);

const string serialPath = "/dev/ttyACM0";  //"/dev/ttyAMA0";

#endif
