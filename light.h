#ifndef LIGHT_H_
#define LIGHT_H_
#include "scene.h"
#include "sphere.h"
#include "myvector.h"
#include "primitive.h"
#include "box.h"

class Scene::Light: public Primitive
{
public:
	Light():
	Primitive(1){}
	Light(const Color& color):
	Primitive(1), color(color){}
	void uvTransfer(const Vec3& , double& , double& ) const
	{
		assert(false);
	}
	const Material* material() const = 0;
	Color color;
};

class Scene::PointLight:public Light
{
	typedef Scene::Sphere Sphere;
public:
	PointLight(double x, double y, double z, double r, const Color& color):
	Light(color),
	position(Vec3(x, y, z), r){}
	PointLight(const Vec3& center, double r, const Color& color):
	Light(color),
	position(center, r){}
	PointLight(const Sphere& position, const Color& color):
	Light(color),
	position(position){}
	PointLight(const Sphere& position):
	position(position){}
	const Sphere& get() const{	return position;	}
	const Material* material() const {	return position.material();	}

	bool intersectAabb(const AabbBox& box) const {	return position.intersectAabb(box);	} 
	int intersect(const Ray& ray, IntersectResult& result) 
	{	
		int re = position.intersect(ray, result);	
		if (re)
			result.pri = this;
		return re;
	}
private:
	Sphere position;
};

class Scene::AreaLight:public Light
{
public:
	AreaLight(const AabbBox& area, const Color& color):
	Light(color),
	area(area),sampleGrid(0)
	{
		initSampleGrid();
	}

	void initSampleGrid();
	double getSampleGridX(int a)	{	return sampleGrid[(a & SAMPLEGRIDSIZEMOD) << 1];	}
	double getSampleGridZ(int a)	{	return sampleGrid[((a & SAMPLEGRIDSIZEMOD) << 1) ^ 1];	}
	const Material* material() const {	assert(false);return 0;	}
	const AabbBox& get() const {	return area;	}
	bool intersectAabb(const AabbBox& box) const {	return area.intersectAabb(box);	}
	int intersect(const Ray&ray, IntersectResult& result)
	{
		int re = area.intersect(ray, result);
		if (re)
			result.pri = this;
		return re;
	}
	static const int SAMPLEGRIDSIZE;
	static const int SAMPLEGRIDSIZEMOD;
	static const int SAMPLEGRIDSIZESQRT;
	static const double SAMPLEGRIDSIZESQRTDOWN;
private:
	AabbBox area;
	double* sampleGrid;
};
#endif