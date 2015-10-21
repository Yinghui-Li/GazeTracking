#include "GazeTracker.h"

int main()
{
	GazeTracker myGaze;
	VideoCapture cam(0);
	myGaze.autoRun(cam);
}