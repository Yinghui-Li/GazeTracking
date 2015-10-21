/*
	Author : Li Yinghui
	Date   : 2015.10.06
*/

#include <fstream>
#include <iostream>
#include <math.h>
#include "dataCleaner.h"

using namespace std;

DataCleaner :: DataCleaner():factor(5),
	inputDataWidth(11),
	dataWidth(6),
	metaDataWidth(24)
{
	channels[0] = 3, channels[1] = 3;
	screenSize[0] = 1100, screenSize[1] = 800;

	data = new vector<int>[channels[0] * channels[1]];
	metaData = new vector<double>[channels[0] * channels[1]];
	
	return;
}

DataCleaner :: ~DataCleaner()
{
//	delete []data;
//	delete []metaData;
}

bool DataCleaner :: read(char* filePath)
{
	ifstream file(filePath);
	if(!file)
		return false;

	int *buff = new int[inputDataWidth];
	int x, y;
	//load data
	while(!file.eof())
	{
		for(int i=0; i < inputDataWidth; i++)
		{
			file >> buff[i];
		}
		x = buff[9] * channels[0] / screenSize[0];
		y = buff[10] * channels[1] / screenSize[1];
		
		int j = x * channels[0] + y;
		data[j].push_back(buff[3]);
		data[j].push_back(buff[4]);
		data[j].push_back(buff[6]);
		data[j].push_back(buff[7]);
		data[j].push_back(buff[9]);
		data[j].push_back(buff[10]);
	}

	calMetaData();
	return true;
}

bool DataCleaner::readNew(char* filePath)
{
	ifstream file(filePath);
	if(!file)
		return false;

	int *buff = new int[dataWidth];
	int x, y;
	//load data
	while(!file.eof())
	{
		for(int i=0; i < dataWidth; i++)
		{
			file >> buff[i];
		}
		x = buff[4] * channels[0] / screenSize[0];
		y = buff[5] * channels[1] / screenSize[1];
		
		int j = x * channels[0] + y;
		data[j].push_back(buff[0]);
		data[j].push_back(buff[1]);
		data[j].push_back(buff[2]);
		data[j].push_back(buff[3]);
		data[j].push_back(buff[4]);
		data[j].push_back(buff[5]);
	}

	calMetaData();
	return true;
}

void DataCleaner :: calMetaData()
{
	if(metaData != NULL)
		delete []metaData;
	
	metaData = new vector<double>[channels[0]*channels[1]];
	for(int i=0; i < channels[0] * channels[1]; i++)
	{
		calAverage(i);
		calSquare(i);
		calBoundary(i);
	}
}

void DataCleaner :: calAverage(int x)
{
	vector<int>::iterator iter = data[x].begin();
	double* sum = new double[dataWidth];
	for(int i=0; i < dataWidth; i++)
		sum[i] = 0;

	while(iter != data[x].end())
	{
		for(int i=0; i < dataWidth; i++)
		{
			sum[i] += *iter;
			iter++;
		}
	}

	for(int i=0; i < dataWidth; i++)
	{
		metaData[x].push_back(sum[i]*dataWidth/data[x].size());
	}
}

void DataCleaner :: calSquare(int x)
{
	vector<int>::iterator iter = data[x].begin();
	double* square = new double[dataWidth];
	for(int i=0; i < dataWidth; i++)
		square[i] = 0;

	while(iter != data[x].end())
	{
		for(int i=0; i < dataWidth; i++)
		{
			square[i] += pow(*iter - metaData[x][i], 2);
			iter++;
		}
	}

	for(int i=0; i < dataWidth; i++)
	{
		metaData[x].push_back(square[i]*dataWidth/data[x].size());
	}
}

void DataCleaner :: calBoundary(int x)
{
	vector<int>::iterator iter = data[x].begin();
	double* boundary = new double[dataWidth*2];
	for(int i=0; i < dataWidth*2; i++)
		boundary[i] = -1;

	while(iter != data[x].end())
	{
		for(int i=0; i < dataWidth; i++)
		{
			if(boundary[i] == -1 || boundary[i] > *iter)
			{
				boundary[i] = *iter;
			}
			if(boundary[i+dataWidth] == -1 || boundary[i+dataWidth] < *iter)
			{
				boundary[i+dataWidth] = *iter;
			}
			iter++;
		}
	}

	for(int i=0; i < dataWidth*2; i++)
	{
		metaData[x].push_back(boundary[i]);
	}
}

//This function will clean some of the dirty data
//It will be called by function "clean" repeatedly 
void DataCleaner :: cleanOnce()
{
	if(data == NULL || metaData == NULL)
		return;

	vector<int>* newData = new vector<int>[channels[0]*channels[1]];
	double square;

	for(int i=0; i < channels[0]*channels[1]; i++)
	{
		for(int j=0; j < data[i].size() / dataWidth; j++)
		{
			square = 0;
			int k=0;
			for(k=0; k < dataWidth; k++)
			{
				if(k < 4)
				{
					square = pow(data[i][j*dataWidth+k]-metaData[i][k], 2);
					if(square > metaData[i][k+dataWidth] * factor)
						break;
				}
			}

			if(k == dataWidth)
			{
				for(k = 0; k < dataWidth; k++)
					newData[i].push_back(data[i][j*dataWidth+k]);
			}
		}
	}

	delete []data;
	data = newData;
	calMetaData();
	return;
}

void DataCleaner :: clean()
{
	vector<int>::size_type *dataLen;
	dataLen = new vector<int>::size_type[channels[0] * channels[1]];
	bool mark;

	for(int i = 0; i < channels[0] * channels[1]; i++)
	{
		dataLen[i] = data[i].size();
	}

	while(true)
	{
		mark = false;
		cleanOnce();

		for(int i = 0; i < channels[0] * channels[1]; i++)
		{
			if(dataLen[i] != data[i].size())
			{
				dataLen[i] = data[i].size();
				mark = true;
			}
		}
		
		if(!mark)
		{
			break;
		}
	}
	calMetaData();
}

vector<int>* DataCleaner :: getData()
{
	return data;
}

vector<double>* DataCleaner :: getMetaData()
{
	return metaData;
}

int DataCleaner :: getDataSize()
{
	return channels[0] * channels[1];
}

int DataCleaner :: getMetaDataSize()
{
	return channels[0] * channels[1];
}

int DataCleaner :: getDataWidth()
{
	return dataWidth;
}

//ÓÐ´íÎó
void DataCleaner :: cvtStd()
{
	if(metaData == NULL)
		return;
	double *average = new double[4];
	int sum = 0;

	for(int i=0; i < channels[0] * channels[1]; i++)
	{
		sum += data[i].size();
	}
	for(int k=0; k < 4; k++)
	{
		average[k] = 0;
	}

	for(int k=0; k < 4; k++)
	{
		for(int i=0; i < channels[0] * channels[1]; i++)
		{
			average[k] += metaData[i][k] * data[i].size();
		}
		average[k] /= sum;
	}

	for(int i=0; i < channels[0] * channels[1]; i++)
	{
		for(int j=0; j < data[i].size()/dataWidth; j++)
		{
			for(int k=0; k < dataWidth; k++)
			{
				if(k < 4)
				{
					data[i][j*dataWidth+k] -= average[k];
				}
			}
		}
	}
	calMetaData();
}


bool DataCleaner :: write(char* filePath)
{
	ofstream file(filePath);
	if(file.fail())
		return false;

	for(int i=0; i < channels[0] * channels[1]; i++)
	{
		for(int j=0; j < data[i].size(); j++)
		{
			file << data[i][j] << ' ';
			if((j % dataWidth) == 5)
				file << endl;
		}
	}
	
	file.close();
	return true;
}

void DataCleaner :: predictTest(int mark)
{
	int c = 0;
	for(int i=0; i < channels[0]*channels[1]; i++)
	{
		for(int j=0; j < data[i].size() / dataWidth; j++)
		{
			if(mark == 0)
			{
				int len = -1;
				int n = 0;
				for(int k=0; k < channels[0]*channels[1]; k++)
				{
					if(len == -1 ||	len > (pow(data[i][j*dataWidth]-metaData[i][0], 2) + 
											pow(data[i][j*dataWidth+1]-metaData[i][1], 2)))
					{
						len = pow(data[i][j*dataWidth]-metaData[i][0], 2) + 
											pow(data[i][j*dataWidth+1]-metaData[i][1], 2);
						n = k;
					}
				}
				if(n != i)
				{
					c++;
				}
			}else if(mark == 1)
			{
				int len = -1;
				int n = 0;
				for(int k=0; k < channels[0]*channels[1]; k++)
				{
					if(len == -1 ||	len > (pow(data[i][j*dataWidth+2]-metaData[i][2], 2) + 
											pow(data[i][j*dataWidth+3]-metaData[i][3], 2)))
					{
						len = pow(data[i][j*dataWidth+2]-metaData[i][2], 2) + 
											pow(data[i][j*dataWidth+3]-metaData[i][3], 2);
						n = k;
					}
				}
				if(n != i)
				{
					c++;
				}
			}
		}
	}

	int total = 0;
	for(int i=0; i < channels[0]*channels[1]; i++)
	{
		total += data[i].size();
	}
	cout << "error rate:" << (double)c /total << endl;
}

void DataCleaner :: writePredicotModel(char* filePath)
{
	ofstream file(filePath);
	if(file.fail())
	{
		return;
	}

	file << channels[0] << ' ' << channels[1] << endl;
	file << screenSize[0] << ' ' << screenSize[1] << endl;
	file << dataWidth - 2 << endl;

	for(int i=0; i < channels[0] * channels[1]; i++)
	{
		for(int j=0; j < 4; j++)
		{
			file << metaData[i][j] << ' ';
		}
		file << endl;
	}
	file.close();
}