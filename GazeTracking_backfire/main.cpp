#include "GazeTracker.h"

int main()
{
	GazeTracker myGaze;
	VideoCapture cam(0);
	myGaze.autoRun(cam);
/*	
	namedWindow("hello");
	Mat img;
	while(true)
	{
		cam >> img;
		imshow("hello", img);
		waitKey(1);
	}
*/
}