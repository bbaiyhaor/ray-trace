#ifndef BOX_H_
#define BOX_H_
#include "myvector.h"
#include "global.h"
#include "scene.h"
#include "intersectresult.h"
#include "ray.h"

class Box
{
public:
	virtual bool intersect(const Box&) = 0;
	virtual bool contain(const Vec3&) = 0;
	const int lastRay() const
	{
		return id;
	};
	int& lastId() {	return id;	};
private:
	int id;
};

class AabbBox:public Box
{
public:
	AabbBox():
	lmin(ZERO3), rmax(ZERO3), l2r(ZERO3){}
	AabbBox(const Vec3& lmin, const Vec3& rmax):
	lmin(lmin), rmax(rmax)
	{
		l2r = rmax - lmin;
	}
	AabbBox(const Vec3& lmin, const Vec3& rmax, const Vec3& l2r):
	lmin(lmin), rmax(rmax), l2r(l2r){}

	int intersect(const Ray& ray, IntersectResult& result)
	{
		this->lastId() = ray.getId();
		double dist[6];
		for (int i = 0; i < 6; i++)
			dist[i] = -1;
		//	arrive x axis firstly
		if (ray.d.x != 0){
			dist[0] = (lmin.x - ray.o.x) / ray.d.x;
			dist[3] = (rmax.x - ray.o.x) / ray.d.x;
		}
		//	arrive y axis firstly
		if (ray.d.y != 0){
			dist[1] = (lmin.y - ray.o.y) / ray.d.y;
			dist[4] = (rmax.y - ray.o.y) / ray.d.y;
		}
		//	arrive z axis firstly
		if (ray.d.z != 0){
			dist[2] = (lmin.z - ray.o.z) / ray.d.z;
			dist[5] = (rmax.z - ray.o.z) / ray.d.z;
		}
		int re = 0;
		for (int i = 0; i < 6; i++)
			if ((dist[i] > 0) && (dist[i] < result.dist)){
				Vec3 arrive = ray.o + dist[i] * ray.d;
				if (contain(arrive)){
					result.dist = dist[i];
					result.inter = arrive;
					re = 1;
				}
			}
		return re;
	}

	bool intersect(const Box& ano)
	{
		return intersectAabb(dynamic_cast<AabbBox&>(const_cast<Box&>(ano)));
	}

	bool intersectAabb(const AabbBox& ano) const
	{
		return ((rmax.x > ano.getMin().x) && (lmin.x < ano.getMax().x)	//	x-axis overlap
			&& (rmax.y > ano.getMin().y) && (lmin.y < ano.getMax().y)	//	y-axis overlap
			&& (rmax.z > ano.getMin().z) && (lmin.z < ano.getMax().z));	//	z-axis overlap
	}
	
	bool contain(const Vec3& p)
	{
		return ((p.x > (lmin.x - DEVIATION)) && (p.x < (rmax.x + DEVIATION))
			&& (p.y > (lmin.y - DEVIATION)) && (p.y < (rmax.y + DEVIATION))
			&& (p.z > (lmin.z - DEVIATION)) && (p.z < (rmax.z + DEVIATION)));
	}

	static const AabbBox getAabb(const Scene::Sphere&);
	static const AabbBox getAabb(const Scene::Plane&);
	static const AabbBox getAabb(const Scene::PointLight&);
	static const AabbBox getAabb(const Scene::AreaLight&);
	static const AabbBox getAabb(const Scene::Triangle&);

	const Vec3& getMin() const {	return lmin;	}
	const Vec3& getMax() const {	return rmax;	}
	const Vec3& getSize() const {	return l2r;		}
	Vec3& setMin() {	return lmin;	}
	Vec3& setMax() {	return rmax;	}
	Vec3& setSize() {	return l2r;		}
	
private:
	Vec3 lmin, rmax, l2r;
};

#endif