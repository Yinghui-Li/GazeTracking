#ifndef GAZETRACKER_H
#define GAZETRACKER_H

#include "ANN.h"
#include "EyeDetection.h"
using namespace cv;
using namespace ml;

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

//	public:
//		static Point click;
//		static bool isClicked;
};

#endif