#ifndef GLOBAL_H_
#define GLOBAL_H_
#include "myvector.h"

const Vec3 ZERO3(0.0, 0.0, 0.0);
const Vec3 XPLUS(1.0, 0.0, 0.0);
const Vec3 YPLUS(0.0, 1.0, 0.0);
const Vec3 ZPLUS(0.0, 0.0, 1.0);

const int MISS = 0;
const int HIT = 1;
const int SHADOWSAMPLETIME = 128 * 500;
const int GLOSSYSAMPLETIME = 128;
const int TRACEMAXRECUR = 5;

const double DEVIATION = 1e-6;
const double MYPI = 3.14159265358979323846;
const double MY_1_PI = 1 / MYPI;
const double INF = 1e18;
const double SHADOWSAMPLEDOWN = 1.0 / SHADOWSAMPLETIME;
const double GLOSSYSAMPLEDOWN = 1.0 / GLOSSYSAMPLETIME;

const Color black = Color(0, 0, 0);
const Color white = Color(1, 1, 1);
const Color red = Color(1, 0, 0);
const Color green = Color(0, 1, 0);
const Color blue = Color(0, 0, 1);

#endif
