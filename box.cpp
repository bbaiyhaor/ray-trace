#include "box.h"
#include "sphere.h"
#include "light.h"
#include "triangle.h"

const AabbBox AabbBox::getAabb(const Scene::Sphere& qiu)
{
	Vec3 size(qiu.getR(), qiu.getR(), qiu.getR());
	return AabbBox(qiu.getCenter() - size, qiu.getCenter() + size, 2.0 * size);
}

const AabbBox AabbBox::getAabb(const Scene::Plane& mian)
{
	return AabbBox(Vec3(-10000.0, -10000.0, -10000.0), Vec3(10000.0, 10000.0, 10000.0), Vec3(20000.0, 20000.0, 20000.0));
}

const AabbBox AabbBox::getAabb(const Scene::PointLight& light)
{
	return getAabb(light.get());
}

const AabbBox AabbBox::getAabb(const Scene::AreaLight& light)
{
	return light.get();
}

const AabbBox AabbBox::getAabb(const Scene::Triangle& tri)
{
	double minx = INF, maxx = -INF;
	double miny = INF, maxy = -INF;
	double minz = INF, maxz = -INF;
	for (int i = 0; i < 3; i++){
		minx = min(minx, tri.vertex[i].x);
		maxx = max(maxx, tri.vertex[i].x);
		miny = min(miny, tri.vertex[i].y);
		maxy = max(maxy, tri.vertex[i].y);
		minz = min(minz, tri.vertex[i].z);
		maxz = max(maxz, tri.vertex[i].z);
	}
	return AabbBox(Vec3(minx, miny, minz), Vec3(maxx, maxy, maxz));
}