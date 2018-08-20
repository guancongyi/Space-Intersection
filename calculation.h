#pragma once
#include"preprocess.h"

class calculation
{
private:
	double x, y;
	double phi, omega, kappa; //外方为元素
	double Xs, Ys, Zs;
	double X0, Y0, f; //内方位元素
	double longtitude, latitude, altitude;
	double miu;

	double M[3][3];
	vector<double> Mresult = vector<double>(3, double(1));
	double coeff[3][3];

	void calculateR();
	void calculateL();
	void Gaussin(double a[3][3], double b[3]);

public:
	double a1, a2, a3, b1, b2, b3, c1, c2, c3;
	double l1, l2, l3, l4, l5, l6, lx, ly;
	calculation();
	calculation(double x, double y, double phi, double omega, double kappa, double Xs, double Ys, double Zs, double X0, double Y0, double f, double ss, int width);
	calculation(calculation c1, calculation c2);
	vector<double> getLocation();

	~calculation();
};

