#ifndef MATERIAL_H_
#define MATERIAL_H_
#include "myvector.h"
#include "global.h"
#include "texture.h"
#include "primitive.h"
#define DEBUG
#ifdef DEBUG
#include "debug.h"
#endif
#include <cmath>
class Material
{
public:
	Material(const Color& color, const char* filename = 0):
	color(color), 
	tex(filename)
	{
		if (filename != 0)
			texExist = true;
		else
			texExist = false;
	}
	
	//	phong
	virtual const Color sample(const Vec3& ray, const Vec3& L, const Vec3& normal, const Vec3& position, const Color& light)const = 0;	
	virtual const Color sample(const Vec3& ray, const Vec3& L, const Vec3& normal, const Scene::Primitive* pri, const Vec3& position, const Color& light)const = 0;	
	virtual const Color sample(const Vec3& ray, const Vec3& L, const Vec3& normal, const Color& texc, const Vec3& position, const Color& light)const = 0;	
	virtual const double getDiffuse() const = 0;
	virtual const double getSpecular() const = 0;
	virtual const double getShininess() const = 0;
	virtual const double getReflectiveness() const = 0;
	virtual const double getRefraction() const = 0;
	virtual const double getRefraIndex() const = 0;
	virtual const double getGlossyReflect() const = 0;



	//	BRDF
	virtual const Color& getEmission() const = 0;
	virtual const int getBRDFType() const = 0;


	const Color getTexc(const Scene::Primitive* pri, const Vec3& position) const
	{
		double x, y;
        pri->uvTransfer(position, x, y);
        return tex.getColor(x, y);
	}
	Color color;
	Texture tex;
	bool texExist;
};


class Phong:public Material
{
public:
	Phong(const Color& color, double diffuse, double specular, double shininess = 20, double reflectiveness = 0, double refraction = 0, double refraIndex = 1.0, double glossyReflect = 0.0, const char* filename = 0):
	Material(color, filename),
	diffuse(diffuse), specular(specular), shininess(shininess), 
	reflectiveness(reflectiveness), refraction(refraction), refraIndex(refraIndex), glossyReflect(glossyReflect){}
	
	const Color sample(const Vec3& ray, const Vec3& L, const Vec3& normal, const Vec3& position, const Color& light)const
	{
	    //	l: the opposite vector of incident light
        //	r: reflected light
        //	v: the vector towards the viewer(the opposite vector of camera ray)
        //	h: the angle-bisector of l and v
        //	n: normal vector

        //	r = (-l) + 2*dot(l, n)n
        //	f(l->v) = diffuseColor + specularColor * dot(r, v)^s / dot(n, l)
        //=>f(l->v) = diffuseColor + specularColor * dot(r, v)    [used]
        double NdotL = std::max(dot(normal, L), 0.0);
        Vec3 R = L - 2 * dot(L, normal) * normal;
        double VdotR = std::max(dot(ray, R), 0.0);
        Color re = NdotL * diffuse * color * light + std::pow(VdotR, shininess) * specular * light;          
		return re;	
	}
	//	used in texture
	const Color sample(const Vec3& ray, const Vec3& L, const Vec3& normal, const Scene::Primitive* pri, const Vec3& position, const Color& light) const
	{
		double NdotL = std::max(dot(normal, L), 0.0);
        Vec3 R = L - 2 * dot(L, normal) * normal;
        double VdotR = std::max(dot(ray, R), 0.0);
        Color texc = getTexc(pri, position);
        Color re = NdotL * diffuse * texc * light + std::pow(VdotR, shininess) * specular * light;    
		return re;	
	}

	//	used in texture
	const Color sample(const Vec3& ray, const Vec3& L, const Vec3& normal, const Color& texc, const Vec3& position, const Color& light)const
	{
		double NdotL = std::max(dot(normal, L), 0.0);
        Vec3 R = L - 2 * dot(L, normal) * normal;
        double VdotR = std::max(dot(ray, R), 0.0);
        Color re = NdotL * diffuse * texc * light + std::pow(VdotR, shininess) * specular * light;    
		return re;	
	}

	virtual const double getDiffuse() const {	return diffuse;	}
	virtual const double getSpecular() const {	return specular;	}
	virtual const double getShininess() const {	return shininess;	}
	virtual const double getReflectiveness() const {	return reflectiveness;	}
	virtual const double getRefraction() const {	return refraction;	}	
	virtual const double getRefraIndex() const {	return refraIndex;	}
	virtual const double getGlossyReflect() const {	return glossyReflect;	}

	virtual const Color& getEmission() const {	assert(false);	}
	virtual const int getBRDFType() const {	assert(false);	}

private:
	double diffuse, specular, shininess, reflectiveness, refraction, refraIndex, glossyReflect;		
};

class Chess:public Material
{
public:
	Chess(const Color& color, const Color& color1, double diffuse, double specular, double shininess = 20, double reflectiveness = 0, double refraction = 0, double refraIndex = 1.0, double glossyReflect = 0.0, double scale = 0.1, const char* filename = 0):
	Material(color, filename), color1(color1), scale(scale),
	diffuse(diffuse), specular(specular), shininess(shininess), 
	reflectiveness(reflectiveness), refraction(refraction), refraIndex(refraIndex), glossyReflect(glossyReflect){}
	
	const Color sample(const Vec3* ray, const Vec3* L, const Vec3* normal, const Vec3* position, const Color* light)const
	{
		int oe = abs(floor(position->x * scale) + floor(position->z * scale));
		if (oe & 1)
			return color * (*light);
		else
			return color1 * (*light);
	}

	const Color sample(const Vec3& ray, const Vec3& L, const Vec3& normal, const Vec3& position, const Color& light)const
	{
		int oe = abs(floor(position.x * scale) + floor(position.z * scale));
		if (oe & 1)
			return color * light;
		else
			return color1 * light;
	}

	const Color sample(const Vec3& position)const
	{
		int oe = abs(floor(position.x * scale) + floor(position.z * scale));
		if (oe & 1)
			return color;
		else
			return color1;	
	}

	void sample(const Vec3& position, Color& re)const
	{
		int oe = abs(floor(position.x * scale) + floor(position.z * scale));
		if (oe & 1)
			re = color;
		else
			re = color1;
	}

	virtual const double getDiffuse() const {	return diffuse;	}
	virtual const double getSpecular() const {	return specular;	}
	virtual const double getShininess() const {	return shininess;	}
	virtual const double getReflectiveness() const {	return reflectiveness;	}
	virtual const double getRefraction() const {	return refraction;	}	
	virtual const double getRefraIndex() const {	return refraIndex;	}
	virtual const double getGlossyReflect() const {	return glossyReflect;	}
	virtual const Color& getEmission() const {	assert(false);	}
	virtual const int getBRDFType() const {	assert(false);	}

private:
	Color color1;
	double scale;
	double diffuse, specular, shininess, reflectiveness, refraction, refraIndex, glossyReflect;		

};

class BRDF : public Material
{
public:
	BRDF(const Color& color, const Color& emission, int BRDFType, double refraIndex = 1.0, const char* filename = 0):
	Material(color, filename), emission(emission), BRDFType(BRDFType), refraIndex(refraIndex){}

	virtual const Color& getEmission() const {	return emission;	}
	virtual const int getBRDFType() const {	return BRDFType;	}


	virtual const Color sample(const Vec3& ray, const Vec3& L, const Vec3& normal, const Vec3& position, const Color& light)const  {	assert(false);	}
	virtual const Color sample(const Vec3& ray, const Vec3& L, const Vec3& normal, const Scene::Primitive* pri, const Vec3& position, const Color& light)const {	assert(false);	}
	virtual const Color sample(const Vec3& ray, const Vec3& L, const Vec3& normal, const Color& texc, const Vec3& position, const Color& light)const {	assert(false);	}	
	virtual const double getDiffuse() const {	assert(false);	}
	virtual const double getSpecular() const {	assert(false);	}
	virtual const double getShininess() const {	assert(false);	}
	virtual const double getReflectiveness() const {	assert(false);	}
	virtual const double getRefraction() const {	assert(false);	}
	virtual const double getRefraIndex() const {	return refraIndex;	}
	virtual const double getGlossyReflect() const {	assert(false);	}
private:
	Color emission;
	int BRDFType;
	double refraIndex;
};
#endif