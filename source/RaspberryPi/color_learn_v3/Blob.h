#ifndef BLOB_H_INCLUDED
#define BLOB_H_INCLUDED

#include "opencv2/opencv.hpp"
#include <omp.h>

#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;



// Enum to name commands
// TODO: add type for no command
enum CommandType{GO, STOP};                         // Enum of commands


// Global coordinate of moments
int blob_currX;
int blob_currY;

// Global current color detected
enum CommandType blob_currCommand;

const int FILT_HSV_MAX = 255;
const int FILT_MIN_AREA_MAX = 1000000;


// Key codes
const int KEY_ESC   = 27;
const int KEY_ENTER = 10;
const int KEY_ENTER_WINDOWS = 13;

// Constants
const double PI = 3.14159;



// Structs for learning color
string commandNames[] = {"Go", "Stop"};  // Command name strings
const int NUM_COMMANDS = 2;                         // Number of commands

class Command{
public:
    // Color limits
    Scalar lowerColorLimit;
    Scalar upperColorLimit;

    // Coordinates
    int posX;
    int posY;
	int momentArea;

    // Area
    int filteredArea;

    Command(){
        lowerColorLimit = Scalar(Scalar::all(0));
        upperColorLimit = Scalar(Scalar::all(255));
    }
private:

};

Command commands[NUM_COMMANDS]; // Array of Comman structs

// Learn colors
//commands[GO].lowerColorLimit = Scalar(58, 217, 48);
//commands[GO].upperColorLimit = Scalar(80, 255, 110);

//commands[STOP].lowerColorLimit = Scalar(0, 221 , 95);
//commands[STOP].upperColorLimit = Scalar(15, 255, 179);

// Min area to qualify as detected area
int minArea = 300000;

// Continue running?
bool exitLoop = false;

int init();

void cam();


#endif