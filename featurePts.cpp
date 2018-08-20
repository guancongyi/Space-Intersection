#include "featurePts.h"



featurePts::featurePts() {}

void featurePts::add(KeyPoint k, Mat d)
{

	this->k.push_back(k);
	this->d.push_back(d);
	pair<double, double> temp(k.pt.x, k.pt.y);
	m[temp] = d;
}

void featurePts::mergeAll()
{
	descrptor = d[0];
	for (size_t i = 1; i < k.size(); i++)
	{
		Mat thisone = d[i];
		descrptor = mergeMat(descrptor, thisone);
	}
}

Mat featurePts::getDescrptorResult()
{
	return descrptor;
}

Mat featurePts::getSingleDescrptor(KeyPoint k)
{
	map<std::pair<double, double>, Mat>::iterator it;
	for (it = m.begin(); it != m.end(); it++) {
		if (it->first.first == k.pt.x && it->first.second == k.pt.y) {
			return it->second;
		}
	}
}

vector<KeyPoint>& featurePts::getKeyPtsResult()
{
	return k;
}

Mat featurePts::mergeMat(Mat& m1, Mat& m2)
{
	if (m1.cols != m2.cols&&m1.type() != m2.type())
	{
		cout << "failed" << endl;
		return Mat();
	}
	else
	{
		int totalrows = m1.rows + m2.rows;
		Mat merged(totalrows, m1.cols, m1.type());
		Mat temp = merged.rowRange(0, m1.rows);
		m1.copyTo(temp);
		temp = merged.rowRange(m1.rows, totalrows);
		m2.copyTo(temp);
		return merged;
	}
}

