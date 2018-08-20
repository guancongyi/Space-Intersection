#pragma once
#include"preprocess.h"



class featurePts
{
private:
	vector<KeyPoint> k;
	vector<Mat> d;
	map<pair<double, double>, Mat> m;
	Mat descrptor;

	Mat mergeMat(Mat& m1, Mat& m2);

public:
	featurePts();
	void add(KeyPoint k, Mat d);
	void mergeAll();

	Mat getDescrptorResult();
	vector<KeyPoint> &getKeyPtsResult();

	Mat getSingleDescrptor(KeyPoint k);



};

