#ifndef GAZETRACKER_H
#define GAZETRACKER_H

#include "ANN.h"
#include "EyeDetection.h"
#include <fstream>

using namespace std;
using namespace cv;
using namespace ml;

#define RECORDPATH "record.txt"
#define MODULEPATH "eyeFocusDetector.xml"

class GazeTracker
{
	public:
		GazeTracker();
		~GazeTracker();
		void autoRun(VideoCapture& cam);
	
	protected:
		void draw(Mat& image);
		friend void onMouseClicked(int event, int x, int y, int flags, void* ustc);

	protected:
		ANN focusFinder;
		EyeDetector detector;
		String windowName;
		ofstream recorder;

//	public:
//		static Point click;
//		static bool isClicked;
};

#endif