#ifndef GAZETRACKER_H
#define GAZETRACKER_H

#include "Predictor.h"
#include "EyeDetection.h"
#include <fstream>

using namespace std;
using namespace cv;
using namespace ml;

#define RECORDPATH "record.txt"
#define MODULEPATH "eyeFocusDetector.xml"
#define PREDICTPATH "eyeCenter.txt"
#define LOGPATH	"info.log"

class GazeTracker
{
	public:
		GazeTracker();
		~GazeTracker();
		void autoRun(VideoCapture& cam);
	
	protected:
		void draw(const char* window, Mat& image, vector<int> circles, 
				vector<int> screenSize, vector<int> preArea = vector<int>());
		void track(Mat& image);
		void train(VideoCapture& cam);
		friend void onMouseClicked(int event, int x, int y, int flags, void* ustc);

	protected:
		Predictor focusFinder;
		EyeDetector detector;
		String windowName;
		ofstream recorder;
		ofstream log;
};

#endif