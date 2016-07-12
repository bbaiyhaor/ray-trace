#ifndef INTERSECTRESULT_H_
#define INTERSECTRESULT_H_

#include "myvector.h"
#include "scene.h"
#include "global.h"

class IntersectResult
{
public:
	//	intersection and normal vector
	IntersectResult():
	inter(ZERO3), fxl(ZERO3), dist(INF), pri(0){}
	Vec3 inter, fxl;
	const Scene::Primitive* pri;
	double dist;
};
#endif