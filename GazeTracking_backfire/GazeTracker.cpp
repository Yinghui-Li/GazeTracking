#include "GazeTracker.h"
#include <iostream>
#include <fstream>

using namespace std;

Point click;
bool isClicked;


GazeTracker::GazeTracker()
{
	if(fopen("module.xml", "r") != NULL)
	{
		focusFinder.load("module.xml");
	}
	else
	{
		Mat layerSize = (Mat_<int>(1,5) << 12,8,6,4,2);
		focusFinder.setLayerSize(layerSize);
		focusFinder.setTrainMethod(ANN_MLP::BACKPROP);
		focusFinder.setActivationFunction(ANN_MLP::SIGMOID_SYM);
	}

	windowName = "gazeTracking";
}

GazeTracker::~GazeTracker()
{
	focusFinder.save("module.xml");
}

void GazeTracker::draw(Mat& image)
{
	Mat faceEyesInfo;
	Mat focus;
	
	detector.detectFaceAndEyes(image, 1);
	if(detector.isRecorded || detector.isFaceDetected)
	{
		faceEyesInfo = detector.getFaceAndEyePos();
	}
	else
	{
		faceEyesInfo.data = NULL;
	}

	if(isClicked == true)
	{
		float s[1][2];
		s[0][0] = click.x;
		s[0][1] = click.y;
		Mat output(1, 2, CV_32FC1, s);

		if(faceEyesInfo.data == NULL)
		{
			cout << "fail to detect eyes!" << endl;
		}

		else
		{
			focusFinder.train(faceEyesInfo, SampleTypes::ROW_SAMPLE, output);
			cout << "eyes detected and tained!" << endl;
			focusFinder.save("module.xml");
		}
	}

	isClicked = false;

	if(detector.isRecorded)
	{
		if(focusFinder.isTrained())
		{
			focusFinder.predict(faceEyesInfo, focus);

			circle(image, Point(focus.data[0], focus.data[1]),
				4, CV_RGB(255,0,0), 3, 8, 0);
		}

		int faceRadius, leftEyeRadius, rightEyeRadius;

		faceRadius = (detector.faceSize.x + detector.faceSize.y) / 3;
		circle(image, Point(detector.faceCenter.x, detector.faceCenter.y),
				faceRadius, CV_RGB(0,0,255), 3, 8, 0);

//		leftEyeRadius = (detector.leftEyeSize.x + detector.leftEyeSize.y) / 3;
		leftEyeRadius = 1;
		circle(image, Point(detector.leftEyeCenter.x, detector.leftEyeCenter.y),
				leftEyeRadius, CV_RGB(0,0,255), 3, 8, 0);

//		rightEyeRadius = (detector.rightEyeSize.x + detector.rightEyeSize.y) / 3;
		rightEyeRadius = 1;
		circle(image, Point(detector.rightEyeCenter.x, detector.rightEyeCenter.y),
				rightEyeRadius, CV_RGB(0,0,255), 3, 8, 0);
	}

	else if(detector.isFaceDetected)
	{
		int faceRadius = (detector.faceSize.x + detector.faceSize.y) / 3;
		circle(image, Point(detector.faceCenter.x, detector.faceCenter.y),
				faceRadius, CV_RGB(0,0,255), 3, 8, 0);
	}

	imshow(windowName, image);
	
}

void onMouseClicked(int event, int x, int y, int flags, void* ustc)
{
	if( event == CV_EVENT_LBUTTONDOWN )  
    {  
/*		
		Mat output = (Mat_<int>(1,2) << x, y);
		Mat input;
		Mat image;

		cam >> image;
		myGaze.detector.detectFaceAndEyes(image, 1);
		input = myGaze.detector.getFaceAndEyePos();

		if(input.data == NULL)
		{
			cout << "fail to detect eyes!" << endl;
		}

		else
		{
			myGaze.focusFinder.train(input, SampleTypes::ROW_SAMPLE, output);
		}
*/		
		click.x = x;
		click.y = y;
		isClicked = true;
    }   
}

void GazeTracker::autoRun(VideoCapture& cam)
{
	Mat image;

	namedWindow(windowName);
	cvSetMouseCallback(windowName.c_str(), onMouseClicked, 0);

	while(true)
	{
		cam >> image;
		draw(image);
		waitKey(1);
	}
}