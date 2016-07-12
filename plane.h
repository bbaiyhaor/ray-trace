 #ifndef PLANE_H_
#define PLANE_H_
#include "scene.h"
#include "myvector.h"
#include "ray.h"
#include "intersectresult.h"
#include "material.h"
#include "global.h"
#include "primitive.h"
#include "box.h"
#include <cstdlib>
//#define DEBUG
#ifdef DEBUG
#include "debug.h"
#endif
class Scene::Plane:public Scene::Primitive
{
public:
	bool intersectAabb(const AabbBox& box) const
	{
		Vec3 v[2];
		v[0] = box.getMin();
		v[1] = box.getMax();
		//	the number of vertexs at the side of plane
		int side1 = 0, side2 = 0;
		//	the eight vertex of box
		for (int i = 0; i < 8; i++){
			Vec3 p(v[i & 1].x, v[(i >> 1) & 1].y, v[(i >> 2) & 1].z);
			if ((dot(p, fxl) + od) < 0)
				side1++;
			else
				side2++;
			if (side1 && side2)
				return 1;
		}
		return 0;
	}

	//	cosA = b / c 
	//=>c = b / cosA
	int intersect(const Ray& ray, IntersectResult& result)
	{
		double cosA = -dot(fxl, ray.d);	
		if (cosA > 0){
			double delta = dot(fxl, ray.o);
			double b = od + delta;
			double dist = b / cosA;
			if (dist < result.dist){
				result.dist = dist;
				result.inter = ray.o + result.dist * ray.d;
				result.fxl = fxl;
				result.pri = this;
				return 1;
			}
		}
		return 0;
	}

	void uvTransfer(const Vec3& po, double& x, double& y) const
	{
		Vec3 newPo = po - uvo;
		x = dot(newPo, uBase);
		y = dot(newPo, vBase);
	}

	const Material* material() const{	return ma;	}
	//	constructor
	Plane():
	Primitive(2), fxl(YPLUS), od(0), ma(0){	initial();	}
	Plane(const Vec3& fxl, double od):
	Primitive(2), fxl(fxl), od(od), ma(0){	initial();	}
	Plane(const Vec3& fxl, double od, Material* m):
	Primitive(2), fxl(fxl), od(od), ma(m){	initial();	}

private:
	void initial()
	{
		uvo = od * fxl;
		Vec3 casual(fxl.x + std::rand(), fxl.y + std::rand(), fxl.z + std::rand());
		uBase = cross(fxl, casual);
		vBase = cross(fxl, uBase);	
		Vec3::normalize(uBase);
		Vec3::normalize(vBase);	
	}
	//	normal and origin in uv system
	//	the unit base vector of uv system
	Vec3 fxl, uvo, uBase, vBase;
	//	the distance from origin to plane
	double od;
	Material* ma;
};
#endif