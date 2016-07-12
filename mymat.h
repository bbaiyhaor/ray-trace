#ifndef MYMAT_H_
#define MYMAT_H_
#include "opencv.h"
//#define DEBUG
//#define DF
#ifdef DEBUG
#include "debug.h"
#endif
class MyMat
{
	//	blue green red
	typedef cv::Mat Mat;
	typedef cv::Vec3d Vec3d;
	typedef cv::Scalar Scalar;
public:
	MyMat(int r, int c):
	mat(r, c, CV_64FC3, Scalar::all(0)) {}
	int row() const {	return mat.rows;	}
	int col() const {	return mat.cols;	}
	Vec3d& pixel(int r, int c){	return mat.at<Vec3d>(r, c);	}
	
	void show(const char* name){	imshow(name, mat);	}
	void waitKey(int t) {	cv::waitKey(t);	}
	void save(const char* name)
	{	
		//	convert CV_64FC3 to CV_8UC3
		//	void Mat::convertTo(OutputArray m,int rtype,double alpha=1,double beta=0)const
		//	m( x;y) = saturate_cast < rType > ( α*( *this)( x;y) +β)
		Mat a;
		mat.convertTo(a, CV_8UC3, 255);
		imwrite(name, a);	
	}
	#ifdef DF
	friend ostream &operator<<(ostream &os, const MyMat &m) ;
	#endif
private:
	Mat mat;
};
#endif