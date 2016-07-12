#ifndef MYDEBUG_H_
#define MYDEBUG_H_

#include "ray.h"
#include "myvector.h"
class MyMat;
#include <iostream>
using namespace std;
ostream &operator<<(ostream &os, const Vec3 &m) ;
ostream &operator<<(ostream &os, const Ray &m) ;
ostream &operator<<(ostream &os, const MyMat &m) ;
#endif