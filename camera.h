#ifndef CAMERA_H_
#define CAMERA_H_

#include "myvector.h"
#include "ray.h"
#include "global.h"
#include <cmath>

#define DEBUG
#include "debug.h"

//	it's right hand coordinate system
class Camera
{
public:
	Camera(){}
	//	the reference system is world space, but the direction is relative to eye, view(or camera) space
	//	and the right direction is figured out by crossing front and up
	//	the view plane is 1 unit distance from eye, f equal to 1
	//	the left-down coordinate is (-1, -1) and the right-up coordinate is (1, 1) in clip space
	//	the left-down coordinate is (0, 0) and the right-up coordinate is (1, 1) in screen space
	//	so the coordinate transformation is 
	//	x = 2(u - 0.5)	u(screen space)->x(clip space)
	//	y = 2(v - 0.5)	v(screen space)->y(clip space)
	Camera(const Vec3& eye, const Vec3& front, const Vec3& realUp, double span):
	aperture(0.0),
	eye(eye), 
	front(Vec3::normalize(front)), 
	realUp(Vec3::normalize(realUp)),
	span(span)
	//	add "this->" in front of the same variable !!!
	{
		this->right = cross(this->front, this->realUp);
		this->up = cross(this->right, this->front);
		this->spanScale = std::tan(this->span * 0.5 * MYPI / 180.0) * 2.0;
	}
	//	another camera used in pathtracing
	Camera(const Vec3& eye, const Vec3& front, const Vec3& right, double spanr, double spanu):
	aperture(0.0),
	eye(eye), 
	front(Vec3::normalize(front)), 
	right(Vec3::normalize(right)),
	spanr(spanr),
	spanu(spanu)
	//	add "this->" in front of the same variable !!!
	{
		this->up = cross(this->right, this->front);
		this->spanScaler = this->spanr * MYPI / 360.0;
		this->spanScaleu = this->spanu * MYPI / 360.0;
	}
	const Ray transmit(double u, double v)
	{
		Ray ray;
		transmit(u, v, ray);
		return ray;
 	}
 	//	ray tracing
 	void transmit(double u, double v, Ray& ray)
	{
		Vec3 rightOffset = (u - 0.5) * spanScale * right;
		Vec3 upOffset = (v - 0.5) * spanScale * up;
		ray.d = Vec3::normalize(front + rightOffset + upOffset);	
		ray.o = eye;
 	}
 	//	path tracing
 	void transmit(double u, double v, Ray& ray, int)
	{
		Vec3 rightOffset = (u - 0.5) * spanScaler * right;
		Vec3 upOffset = (v - 0.5) * spanScaleu * up;
		ray.d = Vec3::normalize(front + rightOffset + upOffset);
		ray.o = eye;
 	}

 	const Vec3& getFront() const {	return front;	}
 	const Vec3& getUp() const {	return up;	}
 	const Vec3& getRight() const {	return right;	}
 	const Vec3& getRealUp() const {	return realUp;	}
 	double aperture;
private:
	//	eye dot
	//	front direction
	//	up direction
	//	realUp the up direction of real world
	//	horizontal and vertical span of [0,180] degree, they are same
	Vec3 eye, front, up, right, realUp;
	union
	{
		struct 
		{
			double spanScale, span;
		};
		struct 
		{
			double spanScaler, spanScaleu, spanr, spanu;
		};
	};
};
#endif