// OpenCV basic blob detection
// Last modified by Ben Wang, 11/18/2015

// Color for command Go learned: (53, 141, 63), (83, 248, 166)
// Color for command Stop learned: (0, 118, 68), (24, 248, 142)







#include "Blob.h"





// Global camera
VideoCapture cap;



// Default settings
int filtLowH = 150;
int filtHighH = 200;

int filtLowS = 150;
int filtHighS = 200;

int filtLowV = 150;
int filtHighV = 200;


// Mats
Mat frame;
Mat frame_hsv;
Mat frame_filtered;

// Functions
int learnColor(enum CommandType cmd);
inline void captureAndFilter();
inline void displayImages();


int init(){
    // Open camera
    cap = VideoCapture(0);
    // VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    // Configure camera resolution
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 240); 
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 180);
    cap.set(CV_CAP_PROP_FPS, 15);

    // Display windows
    namedWindow("Raw", WINDOW_NORMAL);
    namedWindow("HSV", WINDOW_NORMAL);
    namedWindow("Filtered", WINDOW_NORMAL);

    // Window for editing settings
    namedWindow("Settings", WINDOW_NORMAL);
    createTrackbar("Lower H", "Settings", &filtLowH, FILT_HSV_MAX, NULL);
    createTrackbar("Upper H", "Settings", &filtHighH, FILT_HSV_MAX, NULL);
    createTrackbar("Lower S", "Settings", &filtLowS, FILT_HSV_MAX, NULL);
    createTrackbar("Upper S", "Settings", &filtHighS, FILT_HSV_MAX, NULL);
    createTrackbar("Lower V", "Settings", &filtLowV, FILT_HSV_MAX, NULL);
    createTrackbar("Upper V", "Settings", &filtHighV, FILT_HSV_MAX, NULL);
    createTrackbar("Min Area", "Settings", &minArea, FILT_MIN_AREA_MAX, NULL);

    // Stage 1: Calibrate color and size
    // TODO: Variable number of colors
    // Display screen for "Set color GO"
    //learnColor(GO);


    // Display screen for "Set color STOP"
    //learnColor(STOP);
	
	commands[GO].lowerColorLimit = Scalar(58, 217, 48);
	commands[GO].upperColorLimit = Scalar(80, 255, 110);
	
	commands[STOP].lowerColorLimit = Scalar(0, 221 , 95);
	commands[STOP].upperColorLimit = Scalar(15, 255, 179);



	return 0;
}

void cam(){
    // Main loop
    // Capture frame
	cap >> frame;
	if(frame.empty())
	{
		return;
	}
		
	cvtColor(frame, frame_hsv, CV_BGR2HSV);
			
		



    // Reset frame_filtered to black
    frame_filtered = Mat::zeros(frame.size(),  CV_8U);

    // Tracker for largest are detected
    enum CommandType currMaxCommand = (CommandType) 0;
    int currMaxArea = -1;
	bool areaFound = false;

    // Iterate over each filter, add to output
    #pragma omp parallel for
    for (int fNum = 0; fNum < NUM_COMMANDS; fNum++){
		Mat currFrame = Mat(frame_filtered.size(), frame_filtered.type());

        // Perform filter
        inRange(frame_hsv, commands[fNum].lowerColorLimit, 
            commands[fNum].upperColorLimit, currFrame);

        // Get moments
        Moments frame_moments = moments(currFrame);

        // Store x and y 
        int momentArea = commands[fNum].momentArea = (int) frame_moments.m00;
        int momentX = commands[fNum].posX = (int) (frame_moments.m10 / momentArea);
        int momentY = commands[fNum].posY = (int) (frame_moments.m01 / momentArea);

        // Store max X and Y if significant
        if (momentArea > minArea && momentArea > currMaxArea){
			areaFound = true;
            currMaxArea = momentArea;
            currMaxCommand = (CommandType) fNum;
        }

        // Dye filtered frame with filter color
        // Mat currFrameBGR, currFrameColor;
        // currFrame.setTo(currFrameColor, commands[fNum].lowerColorLimit);
        // cvtColor(currFrame, currFrameBGR, CV_HSV2BGR);	

		// Draw dot and circle of command color on filtered frame
		circle(currFrame, Point(momentX, momentY), 20 /*std::sqrt(momentArea / PI)*/, /*commands[fNum].lowerColorLimit*/ Scalar(150 + 70 * fNum), 5);

        // Add filtered image
        // frame_filtered += currFrameBGR;
        frame_filtered += currFrame;
    }

	// Draw position of identified color
	if (areaFound){
		// TODO: Set global X and Y variables
		blob_currX = commands[currMaxCommand].posX;
		blob_currY = commands[currMaxCommand].posY;
		blob_currCommand = currMaxCommand;


		// TODO: Draw on original frame
		circle(frame, Point(blob_currX, blob_currY), std::sqrt(currMaxArea / PI) / 20, Scalar(0, 0, 255), 5);
			
		int fontFace = FONT_HERSHEY_COMPLEX_SMALL;
		double fontScale = 1;
		int thickness = 2;
		putText(frame, commandNames[blob_currCommand], Point(blob_currX, blob_currY), fontFace, fontScale,Scalar(0, 0, 255), thickness,8);
	}

	// Display all frames
    displayImages();

    // Exit if any key pressed
    int currKey = waitKey(30);
    if(currKey == KEY_ESC || currKey == 'q') exitLoop = true;
}





int learnColor(enum CommandType cmd){
    int currKey = -1;   // Key pressed while in loop

    // TODO: Display message on screen

    // TODO: loop for learning this color
    while (1){
        // Capture current frame
        captureAndFilter();

        // Display area on screen
        int fontFace = FONT_HERSHEY_COMPLEX_SMALL;
        double fontScale = 1;
        int thickness = 2;
        Point textOrg(0 /*frame.cols/5*/, frame.rows/1.2);
        stringstream ss;
        ss << "Select color and press Enter for cmd: " << commandNames[cmd];
        string displayText = ss.str();
        putText(frame, displayText, textOrg, fontFace, fontScale,
            /*Scalar::all(255)*/ Scalar(0, 0, 255), thickness,8);

		// Check area
		// Get moments
        Moments frame_moments = moments(frame_filtered);

        // Store x and y 
        int momentArea = frame_moments.m00;

		stringstream ss2;
        ss2 << "Area: " << momentArea << " is enough? " << (momentArea > minArea);
        displayText = ss2.str();
        putText(frame_filtered, displayText, textOrg, fontFace, fontScale, Scalar(150), thickness,8);


        // Display images
        displayImages();


        // Grab current key
        currKey = waitKey(30);
        if(currKey == KEY_ESC || currKey == KEY_ENTER || currKey == 'q' || currKey == KEY_ENTER_WINDOWS) break;
    }

    // TODO: Save color to cmd
    if (currKey == KEY_ENTER || currKey == KEY_ENTER_WINDOWS){
        // Save color to cmd
        commands[cmd].lowerColorLimit = Scalar(filtLowH, filtLowS, filtLowV);
        commands[cmd].upperColorLimit = Scalar(filtHighH, filtHighS, filtHighV);

        // Print color learned
        cout << "Color for command " << commandNames[cmd] << " learned: (" <<
            commands[cmd].lowerColorLimit[0] << ", " << commands[cmd].lowerColorLimit[1] << ", " << 
            commands[cmd].lowerColorLimit[2] << "), (" <<
            commands[cmd].upperColorLimit[0] << ", " << commands[cmd].upperColorLimit[1] << ", " << 
            commands[cmd].upperColorLimit[2] << ")" << endl;
    }

    return 0;
}


inline void captureAndFilter(){
    // Capture frame
    cap >> frame;
    cvtColor(frame, frame_hsv, CV_BGR2HSV);

    // Perform filter
    inRange(frame_hsv, Scalar(filtLowH, filtLowS, filtLowV), 
        Scalar(filtHighH, filtHighS, filtHighV), frame_filtered);
}

inline void displayImages(){
    // Display image
    imshow("Raw", frame);
   // imshow("HSV", frame_hsv);
   // imshow("Filtered", frame_filtered);
}
