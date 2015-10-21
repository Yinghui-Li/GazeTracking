/*
The MIT License (MIT)

Copyright (c) 2015 Shiqi Yu
shiqi.yu@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <opencv.hpp>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml/ml.hpp"
#include "facedetect-dll.h"
#pragma comment(lib,"libfacedetect.lib")

using namespace cv;

int main(int argc, char* argv[])
{
	//load an image and convert it to gray (single-channel)
	VideoCapture cam(0);
	Mat img, gray;
	Point center;
	int radius;
	double t;

	namedWindow("face");

	while(true)
	{
		t = (double)cvGetTickCount();//用来计算算法执行时间 
		cam >> img;
		cvtColor( img, gray, CV_BGR2GRAY );
		equalizeHist( gray, gray );

		if(gray.empty())
		{
			fprintf(stderr, "Can not load the image file.\n");
			continue;
		}

		int * pResults = NULL; 
	
		///////////////////////////////////////////
		// frontal face detection 
		// it's fast, but cannot detect side view faces
		//////////////////////////////////////////
		//!!! The input image must be a gray one (single-channel)
		//!!! DO NOT RELEASE pResults !!!
		pResults = facedetect_frontal((unsigned char*)(gray.ptr(0)), gray.cols, gray.rows, gray.step,
												   1.2f, 2,  24);
		printf("%d frontal faces detected.\n", (pResults ? *pResults : 0));
		//print the detection results
		for(int i = 0; i < (pResults ? *pResults : 0); i++)
		{
			short * p = ((short*)(pResults+1))+6*i;
			int x = p[0];
			int y = p[1];
			int w = p[2];
			int h = p[3];
			int neighbors = p[4];

			center.x = x + w/2;
			center.y = y + h/2;

			radius = w/2;

			circle(img, center, radius, CV_RGB(255,0,0), 3, 8, 0);


			//printf("face_rect=[%d, %d, %d, %d], neighbors=%d\n", x,y,w,h,neighbors);
		}

		imshow("face", img);
		t = (double)cvGetTickCount() - t;//相减为算法执行的时间  
		printf( "detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );  

		waitKey(1);
/*
		///////////////////////////////////////////
		// multiview face detection 
		// it can detection side view faces, but slower than the frontal face detection.
		//////////////////////////////////////////
		//!!! The input image must be a gray one (single-channel)
		//!!! DO NOT RELEASE pResults !!!
		pResults = facedetect_multiview((unsigned char*)(gray.ptr(0)), gray.cols, gray.rows, gray.step,
												   1.2f, 2, 24);
		printf("%d faces detected.\n", (pResults ? *pResults : 0));

		//print the detection results
		for(int i = 0; i < (pResults ? *pResults : 0); i++)
		{
			short * p = ((short*)(pResults+1))+6*i;
			int x = p[0];
			int y = p[1];
			int w = p[2];
			int h = p[3];
			int neighbors = p[4];
			int angle = p[5];

			printf("face_rect=[%d, %d, %d, %d], neighbors=%d, angle=%d\n", x,y,w,h,neighbors, angle);
		}
		*/
	}



	return 0;
}

