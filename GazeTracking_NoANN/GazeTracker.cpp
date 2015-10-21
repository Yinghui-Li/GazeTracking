#include "GazeTracker.h"
#include <iostream>


Point click;
bool isClicked;


GazeTracker::GazeTracker()
{
	focusFinder.load(PREDICTPATH);

	windowName = "gazeTracking";

	recorder.open(RECORDPATH);
	log.open(LOGPATH);
}

GazeTracker::~GazeTracker()
{
	recorder.close();
	log.close();
}

void GazeTracker::draw(const char* window, Mat& image, vector<int> circles, 
					   vector<int> screenSize, vector<int> preArea)
{
	if(circles.size() % 3 != 0)
		return;
	if(screenSize.size() != 2)
		return;

	int faceRadius, leftEyeRadius, rightEyeRadius;

	for(int i=0; i < circles.size()/3; i++)
	{
		circle(image, Point(circles[i*3], circles[i*3+1]), circles[i*3+2], CV_RGB(0,0,255), 3, 8, 0);
	}

	Mat bigImg(screenSize[1], screenSize[0], CV_8UC3);
	resize(image, bigImg, bigImg.size());
	if(preArea.size() == 4)
	{
		Mat imageROI, covImage;
		
		imageROI = bigImg(Rect(preArea[0], preArea[1], preArea[2], preArea[3]));
		covImage = Mat :: zeros(preArea[3], preArea[2], CV_8UC3);
		for(int i=0; i < preArea[3]; i++)
		{
			for(int j=0; j < preArea[2]; j++)
			{
				covImage.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
			}
		}

		addWeighted(imageROI, 0.5, covImage, 0.5, 0, imageROI);
	}
	imshow(window, bigImg);
	
	waitKey(1);
	return;
}

void onMouseClicked(int event, int x, int y, int flags, void* ustc)
{
	if( event == CV_EVENT_LBUTTONDOWN )  
    {  		
		click.x = x;
		click.y = y;
		isClicked = true;
    }   
}

void GazeTracker :: track(Mat& image)
{
	log << "***********************************************" << endl;
	log << "tracking" << endl;
	vector<int> faceEyeInfo;

	detector.detectFaceAndEyes(image, 1);
	vector<int> screenSize = focusFinder.getScreenSize();
	vector<int> channels = focusFinder.getChannels();
	vector<int> preArea;

	if(detector.isRecorded)
	{
		faceEyeInfo.push_back(detector.faceCenter.x);
		faceEyeInfo.push_back(detector.faceCenter.y);
		faceEyeInfo.push_back(detector.faceSize.x);
		faceEyeInfo.push_back(detector.leftEyeCenter.x);
		faceEyeInfo.push_back(detector.leftEyeCenter.y);
		faceEyeInfo.push_back(detector.leftEyeSize.x);
		faceEyeInfo.push_back(detector.rightEyeCenter.x);
		faceEyeInfo.push_back(detector.rightEyeCenter.y);
		faceEyeInfo.push_back(detector.rightEyeSize.x);

		//if(focusFinder.isTrained())
		{
			vector<int> eyeInfo;
			eyeInfo.push_back(detector.leftEyeCenter.x);
			eyeInfo.push_back(detector.leftEyeCenter.y);
			eyeInfo.push_back(detector.rightEyeCenter.x);
			eyeInfo.push_back(detector.rightEyeCenter.x);
			vector<int> pos = focusFinder.predict(eyeInfo);
			
			log << "eye position is: " << detector.leftEyeCenter.x << ' '
				<< detector.leftEyeCenter.y << ' '
				<< detector.rightEyeCenter.x << ' '
				<< detector.rightEyeCenter.y << ' ';

			log << "predict pos is : (" << pos[0] << ", " << pos[1] << ")" << endl;

			preArea.push_back(pos[0] * screenSize[0] / channels[0]);
			preArea.push_back(pos[1] * screenSize[1] / channels[1]);
			preArea.push_back(screenSize[0] / channels[0]);
			preArea.push_back(screenSize[1] / channels[1]);
		}
	}

	draw(windowName.c_str(), image, faceEyeInfo, screenSize, preArea);
	return;
}

void GazeTracker :: train(VideoCapture& cam)
{
	Mat image;
	vector<int> channels = focusFinder.getChannels();
	vector<int> screenSize = focusFinder.getScreenSize();
	vector<int> span;
	
	log << "***********************************************" << endl;
	log << "train info:" << endl;
	
	for(int i=0; i < screenSize.size(); i++)
	{
		int temp = screenSize[i] / channels[i];
		span.push_back(temp);
	}

	for(int i=0; i < channels[0]; i++)
	{
		for(int j=0; j < channels[1]; j++)
		{
			while(true)
			{
				vector<int> faceEyeInfo;
				cam >> image;

				detector.detectFaceAndEyes(image, 1);
				if(detector.isRecorded)
				{
					faceEyeInfo.push_back(detector.faceCenter.x);
					faceEyeInfo.push_back(detector.faceCenter.y);
					faceEyeInfo.push_back(detector.faceSize.x);
					faceEyeInfo.push_back(detector.leftEyeCenter.x);
					faceEyeInfo.push_back(detector.leftEyeCenter.y);
					faceEyeInfo.push_back(detector.leftEyeSize.x);
					faceEyeInfo.push_back(detector.rightEyeCenter.x);
					faceEyeInfo.push_back(detector.rightEyeCenter.y);
					faceEyeInfo.push_back(detector.rightEyeSize.x);
					faceEyeInfo.push_back(i * image.cols/channels[0] + image.cols/channels[0]/2);
					faceEyeInfo.push_back(j * image.rows/channels[1] + image.rows/channels[1]/2);
					faceEyeInfo.push_back(5);
				}
				draw(windowName.c_str(), image, faceEyeInfo, screenSize);

				if(isClicked == true)
				{
					isClicked = false;
					if(abs(click.x - i * span[0] - span[0]/2) < 5 &&
					   abs(click.y - j * span[1] - span[1]/2) < 5)
					{
						vector<int> temp;
						temp.push_back(detector.leftEyeCenter.x);
						temp.push_back(detector.leftEyeCenter.y);
						temp.push_back(detector.rightEyeCenter.x);
						temp.push_back(detector.rightEyeCenter.y);
						focusFinder.insertTrainData(temp);
						cout << "( " << i << ", " << j << ") recorded" << endl;
						log << "( " << i << ", " << j << ") eye position:" 
							<< detector.leftEyeCenter.x << ' '
							<< detector.leftEyeCenter.y << ' '
							<< detector.rightEyeCenter.x << ' '
							<< detector.rightEyeCenter.y << endl;
						break;
					}
				}
			}
		}
	}

	focusFinder.logInfo(log);
}

void GazeTracker::autoRun(VideoCapture& cam)
{
	Mat image;

	namedWindow(windowName);
	cvSetMouseCallback(windowName.c_str(), onMouseClicked, 0);
	
	train(cam);

	while(true)
	{
		cam >> image;
		track(image);
	}
	
}