#include"ANN.h"

using namespace ml;

ANN::ANN()
{
	ann = ANN_MLP::create();
}

ANN::~ANN()
{
	;
}

void ANN::setLayerSize(Mat layerSize)
{
	if(ann != NULL)
	{
		ann->setLayerSizes(layerSize);
	}
	return;
}

void ANN::setTrainMethod(ml::ANN_MLP::TrainingMethods trainMethod)
{
	if(ann != NULL)
	{
		ann->setTrainMethod(trainMethod, 0.1, 0.1);
	}
	return;
}

void ANN::setActivationFunction(ml::ANN_MLP::ActivationFunctions activationFunction)
{
	if(ann != NULL)
	{
		ann->setActivationFunction(activationFunction);
	}
	return;
}

void ANN::train(const Mat& trainData, ml::SampleTypes sampleType, Mat& response)
{
	if(ann != NULL)
	{
		ann->train(trainData, sampleType, response);
	}
}

float ANN::predict(Mat& sampleMat, Mat& responseMat)
{
	if(ann != NULL)
	{
		return ann->predict(sampleMat, responseMat);
	}
	return 0;
}

bool ANN::load(const String& filename)
{
	ann = Algorithm::load<ANN_MLP>(filename);

	if(ann != NULL)
	{
		return true;
	}
	else
	{
		ann = ANN_MLP::create();
		return false;
	}
}

void ANN::save(const String& filename)
{
	ann->save(filename);
}

bool ANN::isTrained()
{
	return ann->isTrained();
}