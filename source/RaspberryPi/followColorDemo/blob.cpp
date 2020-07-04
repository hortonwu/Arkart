// OpenCV basic blob detection
// Last modified by Ben Wang, 12/02/2015

// Color for command Go learned: (25, 150, 150), (75, 255, 255)
// Color for command Stop learned: (220, 150, 150), (35, 255, 255)







#include "blob.h"





// Global camera
VideoCapture cap;



// Default settings
int filtLowH = 25;
int filtHighH = 75;

int filtLowS = 150;
int filtHighS = 255;

int filtLowV = 150;
int filtHighV = 255;

int minArea = 10000;//300000;		// Min area to qualify as detected area

int morphEllipse = 2;

// Mats
Mat frame;
Mat frame_hsv;
Mat frame_filtered;

// Functions
int learnColor(enum CommandType cmd);
inline void captureAndFilter();
inline void displayImages();
inline void filterFrame(Mat& inputFrame, int lowH, int lowS, int lowV, int highH, int highS, int highV, Mat& outputFrame);
inline int scaleCartesianPercent(int coordinate, bool isX);

int init(){
    // Open camera
    cap = VideoCapture(0);
    // VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

	cout << "Successfully opened camera" << endl;

    // Configure camera resolution & frame rate
    cap.set(CV_CAP_PROP_FRAME_WIDTH, CAPTURE_FRAME_WIDTH); 
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAPTURE_FRAME_HEIGHT);
    cap.set(CV_CAP_PROP_FPS, 15);

	cout << "camera resolution set" << endl;

    // Display windows
    namedWindow("Raw", WINDOW_NORMAL);
    namedWindow("HSV", WINDOW_NORMAL);
    namedWindow("Filtered", WINDOW_NORMAL);

	cout << "displayed windows" << endl;

    // Window for editing settings
    namedWindow("Settings", WINDOW_NORMAL);
    createTrackbar("Lower H", "Settings", &filtLowH, FILT_HSV_MAX, NULL);
    createTrackbar("Upper H", "Settings", &filtHighH, FILT_HSV_MAX, NULL);
    createTrackbar("Lower S", "Settings", &filtLowS, FILT_HSV_MAX, NULL);
    createTrackbar("Upper S", "Settings", &filtHighS, FILT_HSV_MAX, NULL);
    createTrackbar("Lower V", "Settings", &filtLowV, FILT_HSV_MAX, NULL);
    createTrackbar("Upper V", "Settings", &filtHighV, FILT_HSV_MAX, NULL);
    createTrackbar("Min Area", "Settings", &minArea, FILT_MIN_AREA_MAX, NULL);
    createTrackbar("Morphological ellipse size", "Settings", &morphEllipse, FILT_MORPH_ELLIPSE_MAX, NULL);

	cout << "created trackbars" << endl;

    // Stage 1: Calibrate color and size
    // TODO: Variable number of colors
    
	
	// Display screen for "Set color GO"
    //learnColor(GO);
	commands[GO].lowerColorLimit = Scalar(25, 150, 150);
	commands[GO].upperColorLimit = Scalar(75, 255, 255);

	// Display screen for "Set color STOP"
    //learnColor(STOP);
	commands[STOP].lowerColorLimit = Scalar(220, 150, 150);
	commands[STOP].upperColorLimit = Scalar(35, 255, 255);

	cap >> frame;

	cout << "init completed successfully" << endl;
	return 0;
}

void cam(){
    // Main loop
    // Capture frame
	cap >> frame;
	cvtColor(frame, frame_hsv, CV_BGR2HSV);

    // Reset frame_filtered to black
    frame_filtered = Mat::zeros(frame.size(),  CV_8U);

	// Tracker for largest are detected
	enum CommandType currMaxCommand = NO_COMMAND;
	int currMaxArea = 0;
	bool areaFound = false;

    // Iterate over each filter, add to output
//    omp_set_num_threads(4);
//    #pragma omp parallel for
    for (int fNum = 0; fNum < NUM_COMMANDS; fNum++){
		Mat currFrame = Mat(frame_filtered.size(), frame_filtered.type());

		// Perform filter & morphological opening
		//filterFrame(frame_hsv, commands[fNum].lowerColorLimit[0], commands[fNum].lowerColorLimit[1], commands[fNum].lowerColorLimit[2],
		//	commands[fNum].upperColorLimit[0], commands[fNum].upperColorLimit[1], commands[fNum].upperColorLimit[2], currFrame);
		if (commands[fNum].lowerColorLimit[0] > commands[fNum].upperColorLimit[0]){
			Mat frame_low, frame_high;
			
			inRange(frame_hsv, Scalar(0, commands[fNum].lowerColorLimit[1], commands[fNum].lowerColorLimit[2]), 
			Scalar(commands[fNum].upperColorLimit[0], commands[fNum].upperColorLimit[1], commands[fNum].upperColorLimit[2]), frame_low);

			inRange(frame_hsv, Scalar(commands[fNum].lowerColorLimit[0], commands[fNum].lowerColorLimit[1], commands[fNum].lowerColorLimit[2]), 
			Scalar(255, commands[fNum].upperColorLimit[1], commands[fNum].upperColorLimit[2]), frame_high);

			currFrame= frame_low + frame_high;
		} else {
			inRange(frame_hsv, Scalar(commands[fNum].lowerColorLimit[0], commands[fNum].lowerColorLimit[1], commands[fNum].lowerColorLimit[2]), 
			Scalar(commands[fNum].upperColorLimit[0], commands[fNum].upperColorLimit[1], commands[fNum].upperColorLimit[2]), currFrame);
		}

        // Get moments
        Moments frame_moments = moments(currFrame);

        // Store x and y 
        unsigned long momentArea = commands[fNum].momentArea = (int) frame_moments.m00;
        int momentX = commands[fNum].posX = (int) (frame_moments.m10 / momentArea);
        int momentY = commands[fNum].posY = (int) (frame_moments.m01 / momentArea);

        // Store max X and Y if significant
        if (momentArea > minArea && momentArea > currMaxArea){
		areaFound = true;
		currMaxArea = momentArea;
		currMaxCommand = (CommandType) fNum;
        }

		// Draw dot and circle of command color on filtered frame
		circle(currFrame, Point(momentX, momentY), 20 /*std::sqrt(momentArea / PI)*/, /*commands[fNum].lowerColorLimit*/ Scalar(150 + 70 * fNum), 5);

        // Add filtered image
        // frame_filtered += currFrameBGR;
        frame_filtered += currFrame;
    }

	// Set command and area
	blob_currCommand = currMaxCommand;
	blob_currArea = currMaxArea;

	// Draw position of identified color
	if (areaFound){
		// TODO: Set global X and Y variables
		blob_currX = scaleCartesianPercent(commands[currMaxCommand].posX, true);
		blob_currY = scaleCartesianPercent(commands[currMaxCommand].posY, false);

		cout << "Area found of type " << commandNames[blob_currCommand] << " @ " << blob_currX << ", " << blob_currY << " \tarea: " << blob_currArea << endl;

		// TODO: Draw on original frame
		circle(frame, Point(commands[currMaxCommand].posX, commands[currMaxCommand].posY), std::sqrt(currMaxArea / PI) / 20, Scalar(0, 0, 255), 5);
			
		int fontFace = FONT_HERSHEY_COMPLEX_SMALL;
		double fontScale = 0.6;
		int thickness = 2;

		stringstream ss;
		ss << blob_currArea << " " << commandNames[blob_currCommand];
		putText(frame, ss.str(), Point(commands[currMaxCommand].posX, commands[currMaxCommand].posY), fontFace, fontScale,Scalar(0, 0, 255), thickness,8);
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
		// Capture frame
		cap >> frame;
		cvtColor(frame, frame_hsv, CV_BGR2HSV);

		// Perform filter
		filterFrame(frame_hsv, filtLowH, filtLowS, filtLowV, filtHighH, filtHighS, filtHighV, frame_filtered);

        // Display area on screen
        int fontFace = FONT_HERSHEY_COMPLEX_SMALL;
        double fontScale = 0.6;
        int thickness = 1;
        Point textOrg(0 /*frame.cols/5*/, frame.rows/1.2);
        Point textOrg2(0 /*frame.cols/5*/, frame.rows/1.0);
        stringstream ss;
        ss << "Select color and press";
        string displayText = ss.str();
        putText(frame, displayText, textOrg, fontFace, fontScale,
            /*Scalar::all(255)*/ Scalar(0, 0, 255), thickness,8);

	ss.str("");
	ss << "Enter for cmd: " << commandNames[cmd] << "  " <<currKey;
        displayText = ss.str();
        putText(frame, displayText, textOrg2, fontFace, fontScale,
            /*Scalar::all(255)*/ Scalar(0, 0, 255), thickness,8);

		// Check area
		// Get moments
        Moments frame_moments = moments(frame_filtered);

        // Store x and y 
        int momentArea = frame_moments.m00;

	ss.str("");
        ss << "Area: " << momentArea << " is enough? " << (momentArea > minArea);
        displayText = ss.str();
        putText(frame_filtered, displayText, textOrg, fontFace, fontScale, Scalar(150), thickness,8);
	
	cout << displayText << endl;

        // Display images
        displayImages();


        // Grab current key
        currKey = waitKey(30);
        if(currKey == KEY_ESC || currKey == KEY_ENTER || currKey == 'q' || currKey == KEY_ENTER_WINDOWS || currKey == 1048586 || currKey == 13) break;
    }

    // TODO: Save color to cmd
    if (currKey == KEY_ENTER || currKey == KEY_ENTER_WINDOWS || currKey == 1048586 || currKey == 13){
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

// Perform in-range HSV filter given input and output frames followed by morphological opening/closing. Performs wrap-around on H if necessary.
inline void filterFrame(Mat& inputFrame, int lowH, int lowS, int lowV, int highH, int highS, int highV, Mat& outputFrame){
	if (lowH > highH){
		Mat frame_low, frame_high;
		
		inRange(inputFrame, Scalar(lowH, lowS, lowV), 
        Scalar(255, highS, highV), frame_low);

		inRange(inputFrame, Scalar(0, lowS, lowV), 
        Scalar(highH, highS, highV), frame_high);

		outputFrame = frame_low + frame_high;
	} else {
		inRange(inputFrame, Scalar(lowH, lowS, lowV), 
        Scalar(highH, highS, highV), outputFrame);
	}

	//morphological opening (removes small objects from the foreground)
	/*erode(outputFrame, outputFrame, getStructuringElement(MORPH_ELLIPSE, Size(morphEllipse, morphEllipse)) );
	dilate( outputFrame, outputFrame, getStructuringElement(MORPH_ELLIPSE, Size(morphEllipse, morphEllipse)) ); 

	//morphological closing (removes small holes from the foreground)
	dilate( outputFrame, outputFrame, getStructuringElement(MORPH_ELLIPSE, Size(morphEllipse, morphEllipse)) ); 
	erode(outputFrame, outputFrame, getStructuringElement(MORPH_ELLIPSE, Size(morphEllipse, morphEllipse)) );
*/
}

inline void displayImages(){
    // Display image
    imshow("Raw", frame);
    imshow("HSV", frame_hsv);
    imshow("Filtered", frame_filtered);
}

inline int scaleCartesianPercent(int coordinate, bool isX){
	int outputCoord;
	
	// Subtract half of screen to center '0'
	if (isX){
		outputCoord = coordinate - (CAPTURE_FRAME_WIDTH / 2);

		// Scale pixels to percentage
		return (outputCoord * 200 / CAPTURE_FRAME_WIDTH);
	} else {
		// if this is Y coordinate, invert value
		outputCoord = (CAPTURE_FRAME_HEIGHT / 2) - coordinate;

		// Scale pixels to percentage
		return (outputCoord * 200 / CAPTURE_FRAME_HEIGHT);
	}
}
