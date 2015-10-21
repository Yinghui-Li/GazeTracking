/*
	Author : Li Yinghui
	Data   : 2015.10.06
*/

#ifndef DATACLEANER_H
#define DATACLEANER_H

#include <vector>
using namespace std;

//Class DataCleaner reads data from files and clean the wrong data
class DataCleaner
{

public:
	bool read(char* filePath);
	bool readNew(char* filePath);
	bool write(char* filePath);
	void clean();
	vector<int>* getData();
	vector<double>* getMetaData();
	int getDataSize();
	int getMetaDataSize();
	int getDataWidth();
	void cvtStd(); // 将坐标转换为标准形式
	DataCleaner();
	~DataCleaner();
	void predictTest(int mark); // 精度预测测试
	void writePredicotModel(char* filePath); //输出平均值

protected:
	const int inputDataWidth; //gaze tracking记录的单条数据的长度，默认为9
	const int dataWidth; //单条数据长度，默认为6
	const int metaDataWidth; 
	int channels[2]; //the numbers of which x and y axis are divided into
	int screenSize[2];
	const int factor;
	vector<int> *data;//单条数据格式为：[leftEyeX, leftEyeY, rightEyeX, rightEyeY, X, Y]
	vector<double> *metaData; //单个集合的数据属性，分别为六条data数据的均值、方差、下限和上限

public:
	void calMetaData(); //计算数据属性
	void cleanOnce();

private:
	void calAverage(int x);
	void calSquare(int x);
	void calBoundary(int x);
};

#endif