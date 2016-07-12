#include "light.h"
#include <cstdlib>

const int Scene::AreaLight::SAMPLEGRIDSIZE = 64;
const int Scene::AreaLight::SAMPLEGRIDSIZEMOD = 63;
const int Scene::AreaLight::SAMPLEGRIDSIZESQRT = 8;
const double Scene::AreaLight::SAMPLEGRIDSIZESQRTDOWN = 1.0 / SAMPLEGRIDSIZESQRT;
void Scene::AreaLight::initSampleGrid()
{
	if (sampleGrid == 0)
		sampleGrid = new double [SAMPLEGRIDSIZE << 1];
	int seq[SAMPLEGRIDSIZE];
	for (int i = 0; i < SAMPLEGRIDSIZE; i++)
		seq[i] = i;
	std::random_shuffle(seq, seq + SAMPLEGRIDSIZE);
	for (int i = 0; i < SAMPLEGRIDSIZE; i++){
		sampleGrid[i << 1] = (seq[i] / SAMPLEGRIDSIZESQRT) * area.getSize().x / SAMPLEGRIDSIZESQRT + area.getMin().x;
		sampleGrid[(i << 1) ^ 1] = (seq[i] % SAMPLEGRIDSIZESQRT) * area.getSize().z / SAMPLEGRIDSIZESQRT + area.getMin().z; 
	}	
}