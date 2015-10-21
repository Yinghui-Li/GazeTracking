#ifndef EYEDETECTION_H
#define EYEDETECTION_H

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <cv.h>
#include "facedetect-dll.h"

using namespace cv;
class EyeDetector
{
	public:
		EyeDetector();
		EyeDetector(const String& faceClassifier, const String& eyeClassifier);
		~EyeDetector();

		void detectFaceAndEyes(Mat& img, double scale);

		//获取脸部和眼睛的位置信息
		//格式为:[faceCenter.x,faceCenter.y,faceSize.x,faceSize.y,
		//		leftEyeCenter.x,leftEyeCenter.y,leftEyeSize.x,leftEyeSize.y,
		//		rightEyeCenter.x,rightEyeCenter.y,rightEyeSize.x,rightEyeSize.y]
		Mat getFaceAndEyePos();

	protected:
		CascadeClassifier faceCascade;
		CascadeClassifier eyeCascade;

		String faceCascadeFilename;
		String eyeCascadeFilename;
	
	public:
		CvPoint leftEyeCenter;
		CvPoint leftEyeSize;
		CvPoint rightEyeCenter;
		CvPoint rightEyeSize;
		
		CvPoint faceCenter;
		CvPoint faceSize;

		bool isRecorded;
		bool isFaceDetected;
};

#endif