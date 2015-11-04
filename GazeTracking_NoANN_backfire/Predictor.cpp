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

	metaData = new vector<double>[2];
	for(int i=0; i < channels[0]*2; i++)
	{
		double temp;
		if(ifile.eof())
			return false;
		ifile >> temp;
		metaData[0].push_back(temp);
	}

	for(int i=0; i < channels[1]*2; i++)
	{
		double temp;
		if(ifile.eof())
			return false;
		ifile >> temp;
		metaData[1].push_back(temp);
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
		double tmpSqr = pow(data[0] - bias[0] - metaData[0][i*2], 2) +
						pow(data[2] - bias[2] - metaData[0][i*2+1], 2);
		if(square == -1 || tmpSqr < square)
		{
			pos[0] = i;
			square = tmpSqr;
		}
	}

	square = -1;
	for(int i=0; i < channels[1]; i++)
	{
		double tmpSqr = pow(data[1] - bias[1] - metaData[1][i*2], 2) +
						pow(data[3] - bias[3] - metaData[1][i*2+1], 2);
		if(square == -1 || tmpSqr < square)
		{
			pos[1] = i;
			square = tmpSqr;
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

	log << "metaData:" << endl;
	for(int i=0; i < 2; i++)
	{
		for(int j=0; j < metaData[i].size(); j++)
		{
			log << metaData[i][j] << ' ';
		}
		log << endl;
	}

	log << "train data:" << endl;
	for(int i=0; i < channels[0] * channels[1]; i++)
	{
		log << "µÚ" << i << "ÐÐ£º";
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