#ifndef KDNODE_H_
#define KDNODE_H_
#include "box.h"
#include "triangle.h"
#include "intersectresult.h"
#include <vector>
class KdNode
{
public:
	AabbBox contained;
	union
	{
		struct
		{
			KdNode *left, *right;
		};
		struct 
		{
			KdNode* kid[2];			
		};
	};
	std::vector<Scene::Triangle*> triangles;
	static KdNode* build(std::vector<Scene::Triangle*>& tris, std::vector<AabbBox*>& trib, int depth = 0);
	static void subbuild(KdNode*, std::vector<Scene::Triangle*>& tris, std::vector<AabbBox*>& trib, int depth = 0);
	static int hit(KdNode* node, const Ray& ray, IntersectResult&, int depth = 0);
};
#endif