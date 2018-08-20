#include "preprocess.h"

preprocess::preprocess()
{
}

preprocess::preprocess(string img1addr, string img2addr)
{
	this->img1 = imread(img1addr, CV_8UC1);
	this->img2 = imread(img2addr, CV_8UC1);

	//resize(img1, img1, Size(img1.cols / 10, img1.rows / 10), 0, 0, INTER_LINEAR);
	//resize(img2, img2, Size(img2.cols / 10, img2.rows / 10), 0, 0, INTER_LINEAR);

	cout << img1.cols << " x  " << img1.rows << endl;
	width = img1.cols;
	height = img1.rows;

}

Mat preprocess::getImg(int id)
{
	if (id == 1)
	{
		return img1;
	}
	else {
		return img2;
	}

}

void preprocess::cut()
{
	int tempW = 0, tempH = 0;
	for (size_t i = 0; i < dim; i++)
	{
		for (size_t j = 0; j < dim; j++) {
			Mat ROI2 = img2(Rect(tempW, tempH, width / dim, height / dim));
			Mat ROI1 = img1(Rect(tempW, tempH, width / dim, height / dim));
			ROIs1[i][j] = ROI1;
			ROIs2[i][j] = ROI2;
			tempW += width / dim;
		}
		tempW = 0;
		tempH += height / dim;
	}
}

void preprocess::display()
{
	for (size_t i = 0; i < dim; i++)
	{
		for (size_t j = 0; j < dim; j++) {
			string name = to_string(i) + to_string(j);
			imshow(name, ROIs1[i][j]);
		}
	}
}

vector<vector<Mat>> preprocess::getMatrix(int id)
{
	if (id == 1)
	{
		return ROIs1;
	}
	else {
		return ROIs2;
	}
}
