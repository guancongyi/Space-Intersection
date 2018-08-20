#include "calculation.h"
#include <math.h>


void calculation::calculateR()
{
	a1 = cos(phi)*cos(kappa) - sin(phi)*sin(omega)*sin(kappa);
	a2 = -cos(phi)*sin(kappa) - sin(phi)*sin(omega)*cos(kappa);
	a3 = -sin(phi)*cos(omega);
	b1 = cos(omega)*sin(kappa);
	b2 = cos(omega)*cos(kappa);
	b3 = -sin(omega);
	c1 = sin(phi)*cos(kappa) + cos(phi)*sin(omega)*sin(kappa);
	c2 = -sin(phi)*sin(kappa) + cos(phi)*sin(omega)*cos(kappa);
	c3 = cos(phi)*cos(omega);
}

void calculation::calculateL()
{
	l1 = f*a1 + miu*(x - X0)*a3;
	l2 = f*b1 + miu*(x - X0)*b3;
	l3 = f*c1 + miu*(x - X0)*c3;
	l4 = f*a2 + miu*(y - Y0)*a3;
	l5 = f*b2 + miu*(y - Y0)*b3;
	l6 = f*c2 + miu*(y - Y0)*c3;
	lx = f*(a1*Xs + b1*Ys + c1*Zs) + (x - X0)*a3*Xs*miu + (x - X0)*b3*Ys*miu + (x - X0)*c3*Zs*miu;
	ly = f*(a2*Xs + b2*Ys + c2*Zs) + (y - Y0)*a3*Xs*miu + (y - Y0)*b3*Ys*miu + (y - Y0)*c3*Zs*miu;
}

void calculation::Gaussin(double a[3][3], double b[3])
{
	//判断能否用高斯消元法，如果矩阵主对角线上有0元素存在是不能用的
	for (int i = 0; i < 3; i++)
		if (a[i][i] == 0)
		{
			cout << "can't use gaussin meathod" << endl;
			return;
		}

	int i, j, k;
	double c[3];    //存储初等行变换的系数，用于行的相减
					//消元的整个过程如下，总共n-1次消元过程。
	for (k = 0; k < 3 - 1; k++)
	{
		//求出第K次初等行变换的系数
		for (i = k + 1; i < 3; i++)
			c[i] = a[i][k] / a[k][k];

		//第K次的消元计算
		for (i = k + 1; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				a[i][j] = a[i][j] - c[i] * a[k][j];
			}
			b[i] = b[i] - c[i] * b[k];
		}
	}

	//解的存储数组
	double x[3];
	//先计算出最后一个未知数；
	x[3 - 1] = b[3 - 1] / a[3 - 1][3 - 1];
	//求出每个未知数的值
	for (i = 3 - 2; i >= 0; i--)
	{
		double sum = 0;
		for (j = i + 1; j < 3; j++)
		{
			sum += a[i][j] * x[j];
		}
		x[i] = (b[i] - sum) / a[i][i];
	}

	Mresult[0] = x[0];
	Mresult[1] = x[1];
	Mresult[2] = x[2];
}

calculation::calculation(double x, double y, double phi, double omega, double kappa, double Xs, double Ys, double Zs, double X0, double Y0, double f, double ss, int width) {
	this->phi = phi; this->kappa = kappa; this->omega = omega;
	this->y = y; this->x = x;
	this->f = f; this->X0 = X0; this->Y0 = Y0;
	this->Xs = Xs; this->Ys = Ys; this->Zs = Zs;
	miu = ss / width;
	calculateR();
	calculateL();
}

calculation::calculation(calculation c1, calculation c2)
{
	M[0][0] = c1.l1; M[0][1] = c1.l2; M[0][2] = c1.l3; M[1][0] = c1.l4; M[1][1] = c1.l5; M[1][2] = c1.l6; M[2][0] = c2.l1; M[2][1] = c2.l2; M[2][2] = c2.l3;
	double temp[3];
	temp[0] = c1.lx; temp[1] = c1.ly; temp[2] = c2.lx;
	Gaussin(M, temp);

}

calculation::calculation()
{
}

vector<double> calculation::getLocation()
{
	return Mresult;// Y Z X
}



calculation::~calculation()
{
}
