#ifndef ANN_H
#define ANN_H

#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/ml/ml.hpp> 
using namespace cv;
using namespace ml;

class ANN
{
	public:
		ANN();
		~ANN();
		void setLayerSize(Mat layerSize);
		void setTrainMethod(ml::ANN_MLP::TrainingMethods trainMethod);
		void setActivationFunction(ml::ANN_MLP::ActivationFunctions activationFunction);
		void train(const Mat& trainData, ml::SampleTypes sampleType, Mat& response);
		float predict(Mat& sampleMat, Mat& responseMat);
		bool load(const String& filename);
		void save(const String& filename);
		bool isTrained();
	
	private:
		Ptr<ANN_MLP> ann;
};

#endif