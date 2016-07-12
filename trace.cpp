#include "trace.h"
#include "intersectresult.h"
#include "object.h"
#include "camera.h"
#include "scene.h"
#include "mymat.h"
#include "opencv.h"
#include "plane.h"
#include "light.h"
#include "global.h"
#include <vector>
#include <cmath>
#include "myrandom.h"
#include "triangle.h"
#include "kdnode.h"
//#define DEBUG
#ifdef DEBUG
#include "debug.h"
#endif

#include "switchmacro.h"
#ifdef THREAD
#include <future>
#include <thread>
#endif
namespace Trace
{
	#ifdef REGULAR
	const int GRIDSIZE = 50;
	#else
	const int GRIDSIZE = 10;	
	#endif
	const int GRIDSIZE2 = GRIDSIZE * GRIDSIZE;
	const int GRIDSIZE3 = GRIDSIZE2 * GRIDSIZE;

	KdNode* root;
	Object cellGrid[GRIDSIZE * GRIDSIZE * GRIDSIZE];
	AabbBox world;

	double unitX, unitY, unitZ;
	Vec3 unit;
	Object scene;
	MyRandom prorand;

	#ifdef REGULAR
	#define nearest(it, ray, result, re)\
		int in_out = 0;\
		if (it->lastRay() != ray.getId()){\
			if (in_out = it->intersect(ray, result))\
				re = in_out;\
		}
	int findNearest(const Ray& ray, IntersectResult& result)
	{		
		//	X/Y/Z: current coordinate of grid 
		int X = (ray.o.x - world.getMin().x) / unitX;
		int Y = (ray.o.y - world.getMin().y) / unitY;
		int Z = (ray.o.z - world.getMin().z) / unitZ;	
		//	the source of ray is out of grid
		if ((X < 0) || (X >= GRIDSIZE)
			|| (Y < 0) || (Y >= GRIDSIZE)
			|| (Z < 0) || (Z >= GRIDSIZE))
			return MISS;
		//	step: the increasing delta
		//	out: the stop boundary		
		//	nextCoordinate: the next coordiante along ray direction	
		int stepX, outX, stepY, outY, stepZ, outZ;	
		Vec3 nextCoordinate;

		if (ray.d.x > 0.0){
			stepX = 1;
			outX = GRIDSIZE;
			nextCoordinate.x = world.getMin().x + (X + 1.0) * unitX;
		}
		else{
			stepX = -1;
			outX = -1;
			nextCoordinate.x = world.getMin().x + X * unitX;
		}

		if (ray.d.y > 0.0){
			stepY = 1;
			outY = GRIDSIZE;
			nextCoordinate.y = world.getMin().y + (Y + 1.0) * unitY;
		}
		else{
			stepY = -1;
			outY = -1;
			nextCoordinate.y = world.getMin().y + Y * unitY;
		}

		if (ray.d.z > 0.0){
			stepZ = 1;
			outZ = GRIDSIZE;
			nextCoordinate.z = world.getMin().z + (Z + 1.0) * unitZ;
		}
		else{
			stepZ = -1;
			outZ = -1;
			nextCoordinate.z = world.getMin().z + Z * unitZ;
		}

		//	tmax: getting a boundary at most needs t (o + t * dire)
		//	tmax.x->arrive x boundary; tmax.y->y boundary; tmax.z->z boundary 
		//	(select the min, arrive min boundary firstly) 
		//	tdelta: crossing a cell at least needs t 
		//	x,y,z -> cross x,y,z boundary respectly
		Vec3 tmax, tdelta;
		if (ray.d.x != 0.0){
			tmax.x = (nextCoordinate.x - ray.o.x) / ray.d.x;
			tdelta.x = unitX * stepX / ray.d.x;
		}
		else 
			tmax.x = INF;

		if (ray.d.y != 0.0){
			tmax.y = (nextCoordinate.y - ray.o.y) / ray.d.y;
			tdelta.y = unitY * stepY / ray.d.y;
		}
		else
			tmax.y = INF;

		if (ray.d.z != 0.0){
			tmax.z = (nextCoordinate.z - ray.o.z) / ray.d.z;
			tdelta.z = unitZ * stepZ / ray.d.z;
		}
		else
			tmax.z = INF;

		//	find the first intersection
		int re = MISS;
		while (true){
			int xyz = X * GRIDSIZE2 + Y * GRIDSIZE + Z;
			for (auto& it : cellGrid[xyz].sphv){
				nearest(it, ray, result, re);
				if (in_out)
					goto closer;
			}							
			for (auto& it : cellGrid[xyz].plav){
				nearest(it, ray, result, re);
				if (in_out)
					goto closer;
			}			
			for (auto& it : cellGrid[xyz].pltv){
				nearest(it, ray, result, re);
				if (in_out)
					goto closer;
			}			
			for (auto& it : cellGrid[xyz].altv){
				nearest(it, ray, result, re);
				if (in_out)
					goto closer;
			}			
			for (auto& it : cellGrid[xyz].triv){
				nearest(it, ray, result, re);
				if (in_out)
					goto closer;
			}			
			if (tmax.x < tmax.y){
				if (tmax.x < tmax.z){
					X += stepX;
					if (X == outX)
						return MISS;
					tmax.x += tdelta.x;
				}
				else{
					Z += stepZ;
					if (Z == outZ)
						return MISS;
					tmax.z += tdelta.z;
				}
			}
			else {
				if (tmax.y < tmax.z){
					Y += stepY;
					if (Y == outY)
						return MISS;
					tmax.y += tdelta.y;
				}
				else{
					Z += stepZ;
					if (Z == outZ)
						return MISS;
					tmax.z += tdelta.z;
				}
			}
		}
	closer:
		//	find the correct intersection
		while (true){
			int xyz = X * GRIDSIZE2 + Y * GRIDSIZE + Z;
			for (auto& it : cellGrid[xyz].sphv){
				nearest(it, ray, result, re);				
			}				
			for (auto& it : cellGrid[xyz].plav){
				nearest(it, ray, result, re);				
			}							
			for (auto& it : cellGrid[xyz].pltv){
				nearest(it, ray, result, re);				
			}
			for (auto& it : cellGrid[xyz].altv){
				nearest(it, ray, result, re);				
			}
			for (auto& it : cellGrid[xyz].triv){
				nearest(it, ray, result, re);				
			}				
			if (tmax.x < tmax.y){
				if (tmax.x < tmax.z){
					//	find the correct intersection
					if (result.dist < tmax.x)
						break;
					X += stepX;
					if (X == outX)
						break;
					tmax.x += tdelta.x;
				}
				else{
					if (result.dist < tmax.z)
						break;
					Z += stepZ;
					if (Z == outZ)
						break;
					tmax.z += tdelta.z;
				}
			}
			else {
				if (tmax.y < tmax.z){
					if (result.dist < tmax.y)
						break;
					Y += stepY;
					if (Y == outY)
						break;
					tmax.y += tdelta.y;
				}
				else{
					if (result.dist < tmax.z)
						break;
					Z += stepZ;
					if (Z == outZ)
						break;
					tmax.z += tdelta.z;
				}
			}
		}
		return re;
	}

	#define traverse(it, lmin, rmax) \
	{\
		AabbBox bound = AabbBox::getAabb(*it);\
		int x1 = (bound.getMin().x - lmin.x) / unitX;\
		int x2 = (bound.getMax().x - lmin.x) / unitX + 1;\
		x1 = x1 < 0 ? 0 : x1;\
		x2 = x2 > GRIDSIZE ? GRIDSIZE : x2;\
		int y1 = (bound.getMin().y - lmin.y) / unitY;\
		int y2 = (bound.getMax().y - lmin.y) / unitY + 1;\
		y1 = y1 < 0 ? 0 : y1;\
		y2 = y2 > GRIDSIZE ? GRIDSIZE : y2;\
		int z1 = (bound.getMin().z - lmin.z) / unitZ;\
		int z2 = (bound.getMax().z - lmin.z) / unitZ + 1;\
		z1 = z1 < 0 ? 0 : z1;\
		z2 = z2 > GRIDSIZE ? GRIDSIZE : z2;\
		for (int x = x1; x < x2; x++)\
			for (int y = y1; y < y2; y++)\
				for (int z = z1; z < z2; z++){\
					Vec3 l(lmin.x + x * unitX, lmin.y + y * unitY, lmin.z + z * unitZ);\
					AabbBox cell(l, l + unit, unit);\
					if (it->intersectAabb(cell))\
						cellGrid[x * GRIDSIZE2 + y * GRIDSIZE + z].push_back(it);\
				}\
	}
	void buildGrid()
	{
		#ifdef PATHTRACE
		Vec3 lmin(-5.0, -5.0, -10.0), rmax(100.0, 85.0, 300.0);
		#endif
		#ifdef PHONGTRACE
		#ifdef SMALLWORLD
		Vec3 lmin(-15.0, -10.0, -5.0), rmax(15.0, 10.0, 30.0);
		#endif
		#ifdef BIGWORLD
		Vec3 lmin(-500.0, -500.0, -500.0), rmax(500.0, 500.0, 500.0);
		#endif
		#endif
		//	a big aabb box contain all objects
		world = AabbBox(lmin, rmax, rmax - lmin);
		
		//	a unit cell
		unitX = (rmax.x - lmin.x) / GRIDSIZE;
		unitY = (rmax.y - lmin.y) / GRIDSIZE;
		unitZ = (rmax.z - lmin.z) / GRIDSIZE;
		unit = Vec3(unitX, unitY, unitZ);

		for (auto& it : scene.sphv)
			traverse(it, lmin, rmax);
		
		for (auto& it : scene.plav)
			traverse(it, lmin, rmax);
		
		for (auto& it : scene.pltv)
			traverse(it, lmin, rmax);

		for (auto& it : scene.altv)
			traverse(it, lmin, rmax);

		for (auto& it : scene.triv)
			traverse(it, lmin, rmax);
		int hehe = GRIDSIZE * GRIDSIZE * GRIDSIZE;
		#ifdef DEBUG
		for (int i  = 0 ; i < hehe; i++)
			if (cellGrid[i].triv.size() > 0)
			cout << cellGrid[i].triv.size() << " ";
		cout << endl;
		#endif
	}

	void initial()
	{
		buildGrid();	
	}
	#endif
	#ifdef KD
	int findNearest(const Ray& ray, IntersectResult& result)
	{
		int re = 0;
		int in_out = 0;
		if (in_out = KdNode::hit(root, ray, result))
			re = in_out;
		for (auto& it : scene.sphv){
			in_out = 0;
			if (in_out = it->intersect(ray, result))
				re = in_out;													
			
		}
		for (auto& it : scene.pltv){
			in_out = 0;
			if (in_out = it->intersect(ray, result))
				re = in_out;													
			
		}
		for (auto& it : scene.plav){
			in_out = 0;
			if (in_out = it->intersect(ray, result))
				re = in_out;													
		}
		for (auto& it : scene.altv){
			in_out = 0;
			if (in_out = it->intersect(ray, result))
				re = in_out;													
		}
		return re;
	}
	void initial()
	{
		vector<AabbBox*> trib;
		for (auto& it : scene.triv)
			trib.push_back(new AabbBox(AabbBox::getAabb(*it)));
		root = KdNode::build(scene.triv, trib);
		for (int i = 0, size = trib.size(); i < size; i++)
			delete trib[i];
	}
	#endif
	#ifdef PUSU
	int findNearest(const Ray& ray, IntersectResult& result)
	{
		int re = 0;
		for (auto& it : scene.sphv){
			int in_out = 0;
			if (in_out = it->intersect(ray, result))
				re = in_out;													
			
		}
		for (auto& it : scene.pltv){
			int in_out = 0;
			if (in_out = it->intersect(ray, result))
				re = in_out;													
			
		}
		for (auto& it : scene.plav){
			int in_out = 0;
			if (in_out = it->intersect(ray, result))
				re = in_out;													
		}
		for (auto& it : scene.altv){
			in_out = 0;
			if (in_out = it->intersect(ray, result))
				re = in_out;													
		}
		return re;
	}
	void initial()
	{

	}
	#endif
	#include "raytrace.inc"
	#include "pathtrace.inc"

};