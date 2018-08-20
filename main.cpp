#include "preprocess.h" 
#include"featurePts.h"
#include"calculation.h"
#include "Smart3DPosReader.h"
#include"BaseType.h"

struct node {
	string imgid;
	double lon, lat, alt;
	double xs, ys, zs, phi, omeg, kappa;
	double x0, y0, f, ss, width;
};

void space_intersect(node n1, node n2);

int main(int argc, const char** argv)
{
	//Smart3DPosReader sr;
	//sr.Open("D:\\FQ2_AT6 - export (2).xml");

	//get CUDA info
	gpu::DeviceInfo d;
	bool flag = d.isCompatible();
	int iDevicesNum = getCudaEnabledDeviceCount();
	cout << "device Count = " << iDevicesNum << endl;
	cout << "capatible with " << d.name() << " = " << flag << endl;

	//读excel表格 可更改 ********************************************
	ifstream in("\excel\\test.xls");
	vector <node> l;
	while (!in.eof()) 
	{
		
		node n;
		in >> n.imgid;
		if (n.imgid == "") break;
		in >> n.lon >> n.lat >> n.alt >> n.phi >> n.omeg >> n.kappa >> n.xs >> n.ys >> n.zs;
		//设置内方位元素 sensorsize 宽度
		n.x0 = 5804.74716296202; n.y0 = 4356.16961092119; n.f = 41.811101873344; n.ss = 53.3968; n.width = 11608;
		l.push_back(n);
	}

	for (size_t i = 0; i < l.size(); i++)
	{
		node first = l.at(i);
		if (i == l.size() - 2) 
		{
			node second = l.at(i + 1);
			//第一个和第二个比
			space_intersect(first, second);
			break;
		}
		else
		{
			node second = l.at(i + 1);
			node third = l.at(i + 2);
			//第一个和第二个比
			space_intersect(first, second);
			//第一个和第三个比
			space_intersect(first, third);
		}
		
	}
	return 0;
}

void space_intersect(node n1, node n2)
{
	//更改此处 图片路径 和 结果路径 ****************************************
	cout << n1.imgid << "---" << n2.imgid << endl;
	string path1 = "G:\\E\\" + n1.imgid + ".tif";
	string path2 = "G:\\E\\" + n2.imgid + ".tif";
	string outfn = n1.imgid + "_" + n2.imgid;
	string outpath = "\output\\E_result\\" + outfn+".txt";

	double xs1 = n1.xs, ys1 = n1.ys, zs1 = n1.zs;
	double xs2 = n2.xs, ys2 = n2.ys, zs2 = n2.zs;
	double phi1 = n1.phi, omeg1 = n1.omeg, kappa1 = n1.kappa;
	double phi2 = n2.phi, omeg2 = n2.omeg, kappa2 = n2.kappa;
	double x0 = n1.x0, y0 = n1.y0, f = n1.f;
	double ss = n1.ss;
	double width = n1.width;

	const int test = 10;
	const double PI = 3.14;
	ofstream out(outpath);

	const int dim = 2;
	clock_t t1 = clock();

	// read imgs and preprocess 读取 分割
	preprocess p(path1, path2);
	//preprocess p(argv[1], argv[2]);
	p.cut();
	vector<vector<Mat>> ROI1 = p.getMatrix(1);
	vector<vector<Mat>> ROI2 = p.getMatrix(2);

	featurePts fpts1, fpts2;
	vector<KeyPoint> keypt1, keypt2;


	for (size_t i = 0; i < dim; i++)
	{
		for (size_t j = 0; j < dim; j++)
		{
			Mat img1 = ROI1[i][j];
			Mat img2 = ROI2[i][j];
			GpuMat gimg1(img1);
			GpuMat gimg2(img2);

			////提取特征点
			////提取特征点的特征向量
			FAST_GPU fg(3000, true, 0.07);
			GpuMat descrptGpu1, descrptGpu2;
			fg(gimg1, GpuMat(), keypt1);
			fg(gimg2, GpuMat(), keypt2);

			ORB_GPU orb_gpu(1000, 0.9f, 8, 31, 1, 2, 0, 31);
			orb_gpu(gimg1, GpuMat(), keypt1, descrptGpu1);
			orb_gpu(gimg2, GpuMat(), keypt2, descrptGpu2);

			////存储 pair of keypoints & descriptor
			Mat tempmat;
			for (size_t k = 0; k < keypt1.size(); k++)
			{
				keypt1[k].pt.x = keypt1[k].pt.x + j*(p.getImg(1).cols / dim);
				keypt1[k].pt.y = keypt1[k].pt.y + i*(p.getImg(1).rows / dim);

				descrptGpu1.download(tempmat);
				Mat temprow = tempmat.rowRange(k, k + 1).clone();
				fpts1.add(keypt1[k], temprow);
			}
			for (size_t k = 0; k < keypt2.size(); k++)
			{
				keypt2[k].pt.x = keypt2[k].pt.x + j*(p.getImg(2).cols / dim);
				keypt2[k].pt.y = keypt2[k].pt.y + i*(p.getImg(2).rows / dim);

				descrptGpu2.download(tempmat);
				Mat temprow = tempmat.rowRange(k, k + 1).clone();
				fpts2.add(keypt2[k], temprow);
			}

		}
	}

	//匹配
	BruteForceMatcher_GPU_base bfm(cv::gpu::BruteForceMatcher_GPU_base::DistType::HammingDist);
	vector<DMatch> matches;
	vector<KeyPoint> k1, k2;
	fpts1.mergeAll();//concatenate bunch of descriptors into one
	fpts2.mergeAll();
	k1 = fpts1.getKeyPtsResult();
	k2 = fpts2.getKeyPtsResult();

	GpuMat trainIdx, distance;
	GpuMat d1(fpts1.getDescrptorResult());
	GpuMat d2(fpts2.getDescrptorResult());
	bfm.matchSingle(d1, d2, trainIdx, distance);
	bfm.matchDownload(trainIdx, distance, matches);

	////RANSAC
	//rkeypoint 储存匹配好的keypoints对 
	vector<KeyPoint> rkeypt1, rkeypt2;
	for (size_t i = 0; i < matches.size(); i++)
	{
		rkeypt1.push_back(k1[matches[i].queryIdx]);
		rkeypt2.push_back(k2[matches[i].trainIdx]);
	}

	vector<Point2f> pts1, pts2;
	for (size_t i = 0; i < matches.size(); i++)
	{
		pts1.push_back(rkeypt1[i].pt);
		pts2.push_back(rkeypt2[i].pt);
	}

	vector<uchar> RansacStatus;
	Mat Fund = findFundamentalMat(pts1, pts2, RansacStatus, CV_FM_RANSAC, 3, 0.99);

	////update old matches and create new keypoint sets
	vector<KeyPoint> rrkeypt1, rrkeypt2;
	vector<DMatch> rmatches;
	int index = 0;
	for (size_t i = 0; i < matches.size(); i++)
	{
		if (RansacStatus[i] != 0) {
			rrkeypt1.push_back(rkeypt1[i]);
			rrkeypt2.push_back(rkeypt2[i]);

			matches[i].queryIdx = index;
			matches[i].trainIdx = index;
			rmatches.push_back(matches[i]);
			index++;
		}
	}

	Mat r_match_img;
	drawMatches(p.getImg(1), rrkeypt1, p.getImg(2), rrkeypt2, rmatches, r_match_img);//这一步花费很多时间

	//custom draw matches
	RNG rng(0xFFFFFFFF);
	for (size_t i = 0; i < rrkeypt2.size(); i++)
	{
		int icolor = (unsigned)rng;
		Scalar s = Scalar(icolor & 255, (icolor >> 8) & 255, (icolor >> 16) & 255);

		rrkeypt2[i].pt.x = rrkeypt2[i].pt.x + p.getImg(1).cols;

		if (i == test) {
			//for test only
			circle(r_match_img, rrkeypt1[test].pt, 200, Scalar(0, 0, 0), -1, 8);
			circle(r_match_img, rrkeypt2[test].pt, 200, Scalar(0, 0, 0), -1, 8);
		}
		else {
			circle(r_match_img, rrkeypt1[i].pt, 100, s, -1, 8);
			circle(r_match_img, rrkeypt2[i].pt, 100, s, -1, 8);
		}
		line(r_match_img, rrkeypt1[i].pt, rrkeypt2[i].pt, s, 15, CV_AA);
	}


	resize(r_match_img, r_match_img, Size(r_match_img.cols / 12, r_match_img.rows / 12), 0, 0, INTER_LINEAR);
	imshow("with RANSAC", r_match_img);


	//计算
	vector<double> result;
	out << "\t[img1_x,  img1_y] \t\t [img2_x, img2_y]  \t\t\t  X  \t\t\t  Y  \t\t  Z   \t\t  descriptor1  \t\t   descriptor2 " << endl;
	for (size_t i = 0; i < rmatches.size(); i++)
	{
		calculation c1(rrkeypt1[i].pt.x, rrkeypt1[i].pt.y, phi1*PI / 180, omeg1*PI / 180, kappa1*PI / 180,
			xs1, ys1, zs1,
			x0, y0, f,
			ss, width);

		calculation c2(rrkeypt2[i].pt.x, rrkeypt2[i].pt.y, phi2*PI / 180, omeg2*PI / 180, kappa2*PI / 180,
			xs2, ys2, zs2,
			x0, y0, f,
			ss, width);

		calculation c(c1, c2);
		result = c.getLocation();

		//导出数据
		out.setf(ios_base::fixed);
		out.setf(std::ios::left);
		rrkeypt2[i].pt.x = rrkeypt2[i].pt.x - p.getImg(1).cols;
		out << rrkeypt1[i].pt << " " << rrkeypt2[i].pt << " " << result[0] << " " << result[1] << " " << result[2];
		Mat d1_temp = fpts1.getSingleDescrptor(rrkeypt1[i]);
		Mat d2_temp = fpts2.getSingleDescrptor(rrkeypt2[i]);
		//1*32pixel, each pixel is 8 bits(1 byte),
		//read 32 bytes in total
		for (int r = 0; r < d1_temp.rows; r++)
			for (int c = 0; c < d1_temp.cols; c++)
				out << d1_temp.at<char>(r, c);

		for (int r = 0; r < d2_temp.rows; r++)
			for (int c = 0; c < d2_temp.cols; c++)
				out << d2_temp.at<char>(r, c);

		out << endl;
	}

	out.close();

	clock_t t2 = clock();
	cout << "time: " << (t2 - t1) / 1000 << "s" << endl;


	//waitKey(0);
}


/*ifstream in(argv[3]);
Mat img1(1, 32, CV_8UC1);
for (size_t i = 0; i < 1; i++)
{
for (size_t j = 0; j < 32; j++)
{
char temp;
in >> temp;
img1.at<char>(i, j) = temp;
}
}

imshow("read in", img1);*/