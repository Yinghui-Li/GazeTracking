#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <vector>
using namespace std;
class Predictor
{
public:
	Predictor(){};
	~Predictor(){};
	bool load(char* filename);
	bool insertTrainData(vector<int> data);
	vector<int> predict(vector<int> data);
	bool isTrained();
	vector<int> getChannels();
	vector<int> getScreenSize(); 
	void logInfo(ofstream& log);

protected:
	int channels[2]; //the numbers of which x and y axis are divided into
	int screenSize[2];
	int dataWidth;
	vector<double> *metaData;
	vector<int> trainData;
	double *bias;	//Æ«ÒÆÁ¿

protected:
	double getLength(vector<int>& data1, vector<double>& data2);
};

#endif