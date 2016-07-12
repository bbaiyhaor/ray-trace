#ifndef TEXTURE_H_
#define TEXTURE_H_
#include "opencv.h"
//#define DEBUG
#ifdef DEBUG
#include "debug.h"
#endif
class Texture
{
	typedef cv::Mat Mat;
	typedef cv::Vec3d Vec3d;
public:
	Texture(const char* filename = 0)
	{
		if (filename != 0)
			loadFile(filename);
	}
	void loadFile(const char* filename)
	{
		mat = cv::imread(filename);
		mat.convertTo(mat, CV_64FC3, 1.0 / 255.0);
	//	rowScale = mat.rows / 10.0;
	//	colScale = mat.cols / 10.0;
		rowScale = mat.rows / 100.0;
		colScale = mat.cols / 100.0;	
	}

	const Color getColor(double X, double Y) const
	{
		//	ensure the coordinates are positive
		double x = (X + 1e5) * colScale;
		double y = (Y + 1e5) * rowScale;
		int u1 = (unsigned int)x % mat.cols;
		int v1 = (unsigned int)y % mat.rows;
		int u2 = (unsigned int)(x + 1) % mat.cols;
		int v2 = (unsigned int)(y + 1) % mat.rows;
		//	bilinear interpolation method
		assert(u1 > -1);
		assert(v1 > -1);
		assert(u2 > -1);
		assert(v2 > -1);
		double deltaU = x - floor(x);
		double deltaV = y - floor(y);
		double w1 = (1 - deltaU) * (1 - deltaV);
		double w2 = deltaU * (1 - deltaV);
		double w3 = (1 - deltaU) * deltaV;
		double w4 = deltaU * deltaV;
		Vec3d c = w1 * mat.at<Vec3d>(v1, u1) +
				w2 * mat.at<Vec3d>(v1, u2) +
				w3 * mat.at<Vec3d>(v2, u1) +
				w4 * mat.at<Vec3d>(v2, u2);
		return Color(c[2], c[1], c[0]);
	}
private:
	Mat mat;
	double colScale, rowScale;
};
#endif