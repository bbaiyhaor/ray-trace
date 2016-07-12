#ifndef PRIMITIVE_H_
#define PRIMITIVE_H_

namespace Scene{
	class Primitive;
}

class Material;
class AabbBox;
class Ray;
class IntersectResult;
#define SPHERE 0
#define LIGHT 1
#define PLANE 2
#define TRIANGLE 3
class Scene::Primitive
{
public:
	Primitive(int type):
	id(0), type(type){}
	virtual const Material* material() const = 0;
	//virtual int intersect(const Ray&, IntersectResult&) = 0;
	//virtual bool intersectAabb(const AabbBox&) const = 0;
	virtual void uvTransfer(const Vec3& , double& , double& ) const = 0;
	const int lastRay() const
	{
		return id;
	};
	int& lastId(){	return id;	};
	int getType() const {	return type;	}
private:
	int id, type;
};
#endif