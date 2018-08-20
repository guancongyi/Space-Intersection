#ifndef PREP
#define PREP

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <opencv2/nonfree/nonfree.hpp>
#include "opencv2/legacy/legacy.hpp" 
#include <opencv2/gpu/gpu.hpp>
#include<fstream>
#include<iostream>
#include <iomanip>
#include <map>
#include <utility>
#include<time.h>
#include<string>

using namespace cv;
using namespace std;
using namespace gpu;

class preprocess
{
private:
	Mat img1, img2;
	int dim = 2;
	vector<vector<Mat>> ROIs2 = vector<vector<Mat>>(dim, vector<Mat>(dim));
	vector<vector<Mat>> ROIs1 = vector<vector<Mat>>(dim, vector<Mat>(dim)); // 4*4 Matrix

	int width, height;

public:
	preprocess();
	preprocess(string img1addr, string img2addr);
	Mat getImg(int id);

	void cut();
	void display();
	vector<vector<Mat>> getMatrix(int id);


};


#endif // !PREP



