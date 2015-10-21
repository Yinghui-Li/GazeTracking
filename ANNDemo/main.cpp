//The example of using BPNetwork in OpenCV  
//Coded by L. Wei    
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/ml/ml.hpp>  
#include <opencv2/imgproc.hpp>
#include <iostream>  
#include <string>  
  
using namespace std;  
using namespace cv;  
using namespace cv::ml;
  
int main()  
{  
    //Setup the BPNetwork  
	Ptr<ANN_MLP> ann = ANN_MLP::create();
    // Set up BPNetwork's parameters  
	
    //params.train_method=CvANN_MLP_TrainParams::RPROP;  
    //params.rp_dw0 = 0.1;   
    //params.rp_dw_plus = 1.2;   
    //params.rp_dw_minus = 0.5;  
    //params.rp_dw_min = FLT_EPSILON;   
    //params.rp_dw_max = 50.;  
  
    // Set up training data  
	float labels[4][5] = {{0,0,0,0,0},{1,1,1,1,1},{0,0,0,0,0},{1,0,1,1,1}};  
    Mat labelsMat(4, 5, CV_32FC1, labels);  
  
	float trainingData[4][5] = { {1,2,3,4,5},{111,112,113,114,115}, {21,22,23,24,25}, {32,14,63,24,12} };  
    Mat trainingDataMat(4, 5, CV_32FC1, trainingData);  
    Mat layerSizes=(Mat_<int>(1,5) << 5,2,2,2,5);  
	//Ptr<TrainData> params = TrainData::create(trainingDataMat, SampleTypes::ROW_SAMPLE, labelsMat);
	ann->setLayerSizes(layerSizes);
	ann->setTrainMethod(ANN_MLP::BACKPROP, 0.1, 0.1);
	ann->setActivationFunction(ANN_MLP::SIGMOID_SYM);
	ann->train(trainingDataMat, SampleTypes::ROW_SAMPLE, labelsMat);  
  
  
    // Data for visual representation  
    int width = 512, height = 512;  
    Mat image = Mat::zeros(height, width, CV_8UC3);  
    Vec3b green(0,255,0), blue (255,0,0);  
    // Show the decision regions given by the SVM  
    for (int i = 0; i < image.rows; ++i)  
        for (int j = 0; j < image.cols; ++j)  
        {  
            Mat sampleMat = (Mat_<float>(1,5) << i,j,0,0,0);  
            Mat responseMat;  
            ann->predict(sampleMat,responseMat);  
            float* p=responseMat.ptr<float>(0);  
            float response=0.0f;  
            for(int k=0;k<5;k++){  
            //  cout<<p[k]<<" ";  
                response+=p[k];  
            }  
            if (p[1] > 0 )  
                image.at<Vec3b>(j, i)  = green;  
            else    
                image.at<Vec3b>(j, i)  = blue;  
        }  
  
        // Show the training data  
        int thickness = -1;  
        int lineType = 8;  
        circle( image, Point(501,  10), 5, Scalar(  0,   0,   0), thickness, lineType);  
        circle( image, Point(255,  10), 5, Scalar(255, 255, 255), thickness, lineType);  
        circle( image, Point(501, 255), 5, Scalar(255, 255, 255), thickness, lineType);  
        circle( image, Point( 10, 501), 5, Scalar(255, 255, 255), thickness, lineType);  
  
        imwrite("result.png", image);        // save the image   
  
        imshow("BP Simple Example", image); // show it to the user  
        waitKey(0);  
  
}  