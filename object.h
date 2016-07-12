#ifndef OBJECT_H_
#define OBJECT_H_
#include "scene.h"
#include <vector>

class Object
{
	typedef Scene::Sphere Sphere;
	typedef Scene::Plane Plane;
	typedef Scene::PointLight PointLight;
	typedef Scene::AreaLight AreaLight;
	typedef Scene::Triangle Triangle;
public:
	Object(){};
	void push_back(Sphere* ele){	sphv.push_back(ele);	}
	void push_back(Plane* ele){	plav.push_back(ele);	}
	void push_back(PointLight* ele){	pltv.push_back(ele);	}
	void push_back(AreaLight* ele){	altv.push_back(ele);	}
	void push_back(Triangle* ele){	triv.push_back(ele);	}

	std::vector<Sphere*> sphv;
	std::vector<Plane*> plav;
	std::vector<PointLight*> pltv;
	std::vector<AreaLight*> altv;
	std::vector<Triangle*> triv;
};
#endif