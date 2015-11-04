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

	data = new vector<double>[dataWidth+2];
	double temp;
	while(!ifile.eof())
	{
		for(int i=0; i < dataWidth+2 && !ifile.eof(); i++)
		{
			ifile >> temp;
			data[i].push_back(temp);
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

vector<int> Predictor :: predict(vector<int> info)
{
	const int MAX_SIZE = 15;
	vector<int>* neighborSet = new vector<int>[dataWidth+2];
	vector<int> varLen;
	double maxLen = -1;
	int maxDisPos = -1;
	int size = data[0].size();

	for(int i=0; i < size; i++)
	{
		double tempVar = 0;
		for(int j=0; j < dataWidth; j++)
		{
			if(j % 2 == 0)
			{
				tempVar += pow(info[j]-bias[j]-data[j][i], 2);
			}else
			{
				tempVar += 14 * pow(info[j]-bias[j]-data[j][i], 2);
			}
		}

		if(neighborSet[0].size() < MAX_SIZE)
		{
			for(int j=0; j < dataWidth+2; j++)
			{
				neighborSet[j].push_back(data[j][i]);
			}

			varLen.push_back(tempVar);
			if(tempVar > maxLen)
			{
				maxLen = tempVar;
				maxDisPos = neighborSet[0].size()-1;
			}
		}else
		{
			if(tempVar < maxLen)
			{
				for(int j=0; j < dataWidth+2; j++)
				{
					neighborSet[j][maxDisPos] = data[j][i];
				}
				varLen[maxDisPos] = tempVar;
				maxLen = tempVar;

				for(int k=0; k < MAX_SIZE; k++)
				{
					if(varLen[k] > maxLen)
					{
						maxLen = varLen[k];
						maxDisPos = k;
					}
				}
			}
		}
	}

	double cordinate_x=0, cordinate_y=0;
	for(int i=0; i < MAX_SIZE; i++)
	{
		cordinate_x += neighborSet[dataWidth][i];
		cordinate_y += neighborSet[dataWidth+1][i];
	}
	cordinate_x /= MAX_SIZE;
	cordinate_y /= MAX_SIZE;

	vector<int> result;
	result.push_back((int) cordinate_x*channels[0]/screenSize[0]);
	result.push_back((int) cordinate_y*channels[1]/screenSize[1]);

	return result;
}

bool Predictor :: isTrained()
{
	if(trainData.size() >= dataWidth * 3 * 3)
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