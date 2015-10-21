#include "Predictor.h"
#include <fstream>
#include <math.h>

bool Predictor :: load(char* filename)
{
	ifstream ifile(filename);
	if(ifile.fail())
	{
		return false;
	}
	
	ifile >> channels[0] >> channels[1];
	if(ifile.eof() || channels[0] < 1 || channels[1] < 1)
		return false;

	ifile >> screenSize[0] >> screenSize[1];
	if(ifile.eof() || screenSize[0] < channels[0] ||
		screenSize[1] < channels[1])
		return false;

	ifile >> dataWidth;
	if(ifile.eof() || dataWidth < 1)
		return false;

	metaData = new vector<double>[channels[0] * channels[1]];
	for(int i=0; i < channels[0]*channels[1]; i++)
	{
		for(int j=0; j < dataWidth; j++)
		{
			double temp;
			if(ifile.eof())
				return false;
			ifile >> temp;
			metaData[i].push_back(temp);
		}
	}

	bias = new double[channels[0] * channels[1]];
	for(int i=0; i < channels[0]*channels[1]; i++)
	{
		bias[i] = 0;
	}

	return true;
}

bool Predictor :: insertTrainData(vector<int> data)
{
	if(data.size() != dataWidth)
		return false;

	for(int i=0; i < dataWidth; i++)
	{
		trainData.push_back(data[i]);
	}

	if(isTrained())
	{
		int size = trainData.size() / dataWidth;
		for(int i=0; i < dataWidth; i++)
		{
			int sum=0;
			for(int j=0; j < size; j++)
			{
				sum += trainData[i+j*dataWidth];
			}
			bias[i] = sum / size;
		}
	}

	return true;
}

vector<int> Predictor :: predict(vector<int> data)
{
	int pos[2];
	double square = -1;
	for(int i=0; i < channels[0]; i++)
	{
		for(int j=0; j < channels[1]; j++)
		{
			double temp = getLength(data, metaData[i*channels[0]+j]);
			if(temp > 0 && (square < 0 || square > temp))
			{
				pos[0] = i;
				pos[1] = j;
				square = temp;
			}
		}
	}
	vector<int> temp;
	temp.push_back(pos[0]);
	temp.push_back(pos[1]);
	return temp;
}

bool Predictor :: isTrained()
{
	if(trainData.size() >= dataWidth * channels[0] * channels[1])
		return true;

	return false;
}

vector<int> Predictor :: getChannels()
{
	vector<int> temp;
	temp.push_back(channels[0]);
	temp.push_back(channels[1]);
	return temp;
}

vector<int> Predictor :: getScreenSize()
{
	vector<int> temp;
	temp.push_back(screenSize[0]);
	temp.push_back(screenSize[1]);
	return temp;
}

double Predictor :: getLength(vector<int>& data1, vector<double>& data2)
{
	if(data1.size() != data2.size())
		return -1;

	int len = data1.size();
	double square = 0;
	for(int i=0; i < len; i++)
	{
		square += pow(data1[i]-bias[i]-data2[i], 2);
	}
	square /= len;
	return square;
}

void Predictor :: logInfo(ofstream& log)
{
	log << "***********************************************" << endl;
	log << "predictor info:" << endl;

	log << "eye center:" << endl;
	for(int i=0; i < channels[0] * channels[1]; i++)
	{
		log << "第" << i << "行：";
		for(int j=0; j < dataWidth; j++)
		{
			log << metaData[i][j] << ' ';
		}
		log << endl;
	}

	log << "train data:" << endl;
	for(int i=0; i < channels[0] * channels[1]; i++)
	{
		log << "第" << i << "行：";
		for(int j=0; j < dataWidth; j++)
		{
			log << trainData[i*dataWidth+j] << ' ';
		}
		log << endl;
	}

	if(bias != NULL)
	{
		log << "bias:" << endl;
		for(int i=0; i < dataWidth; i++)
		{
			log << bias[i] << ' ';
		}
		log << endl;
	}

	return;
}