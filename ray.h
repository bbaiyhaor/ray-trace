#ifndef RAY_H_
#define RAY_H_
#include "myvector.h"
#include "global.h"

class Ray
{
public:
	//	constructor
	Ray():
	o(ZERO3), d(ZERO3)
	{	
		id = population++;
	}

	Ray(const Vec3& o, const Vec3& d):
	o(o), d(d)
	{
		id = population++;
	}

	Vec3 o, d;
	const int getId() const {	return id;	}
	static int getPopulation();
private:
	int id;
	static int population;
};
#endif