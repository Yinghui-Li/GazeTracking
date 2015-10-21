#include <iostream>
#include <fstream>
#include "dataCleaner.h"

#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/ml/ml.hpp>  
#include <opencv2/imgproc.hpp>
#include <string> 
#include <math.h>

using namespace std;
using namespace cv;

#define NEWFILEPATH "newRecord.txt"
#define FILEPATH "record.txt"
#define OFPATH   "data.txt"
#define CENTERPATH	"eyeCenter.txt"
#define CHINNELX 2
#define CHINNELY 2

void draw(DataCleaner cleaner, int m);

/*
int main()
{
	DataCleaner cleaner;
	cleaner.read(FILEPATH);
	cleaner.clean();
	cleaner.cvtStd();
	cleaner.write(OFPATH);
	draw(cleaner, 1);
	waitKey(0);
	return 0;
}
*/


int main()
{
	DataCleaner cleaner;
	cleaner.readNew(NEWFILEPATH);
//	cleaner.clean();
	cleaner.predictTest(0);
	cleaner.predictTest(1);
	cleaner.writePredicotModel(CENTERPATH);
	draw(cleaner, 0);
	draw(cleaner, 1);
	waitKey(0);
	return 0;
}


//show the data distribution
//m shows weather draw left eye's data 
//or right eye's data
//m = 0 means left eye
//m = 1 means right eye
void draw(DataCleaner cleaner, int m)
{
	vector<int>* data = cleaner.getData();
	int dataSize = cleaner.getDataSize();

	vector<double>* metaData = cleaner.getMetaData();
	int metaDataSize = cleaner.getMetaDataSize();

	int dataWidth = cleaner.getDataWidth();

	int lowerBound[2] = {-1, -1};
	int upperBound[2] = {-1, -1};

	//calculate data boundary
	if(m == 0)
	{
		for(int i=0; i <metaDataSize; i++)
		{
			for(int j=0; j < 2; j++)
			{
				if(lowerBound[j] == -1 || lowerBound[j] > metaData[i][j+12])
					lowerBound[j] = metaData[i][j+12];
				if(upperBound[j] == -1 || upperBound[j] < metaData[i][j+18])
					upperBound[j] = metaData[i][j+18];
			}
		}
	}

	else if(m == 1)
	{
		for(int i=0; i <metaDataSize; i++)
		{
			for(int j=0; j < 2; j++)
			{
				if(lowerBound[j] == -1 || lowerBound[j] > metaData[i][j+12+2])
					lowerBound[j] = metaData[i][j+12+2];
				if(upperBound[j] == -1 || upperBound[j] < metaData[i][j+18+2])
					upperBound[j] = metaData[i][j+18+2];
			}
		}
	}

	//prepare painting pad
	const int rate = 4;
	Mat image = Mat::zeros((upperBound[0]-lowerBound[0]+1)*rate,(upperBound[1]-lowerBound[1]+1)*rate, CV_8UC3);
	Vec3b white(255,255,255);

	for(int i=0; i < image.cols; i++)
	{
		for(int j=0; j < image.rows; j++)
		{
			image.at<Vec3b>(j, i) = white;
		}
	}

	//color difference
	Vec3b *posColor = new Vec3b[dataSize];
	for(int i = 0; i < CHINNELX; i++)
	{
		for(int j = 0; j < CHINNELY; j++)
		{
			posColor[i*CHINNELX+j] = Vec3b(255*(i*CHINNELX+j)/(CHINNELX*CHINNELY), 255*i/CHINNELX, 255*j/CHINNELY);
		}
	}

	//painting
	for(int i = 0; i < dataSize; i++)
	{
		for(int j = 0; j < data[i].size() / dataWidth; j++)
		{
			int l1, l2;
			if(m == 0)
			{
				l1 = (data[i][j*dataWidth] - lowerBound[0]) * rate;
				l2 = (data[i][j*dataWidth+1] - lowerBound[1]) * rate;
			}
			else if(m == 1)
			{
				l1 = (data[i][j*dataWidth+2] - lowerBound[0]) * rate;
				l2 = (data[i][j*dataWidth+3] - lowerBound[1]) * rate;
			}

			for(int k=l1; k < l1+rate; k++)
				{
					for(int l=l2; l < l2+rate; l++)
					{
						image.at<Vec3b>(k, l) = posColor[i];
					}
				}
		}
	}

	//show image
	if(m == 0)
	{
		namedWindow("leftEye");
		imshow("leftEye", image);
	}
	else if(m == 1)
	{
		namedWindow("rightEye");
		imshow("rightEye", image);
	}
}