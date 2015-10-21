// face_detect.cpp : �������̨Ӧ�ó������ڵ㡣  
//  
  
//#include "stdafx.h"  
  
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml/ml.hpp"
#include <cv.h>

#include <fstream>
#include <iostream>
#include <stdio.h>

using namespace std;  
using namespace cv;  
  
void detectAndDraw( Mat& img,  
                   CascadeClassifier& cascade, CascadeClassifier& nestedCascade,  
                   double scale, ofstream& record);  
  
String cascadeName = "./haarcascade_frontalface_alt2.xml";//������ѵ������  
//String nestedCascadeName = "./haarcascade_eye_tree_eyeglasses.xml";//���۵�ѵ������  
String nestedCascadeName = "./haarcascade_eye_tree_eyeglasses.xml";//���۵�ѵ������  
String recordFileName = "./record.txt"; //���ݼ�¼

int point_x, point_y;
bool is_clicked = false;
  
void on_mouse( int event, int x, int y, int flags, void* ustc)  
{  
    if( event == CV_EVENT_LBUTTONDOWN )  
    {  
		point_x = x;
		point_y = y;
		is_clicked = true;
    }   
} 

int main( int argc, const char** argv )  
{  
    Mat image;  
    CascadeClassifier cascade, nestedCascade;//������������������ 
	ofstream record(recordFileName.c_str());

    double scale = 1;  
	namedWindow("result", 1);
	cvSetMouseCallback("result", on_mouse, 0);
	VideoCapture cam(0);
	if( !cascade.load( cascadeName ) )//��ָ�����ļ�Ŀ¼�м��ؼ���������  
		{  
			 cerr << "ERROR: Could not load classifier cascade����������" << endl;  
			 return 0;  
		}  
  
	if( !nestedCascade.load( nestedCascadeName ) )  
		{  
			 cerr << "WARNING: Could not load classifier cascade for nested objects" << endl;  
			 return 0;  
		}  
      
    while(true){
		cam >> image;

		if( !image.empty() )//��ȡͼƬ���ݲ���Ϊ��  
		{  
			detectAndDraw( image, cascade, nestedCascade, scale, record );
			waitKey(1);
		}  
	}
  
    return 0;  
}  
  
void detectAndDraw( Mat& img,  
                   CascadeClassifier& cascade, CascadeClassifier& nestedCascade,  
				   double scale, ofstream& record)  
{  
    int i = 0;  
    double t = 0;  
    vector<Rect> faces;  
    const static Scalar colors[] =  { CV_RGB(0,0,255),  
        CV_RGB(0,128,255),  
        CV_RGB(0,255,255),  
        CV_RGB(0,255,0),  
        CV_RGB(255,128,0),  
        CV_RGB(255,255,0),  
        CV_RGB(255,0,0),  
        CV_RGB(255,0,255)} ;//�ò�ͬ����ɫ��ʾ��ͬ������  
  
    Mat gray, smallImg( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );//��ͼƬ��С���ӿ����ٶ�  
  
    cvtColor( img, gray, CV_BGR2GRAY );//��Ϊ�õ�����haar���������Զ��ǻ��ڻҶ�ͼ��ģ�����Ҫת���ɻҶ�ͼ��  
    resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );//���ߴ���С��1/scale,�����Բ�ֵ  
    equalizeHist( smallImg, smallImg );//ֱ��ͼ����  
  
    t = (double)cvGetTickCount();//���������㷨ִ��ʱ��  
  
  
//�������  
 //detectMultiScale������smallImg��ʾ����Ҫ��������ͼ��ΪsmallImg��faces��ʾ��⵽������Ŀ�����У�1.1��ʾ  
 //ÿ��ͼ��ߴ��С�ı���Ϊ1.1��2��ʾÿһ��Ŀ������Ҫ����⵽3�β��������Ŀ��(��Ϊ��Χ�����غͲ�ͬ�Ĵ��ڴ�  
 //С�����Լ�⵽����),CV_HAAR_SCALE_IMAGE��ʾ�������ŷ���������⣬��������ͼ��Size(30, 30)ΪĿ���  
 //��С���ߴ�  
    cascade.detectMultiScale( smallImg, faces,  
        1.1, 2, 0|CV_HAAR_SCALE_IMAGE,Size(30, 30) );  
  
    t = (double)cvGetTickCount() - t;//���Ϊ�㷨ִ�е�ʱ��  
    printf( "detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );  
    for( vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++ )  
    {  
        Mat smallImgROI;  
        vector<Rect> nestedObjects;  
        Point center;  
        Scalar color = colors[i%8];  
        int radius;  

		center.x = cvRound((r->x + r->width*0.5)*scale);//��ԭ��ԭ���Ĵ�С  
		center.y = cvRound((r->y + r->height*0.5)*scale);  
		radius = cvRound((r->width + r->height)*0.25*scale);  
		circle( img, center, radius, color, 3, 8, 0 );

		if(is_clicked)
		{
			cout << "[" << center.x << "," << center.y << "] ";
		}
  
        //������ۣ���ÿ������ͼ�ϻ�������  
        if( nestedCascade.empty() )  
            continue;  
        smallImgROI = smallImg(*r);  
  
        //������ĺ�������һ��  
        nestedCascade.detectMultiScale( smallImgROI, nestedObjects,  
            1.1, 2, 0  
            //|CV_HAAR_FIND_BIGGEST_OBJECT  
//|CV_HAAR_DO_ROUGH_SEARCH  
//|CV_HAAR_DO_CANNY_PRUNING  
            |CV_HAAR_SCALE_IMAGE  
            ,  
            Size(0, 30) );  
		if(nestedObjects.size() == 2)
		{
			 
			for( vector<Rect>::const_iterator nr = nestedObjects.begin(); nr != nestedObjects.end(); nr++ )  
			{  
				center.x = cvRound((r->x + nr->x + nr->width*0.5)*scale);  
				center.y = cvRound((r->y + nr->y + nr->height*0.5)*scale); 
				radius = 0;
				//radius = cvRound((nr->width + nr->height)*0.25*scale);  
				circle( img, center, radius, color, 3, 8, 0 );//���۾�Ҳ���������Ͷ�Ӧ������ͼ����һ����  

				if(is_clicked)
				{
					cout << "[" << center.x << "," << center.y << "] ";
				}
			}  
		}
		//break; //ֻ���һ������
    }  

	if(is_clicked)
	{
		cout << "[" << point_x << "," << point_y << "]" << endl;
	}
	is_clicked = false;
    cv::imshow( "result", img );  
}