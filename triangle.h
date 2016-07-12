#ifndef TRIANGLE_H_
#define TRIANGLE_H_
#include "myvector.h"
#include "ray.h"
#include "intersectresult.h"
#include "box.h"
#include "primitive.h"
#include "material.h"
//#define DEBUG
#ifdef DEBUG
#include "debug.h"
#endif
class Scene::Triangle:public Scene::Primitive
{
public:
	Triangle():
	Primitive(3){}
	Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& fxl, Material* ma = 0):
	Primitive(3), vertex{v0, v1, v2}, fxl(fxl), ma(ma){}

	const Material* material() const 
	{	
		return ma;
	}
	//	gravity formula
	int intersect(const Ray& ray, IntersectResult& result)
	{
		this->lastId() = ray.getId();
		Vec3 e1 = vertex[1] - vertex[0];
		Vec3 e2 = vertex[2] - vertex[0];
		Vec3 q = cross(ray.d, e2);
		double a = dot(e1, q);
		if ((a > -1e-6) && (a < 1e-6))
			return 0;
		double f = 1.0 / a;
		Vec3 s = ray.o - vertex[0];
		double u = f * dot(s, q);
		if (u < 0.0)
			return 0;
		Vec3 r = cross(s, e1);
		double v = f * dot(ray.d, r);
		if ((v < 0.0) || (u + v > 1.0))
			return 0;
		double t = f * dot(e2, r);
		if (t < 0)
			return 0;
		if (t < result.dist - 1e-5){
			result.dist = t;
			result.inter = ray.o + t * ray.d;
			result.pri = this;
			result.fxl = fxl;
		}
		else if ((t - result.dist < 1e-5) && (t - result.dist > -1e-5)){
			result.fxl = result.fxl + fxl;
			Vec3::normalize(result.fxl);
		}
		if (dot(ray.d, result.fxl) < 0)
			return 1;
		else
			return -1;
	}

	#define axis_X 0
	#define axis_Y 1
	#define axis_Z 2

	#define CROSS(dest,v1,v2) \
		dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
		dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
		dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

	#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

	#define SUB(dest,v1,v2) \
		dest[0]=v1[0]-v2[0]; \
		dest[1]=v1[1]-v2[1]; \
		dest[2]=v1[2]-v2[2];

	#define FINDMINMAX(x0,x1,x2,min,max) \
		min = max = x0; \
		if(x1<min) min=x1;\
		if(x1>max) max=x1;\
		if(x2<min) min=x2;\
		if(x2>max) max=x2;

	int planeBoxOverlap(double normal[3],double d, double maxbox[3]) const
	{
		int q;
		double vmin[3],vmax[3];
		for(q=axis_X;q<=axis_Z;q++){
			if(normal[q]>0.0f){
				vmin[q]=-maxbox[q];
				vmax[q]=maxbox[q];
			}
			else{
				vmin[q]=maxbox[q];
				vmax[q]=-maxbox[q];
			}
		}
		if(DOT(normal,vmin)+d>0.0f) return 0;
		if(DOT(normal,vmax)+d>=0.0f) return 1;
		return 0;
	}


	/*======================== X-tests ========================*/
	#define AXISTEST_X01(a, b, fa, fb) \
		p0 = a*v0[axis_Y] - b*v0[axis_Z];\
		p2 = a*v2[axis_Y] - b*v2[axis_Z];\
		if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
		rad = fa * boxhalfsize[axis_Y] + fb * boxhalfsize[axis_Z]; \
		if(min>rad || max<-rad) return 0;

	#define AXISTEST_X2(a, b, fa, fb)\
		p0 = a*v0[axis_Y] - b*v0[axis_Z];\
		p1 = a*v1[axis_Y] - b*v1[axis_Z];\
		if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
		rad = fa * boxhalfsize[axis_Y] + fb * boxhalfsize[axis_Z]; \
		if(min>rad || max<-rad) return 0;

	/*======================== Y-tests ========================*/
	#define AXISTEST_Y02(a, b, fa, fb) \
		p0 = -a*v0[axis_X] + b*v0[axis_Z]; \
		p2 = -a*v2[axis_X] + b*v2[axis_Z]; \
		if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
		rad = fa * boxhalfsize[axis_X] + fb * boxhalfsize[axis_Z]; \
		if(min>rad || max<-rad) return 0;

	#define AXISTEST_Y1(a, b, fa, fb)\
		p0 = -a*v0[axis_X] + b*v0[axis_Z]; \
		p1 = -a*v1[axis_X] + b*v1[axis_Z]; \
		if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
		rad = fa * boxhalfsize[axis_X] + fb * boxhalfsize[axis_Z]; \
		if(min>rad || max<-rad) return 0;

	/*======================== Z-tests ========================*/

	#define AXISTEST_Z12(a, b, fa, fb) \
		p1 = a*v1[axis_X] - b*v1[axis_Y];\
		p2 = a*v2[axis_X] - b*v2[axis_Y];\
		if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
		rad = fa * boxhalfsize[axis_X] + fb * boxhalfsize[axis_Y]; \
		if(min>rad || max<-rad) return 0;

	#define AXISTEST_Z0(a, b, fa, fb)\
		p0 = a*v0[axis_X] - b*v0[axis_Y];\
		p1 = a*v1[axis_X] - b*v1[axis_Y];\
		if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
		rad = fa * boxhalfsize[axis_X] + fb * boxhalfsize[axis_Y]; \
		if(min>rad || max<-rad) return 0;

	bool intersectAabb(const AabbBox& box) const
	{
		double boxhalfsize[3] = {box.getSize().x * 0.5, 
								box.getSize().y * 0.5, 
								box.getSize().z * 0.5};

		double boxcenter[3] = {box.getMin().x + boxhalfsize[0], 
								box.getMin().y + boxhalfsize[1], 
								box.getMin().z + boxhalfsize[2]};

		double triverts[3][3] = {{vertex[0].x, vertex[0].y, vertex[0].z},
								{vertex[1].x, vertex[1].y, vertex[1].z},
								{vertex[2].x, vertex[2].y, vertex[2].z}};

		return triBoxOverlap(boxcenter, boxhalfsize, triverts);
	}

	int triBoxOverlap(double boxcenter[3],double boxhalfsize[3],double triverts[3][3]) const
	{
		/*use separating axis theorem to test overlap between triangle and box */
		/*need to test for overlap in these directions: */
		/*1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
		/* we do not even need to test these) */
		/*2) normal of the triangle */
		/*3) crossproduct(edge from tri, {x,y,z}-directin) */
		/* this gives 3x3=9 more tests */
		double v0[3],v1[3],v2[3];
		double min,max,d,p0,p1,p2,rad,fex,fey,fez;
		double normal[3],e0[3],e1[3],e2[3];

		 /* This is the fastest branch on Sun */
		 /* move everything so that the boxcenter is in (0,0,0) */
		 SUB(v0,triverts[0],boxcenter);
		 SUB(v1,triverts[1],boxcenter);
		 SUB(v2,triverts[2],boxcenter);

		 /* compute triangle edges */
		 SUB(e0,v1,v0);/* tri edge 0 */
		 SUB(e1,v2,v1);/* tri edge 1 */
		 SUB(e2,v0,v2);/* tri edge 2 */

		 /* Bullet 3:*/
		 /*test the 9 tests first (this was faster) */
		 fex = fabs(e0[axis_X]);
		 fey = fabs(e0[axis_Y]);
		 fez = fabs(e0[axis_Z]);
		 AXISTEST_X01(e0[axis_Z], e0[axis_Y], fez, fey);
		 AXISTEST_Y02(e0[axis_Z], e0[axis_X], fez, fex);
		 AXISTEST_Z12(e0[axis_Y], e0[axis_X], fey, fex);

		 fex = fabs(e1[axis_X]);
		 fey = fabs(e1[axis_Y]);
		 fez = fabs(e1[axis_Z]);
		 AXISTEST_X01(e1[axis_Z], e1[axis_Y], fez, fey);
		 AXISTEST_Y02(e1[axis_Z], e1[axis_X], fez, fex);
		 AXISTEST_Z0(e1[axis_Y], e1[axis_X], fey, fex);

		 fex = fabs(e2[axis_X]);
		 fey = fabs(e2[axis_Y]);
		 fez = fabs(e2[axis_Z]);
		 AXISTEST_X2(e2[axis_Z], e2[axis_Y], fez, fey);
		 AXISTEST_Y1(e2[axis_Z], e2[axis_X], fez, fex);
		 AXISTEST_Z12(e2[axis_Y], e2[axis_X], fey, fex);

		 /* Bullet 1: */
		 /*first test overlap in the {x,y,z}-directions */
		 /*find min, max of the triangle each direction, and test for overlap in */
		 /*that direction -- this is equivalent to testing a minimal AABB around */
		 /*the triangle against the AABB */

		 /* test in X-direction */
		 FINDMINMAX(v0[axis_X],v1[axis_X],v2[axis_X],min,max);
		 if(min>boxhalfsize[axis_X] || max<-boxhalfsize[axis_X]) return 0;

		 /* test in Y-direction */
		 FINDMINMAX(v0[axis_Y],v1[axis_Y],v2[axis_Y],min,max);
		 if(min>boxhalfsize[axis_Y] || max<-boxhalfsize[axis_Y]) return 0;

		 /* test in Z-direction */
		 FINDMINMAX(v0[axis_Z],v1[axis_Z],v2[axis_Z],min,max);
		 if(min>boxhalfsize[axis_Z] || max<-boxhalfsize[axis_Z]) return 0;

		 /* Bullet 2: */
		 /*test if the box intersects the plane of the triangle */
		 /*compute plane equation of triangle: normal*x+d=0 */
		 CROSS(normal,e0,e1);
		 d=-DOT(normal,v0);/* plane eq: normal.x+d=0 */
		 if(!planeBoxOverlap(normal,d,boxhalfsize)) return 0;

		 return 1; /* box and triangle overlaps */
	}

	void uvTransfer(const Vec3& , double& , double& ) const {	assert(false);	}
	Vec3 vertex[3], fxl;
private:
	Material* ma;
};
#endif