#ifndef BLOB_H_INCLUDED
#define BLOB_H_INCLUDED

#include "opencv2/opencv.hpp"
#include "omp.h"

#include <iostream>
#include <cmath>

#include "mylimits.h"

using namespace cv;
using namespace std;



// Enum to name commands
// TODO: add type for no command
enum CommandType{GO, STOP, NO_COMMAND}; // Enum of commands


// Frame rate / loop time
const int FPS = 12;						// Global camera frame rate
const double DT = 1.0 / FPS;			// Global loop time (for decision_making.cpp)

// Picture size
const int CAPTURE_FRAME_WIDTH = 100; //240;
const int CAPTURE_FRAME_HEIGHT = 75; //180;

// Global coordinate of moments
int blob_currX;							// Global current X-coord, based on camera window
int blob_currY;							// Global current Y-coord, based on camera window
unsigned long blob_currArea;			// Global current command's area
enum CommandType blob_currCommand;		// Global current color detected

const int FILT_HSV_MAX = 255;
const int FILT_MIN_AREA_MAX = 1000000;
const int FILT_MORPH_ELLIPSE_MAX = 100;


// Key codes
const int KEY_ESC   = 27;
const int KEY_ENTER = 10;
const int KEY_ENTER_WINDOWS = 13;

// Constants
const double PI = 3.14159;



// Structs for learning color
string commandNames[] = {"Go", "Stop", "No command"};	// Command name strings
const int NUM_COMMANDS = 2;								// Number of commands

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


// Continue running?
bool exitLoop = false;

int init();

void cam();


#endif
