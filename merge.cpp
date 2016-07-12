#include "opencv.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
using namespace std;
using namespace cv;
int main()
{
	Mat mat(600, 600, CV_8UC3, Scalar::all(0));
	Mat a = imread("fourth.png");
	Mat b = imread("awe.png");
	for (int r = 0; r < 600; r++)
		for (int c = 0; c < 310; c++)
				mat.at<Vec3b>(r, c) = a.at<Vec3b>(r, c);
	for (int r = 0; r < 600; r++)
		for (int c = 310; c < 600; c++)
				mat.at<Vec3b>(r, c) = b.at<Vec3b>(r, c);
	imwrite("hehewokao.png", mat);
	return 0;
}