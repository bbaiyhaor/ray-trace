#ifndef SPHERE_H_
#define SPHERE_H_
#include "scene.h"
#include "myvector.h"
#include "ray.h"
#include "intersectresult.h"
#include "material.h"
#include "primitive.h"
#include "global.h"
#include "box.h"
#include <cmath>
//#define DEBUG
#include "debug.h"
class Scene::Sphere:public Scene::Primitive
{
	#define sqr(x) (x * x)
public:
	bool intersectAabb(const AabbBox& box) const
	{
		double dist = 0.0;
		//	judge whether or not intersection is existed
		//	by cross section
		if (center.x < box.getMin().x)
			dist = dist + sqr(center.x - box.getMin().x);
		else if (center.x > box.getMax().x)
			dist = dist + sqr(center.x - box.getMax().x);

		if (center.y < box.getMin().y)
			dist = dist + sqr(center.y - box.getMin().y);
		else if (center.y > box.getMax().y)
			dist = dist + sqr(center.y - box.getMax().y);

		if (center.z < box.getMin().z)
			dist = dist + sqr(center.z - box.getMin().z);
		else if (center.z > box.getMax().z)
			dist = dist + sqr(center.z - box.getMax().z);		
		return dist <= sqrR;
	}

	int intersect(const Ray& ray, IntersectResult& result)
	{
		this->lastId() = ray.getId();
		//	|| o + t*d - c|| = r
		//	|| v + t*d|| = r
		//	t = - dot(d, v) +- sqrt( dot(d,v)^2 - (||v|| - r^2) )
		//	dot(d,v)^2 - (||v|| - r^2) < 0 then no intersection
		double x1, x2;
		Vec3 v = center - ray.o;
		double judge = dot(v, ray.d);	
		double judge1 = sqr(judge) - dot(v, v) + sqrR;//sqr(radius);
		if (judge1 >= 0){
			judge1 = sqrt(judge1);
			x1 = judge + judge1;
			x2 = judge - judge1;
			//	the origin of ray is outside
			if (x2 > 0){
				if (x2 < result.dist){
					result.inter = ray.o + x2 * ray.d;
					result.dist = x2;
					result.fxl = result.inter - center;
					Vec3::normalize(result.fxl);
					result.pri = this;
					return 1;
				}
			}
			//	the origin of ray is inside
			else if (x1 > 0){
				if (x1 < result.dist){
					result.inter = ray.o + x1 * ray.d;
					result.dist = x1;
					result.fxl = result.inter - center;
					Vec3::normalize(result.fxl);
					result.pri = this;	
					return -1;
				}
			}
		}		
		return 0;
	}

	void uvTransfer(const Vec3& po, double& x, double& y) const
	{
		Vec3 relative = po - center;
		x = acos(relative.z / radius);
		y = acos(relative.x / (radius * sin(x)));
		if (relative.y < 0)
			y += MYPI;
		
//		x = x * 2.0 * sqrR;
//		y = y * sqrR;
		x = x * 20.0;
		y = y * 10;
	}

	const Material* material() const{	return ma;	}
	const Vec3& getCenter() const{	return center;	}
	const double getR() const {	return radius;	}
	const double getR2() const {	return sqrR;	}
	//	constructor
	Sphere():
	Primitive(0), center(ZERO3), radius(0), ma(0){	init();	}
	Sphere(const Vec3& center, double radius, Material* ma = 0):
	Primitive(0), center(center), radius(radius), ma(ma){	init();	}

private:
	
	void init(){	sqrR = sqr(radius);	}
	Vec3 center;
	double radius, sqrR;
	Material* ma;
};
#endif