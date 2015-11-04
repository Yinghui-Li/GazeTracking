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
	void cvtStd(); // ������ת��Ϊ��׼��ʽ
	DataCleaner();
	~DataCleaner();
	void predictTest(int mark); // ����Ԥ�����
	void writeAverage(char* filePath); // ���ƽ��ֵ
	void writeChannelsCenter(char* filePath);	// ���ÿ�С�ÿ�еĻ��ֵ�ƽ��ֵ

protected:
	const int inputDataWidth; //gaze tracking��¼�ĵ������ݵĳ��ȣ�Ĭ��Ϊ9
	const int dataWidth; //�������ݳ��ȣ�Ĭ��Ϊ6
	const int metaDataWidth; 
	int channels[2]; //the numbers of which x and y axis are divided into
	int screenSize[2];
	const int factor;
	vector<int> *data;//�������ݸ�ʽΪ��[leftEyeX, leftEyeY, rightEyeX, rightEyeY, X, Y]
	vector<double> *metaData; //�������ϵ��������ԣ��ֱ�Ϊ����data���ݵľ�ֵ��������޺�����

public:
	void calMetaData(); //������������
	void cleanOnce();

private:
	void calAverage(int x);
	void calSquare(int x);
	void calBoundary(int x);
};

#endif