#include "GazeTracker.h"
#include <iostream>


Point click;
bool isClicked;


GazeTracker::GazeTracker()
{
	if(fopen(MODULEPATH, "r") != NULL)
	{
		focusFinder.load(MODULEPATH);
	}
	else
	{
		Mat layerSize = (Mat_<int>(1,5) << 12,8,6,4,2);
		focusFinder.setLayerSize(layerSize);
		focusFinder.setTrainMethod(ANN_MLP::BACKPROP);
		focusFinder.setActivationFunction(ANN_MLP::SIGMOID_SYM);
	}

	windowName = "gazeTracking";

	recorder.open(RECORDPATH);
}

GazeTracker::~GazeTracker()
{
	focusFinder.save(MODULEPATH);
}

void GazeTracker::draw(Mat& image)
{
	Mat faceEyesInfo;
	Mat focus;
	
	double t = (double)cvGetTickCount();//用来计算算法执行时间
	detector.detectFaceAndEyes(image, 1);
	t = (double)cvGetTickCount() - t;//相减为算法执行的时间  
    printf( "detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) ); 

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
//			focusFinder.train(faceEyesInfo, SampleTypes::ROW_SAMPLE, output);
//			cout << "eyes detected and trained!" << endl;
//			focusFinder.save(MODULEPATH);

			//写入记录
			recorder << detector.faceCenter.x << ' ' << detector.faceCenter.y << ' '
					 << detector.faceSize.x << ' '
					 << detector.leftEyeCenter.x << ' ' << detector.leftEyeCenter.y << ' '
					 << detector.leftEyeSize.x << ' '
					 << detector.rightEyeCenter.x << ' ' << detector.rightEyeCenter.y << ' '
					 << detector.rightEyeSize.x << ' ' 
					 << s[0][0] << ' ' << s[0][1] << endl;

			cout << detector.faceCenter.x << ' ' << detector.faceCenter.y << ' '
				 << detector.faceSize.x << ' '
				 << detector.leftEyeCenter.x << ' ' << detector.leftEyeCenter.y << ' '
				 << detector.leftEyeSize.x << ' '
				 << detector.rightEyeCenter.x << ' ' << detector.rightEyeCenter.y << ' '
				 << detector.rightEyeSize.x << ' ' 
				 << s[0][0] << ' ' << s[0][1] << endl;

		}
	}

	isClicked = false;

	if(detector.isRecorded)
	{
		int faceRadius, leftEyeRadius, rightEyeRadius;

		faceRadius = 1;
		circle(image, Point(detector.faceCenter.x, detector.faceCenter.y),
				faceRadius, CV_RGB(0,0,255), 3, 8, 0);

		leftEyeRadius = (detector.leftEyeSize.x + detector.leftEyeSize.y) / 3;
		leftEyeRadius = 1;
		circle(image, Point(detector.leftEyeCenter.x, detector.leftEyeCenter.y),
				leftEyeRadius, CV_RGB(0,0,255), 3, 8, 0);

		rightEyeRadius = (detector.rightEyeSize.x + detector.rightEyeSize.y) / 3;
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

	Mat bigImg(800, 1100, CV_8UC3);
	resize(image, bigImg, bigImg.size());
	imshow(windowName, bigImg);
	waitKey(1);
	
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

void GazeTracker::autoRun(VideoCapture& cam)
{
	Mat image;
	IplImage ipl_img;
	CvVideoWriter* video;

	video = cvCreateVideoWriter("camera.avi", CV_FOURCC('X', 'V', 'I', 'D'), 12, cvSize(640, 480));
	namedWindow(windowName);
	cvSetMouseCallback(windowName.c_str(), onMouseClicked, 0);
	int i = 0;
	while(i < 2000)
	{
		cam >> image;
		ipl_img = image;
		cvWriteFrame(video, &ipl_img);
		draw(image);
		waitKey(1);
		i++;
	}
	cvReleaseVideoWriter(&video);
}