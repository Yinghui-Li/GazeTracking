#include "EyeDetection.h"
#include <vector>
using namespace std;

EyeDetector::EyeDetector()
{
	faceCascadeFilename = "./haarcascade_frontalface_alt2.xml";
	eyeCascadeFilename = "./haarcascade_eye_tree_eyeglasses.xml";

	if(!faceCascade.load(faceCascadeFilename))
	{
		cout << "fail to load faceCascade!" << endl;
	}
	if(!eyeCascade.load(eyeCascadeFilename))
	{
		cout << "fail to load eyeCascade!" << endl;
	}
	
	isRecorded = false;
}

EyeDetector::EyeDetector(const String& faceClassifier, const String& eyeClassifier)
{
	faceCascadeFilename = faceClassifier;
	eyeCascadeFilename = eyeClassifier;
	faceCascade.load(faceCascadeFilename);
	eyeCascade.load(eyeCascadeFilename);
}

EyeDetector::~EyeDetector()
{
	;
}

void EyeDetector::detectFaceAndEyes(Mat& img, double scale = 1)
{
	
	Mat gray, smallImg( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );//��ͼƬ��С���ӿ����ٶ�  
//    vector<Rect> faces;
	int* faces = NULL;

    cvtColor( img, gray, CV_BGR2GRAY );//��Ϊ�õ�����haar���������Զ��ǻ��ڻҶ�ͼ��ģ�����Ҫת���ɻҶ�ͼ��  
    resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );//���ߴ���С��1/scale,�����Բ�ֵ  
    equalizeHist( smallImg, smallImg );//ֱ��ͼ���� 

//	faceCascade.detectMultiScale( smallImg, faces,  
//        1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(50, 100) );
	faces = facedetect_frontal((unsigned char*)(gray.ptr(0)), gray.cols, gray.rows, gray.step,
												   1.2f, 2,  24);
	
	isFaceDetected = false;
	isRecorded = false;
	
	for(int i = 0; i < (faces ? *faces : 0); i++)
	{
		short* p = ((short*)(faces+1))+6*i;
		Mat smallROI;
		vector<Rect> nestedObjects;

		faceSize.x = p[2];
		faceSize.y = p[3];
		faceCenter.x = p[0] + p[2]/2;
		faceCenter.y = p[1] + p[3]/2;
		isFaceDetected = true;

		smallROI = smallImg(Rect(p[0], p[1], p[2], p[3]));
		eyeCascade.detectMultiScale(smallROI, nestedObjects, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(0,30));

		if(nestedObjects.size() == 2)
		{
			vector<Rect>::const_iterator e = nestedObjects.begin();
			leftEyeSize.x = e->width;
			leftEyeSize.y = e->height;
			leftEyeCenter.x = cvRound((p[0] + e->x + e->width*0.5)*scale);
			leftEyeCenter.y = cvRound((p[1] + e->y + e->height*0.5)*scale);
			e++;
			rightEyeSize.x = e->width;
			rightEyeSize.y = e->height;
			rightEyeCenter.x = cvRound((p[0] + e->x + e->width*0.5)*scale);
			rightEyeCenter.y = cvRound((p[1] + e->y + e->height*0.5)*scale);

			if(leftEyeCenter.x < rightEyeCenter.x)
			{
				swap(leftEyeCenter.x, rightEyeCenter.x);
				swap(leftEyeCenter.y, rightEyeCenter.y);
				swap(leftEyeSize.x, rightEyeSize.x);
				swap(leftEyeSize.y, rightEyeSize.y);
			}

			isRecorded = true;
 		}
	}
}

Mat EyeDetector::getFaceAndEyePos()
{
	float s[1][12];
	if(isRecorded || isFaceDetected)
	{
		s[0][0] = faceCenter.x;
		s[0][1] = faceCenter.y;
		s[0][2] = faceSize.x;
		s[0][3] = faceSize.y;
		s[0][4] = leftEyeCenter.x;
		s[0][5] = leftEyeCenter.y;
		s[0][6] = leftEyeSize.x;
		s[0][7] = leftEyeSize.y;
		s[0][8] = rightEyeCenter.x;
		s[0][9] = rightEyeCenter.y;
		s[0][10] = rightEyeSize.x;
		s[0][11] = rightEyeSize.y;

		return Mat(1, 12, CV_32FC1, s);
	}
	else
	{
		Mat data;
		data.data = NULL;
		return data;
	}

}