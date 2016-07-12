#include "debug.h"
#include "mymat.h"
#include <iostream>
using namespace std;
ostream &operator<<(ostream &os, const Vec3 &m) { 
		return os << m.x << " " << m.y << " " << m.z;
	}
ostream &operator<<(ostream &os, const Ray &m) { 
		return os << "origin: " << m.o.x << " " << m.o.y << " " << m.o.z
		<< " dire: " << m.d.x << " " << m.d.y << " " << m.d.z ;
	}
#ifdef DF
ostream &operator<<(ostream &os, const MyMat &m) {
	return os << "mymat:" << endl << m.mat;
}
#endif