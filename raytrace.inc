#pragma once
#ifdef PHONGTRACE
//	Monte Carlo sampling
double calcShadow(Scene::AreaLight* light, const Vec3& inter, Vec3& L, double sampleTime, double sampleDown)
{
	double re = 0;
	L = light->get().getMin() + 0.5 * light->get().getSize() - inter;
	Vec3::normalize(L);
	double deltaX = light->get().getSize().x * Scene::AreaLight::SAMPLEGRIDSIZESQRTDOWN;
	double deltaZ = light->get().getSize().z * Scene::AreaLight::SAMPLEGRIDSIZESQRTDOWN;
	for (int i = 0; i < sampleTime; i++){
		Vec3 lightPoint(light->getSampleGridX(i) + prorand.rand01() * deltaX,
			light->get().getMin().y, light->getSampleGridZ(i) + prorand.rand01() * deltaZ);
		Vec3 dire = lightPoint - inter;
		Vec3::normalize(dire);
		IntersectResult temp;
		if (findNearest(Ray(inter + DEVIATION * dire, dire), temp) &&
			(temp.pri == light))
			re += sampleDown;
	}
	return re;
}


const Color rayTraceRecursive(const Ray& ray, IntersectResult& result, int recursiveDepth, double sampleTime, double sampleDown)
{
	//	find the nearest intersection
	double in_out = findNearest(ray, result);
	//	no intersection
	if (in_out == 0)
		return black;	
	//	intersection is in light
	if (result.pri->getType() == LIGHT)
		return dynamic_cast<Scene::Light*>(const_cast<Scene::Primitive*>(result.pri))->color;
	Color re = black;	
	//	figure out the diffuseColor and specularColor
	for (auto& it : scene.pltv){
		Vec3 L = it->get().getCenter() - result.inter;
		double Ldist = sqrt(dot(L, L));			
		Vec3::normalize(L);
		//	whether it's screened or not
		Ray shadowRay(result.inter + DEVIATION * L, L);
		IntersectResult temp;
		findNearest(shadowRay, temp);					
		if (temp.pri != it)
			continue;
		#ifndef TEXTURE
		re = re + result.pri->material()->sample(ray.d, L, result.fxl, result.inter, it->color);	
		#else
		if (result.pri->material()->texExist)
			re = re + result.pri->material()->sample(ray.d, L, result.fxl, result.pri, result.inter, it->color);	
		else
			re = re + result.pri->material()->sample(ray.d, L, result.fxl, result.inter, it->color);	
		#endif
	}
	for (auto& it : scene.altv){
		Vec3 L;
		double shadow = calcShadow(it, result.inter, L, sampleTime, sampleDown);
		if (shadow > 0)
			#ifndef TEXTURE
			re = re + shadow * result.pri->material()->sample(ray.d, L, result.fxl, result.inter, it->color);			
			#else
			if (result.pri->material()->texExist)
				re = re + shadow * result.pri->material()->sample(ray.d, L, result.fxl, result.pri, result.inter, it->color);	
			else
				re = re + shadow * result.pri->material()->sample(ray.d, L, result.fxl, result.inter, it->color);				
			#endif
	}

	//	calculate refraction
	double refraction = result.pri->material()->getRefraction();
	double Re = 1, Tr = 0;
	if (recursiveDepth < TRACEMAXRECUR && refraction > 0){
		double tIndex = result.pri->material()->getRefraIndex();
		double n = (in_out == 1) ? (1.0 / tIndex) : (tIndex / 1.0);
		Vec3 N = in_out * result.fxl;
		double cosI = -dot(N, ray.d );
		double sqrcosT = 1.0 - n * n * (1.0 - cosI * cosI);		
		
		if (sqrcosT > 0.0){
			double a = tIndex - 1.0;
			double b = tIndex + 1.0;
			double c = 1 - (in_out == 1 ? cosI : sqrt(sqrcosT));   
			double R0 = a * a / (b * b);
			//	schlick's approximation
			Re = R0 + (1 - R0) * c * c * c * c * c;
			Tr = 1 - Re;		
			Vec3 T = (n * ray.d) + (n * cosI - sqrt(sqrcosT)) * N;
			IntersectResult resultNext;
			#ifdef SAMPLEWAY
			Color got = rayTraceRecursive(Ray(result.inter + DEVIATION * T, T), resultNext, recursiveDepth + 1, sampleTime, sampleDown);
			#else
			Color got = rayTraceRecursive(Ray(result.inter + DEVIATION * T, T), resultNext, recursiveDepth + 1, sampleTime * 0.25, sampleDown * 4.0);
			#endif
			// apply Beer's law
			const double BEER = 0.01;
			#ifndef TEXTURE
			Color absorbance = -resultNext.dist * BEER * result.pri->material()->color;					
			#else
			Color absorbance;
			if (result.pri->material()->texExist)
				absorbance = -resultNext.dist * BEER * result.pri->material()->getTexc(result.pri, result.inter);					
			else
				absorbance = -resultNext.dist * BEER * result.pri->material()->color;					
			#endif
			Color transparency = Color(exp(absorbance.x), exp(absorbance.y), exp(absorbance.z));				
			re = re + Tr * got * transparency;
		}
	}


	//	calculate reflection
	double reflectiveness = result.pri->material()->getReflectiveness();
	if (recursiveDepth < TRACEMAXRECUR && reflectiveness > 0){
		reflectiveness = min(Re, reflectiveness);
		double glossyReflectOffset = result.pri->material()->getGlossyReflect();
		//	only perform diffuse reflection for primary rays; otherwise it's just too slow.
		#ifdef SAMPLEWAY
		if ((glossyReflectOffset > 0) && (recursiveDepth == 0)){
			Vec3 dire = ray.d - 2.0 * dot(ray.d, result.fxl) * result.fxl;
			//	direXZ and dire are vertical crossing in xz plane
			Vec3 direXZ(dire.z, 0, -dire.x);
			Vec3 direY = cross(dire, direXZ);
			reflectiveness *= GLOSSYSAMPLEDOWN;
			for (int i = 0; i < GLOSSYSAMPLETIME; i++){
				double xzoffs = prorand.exrand01();
				double yoffs = prorand.exrand01() * (1.0 - xzoffs * xzoffs);
				Vec3 realDire = dire + xzoffs * glossyReflectOffset * direXZ + yoffs * glossyReflectOffset * direY;
				Vec3::normalize(realDire);
				Ray reflect(result.inter + DEVIATION * realDire, realDire);
				IntersectResult resultNext;
				Color got = rayTraceRecursive(reflect, resultNext, recursiveDepth + 1, sampleTime, sampleDown);
				#ifndef TEXTURE
				re = re + reflectiveness * got * result.pri->material()->color;
				#else
				if (result.pri->material()->texExist)
					re = re + reflectiveness * got * result.pri->material()->getTexc(result.pri, result.inter);
				else
					re = re + reflectiveness * got * result.pri->material()->color;
				#endif
			}
		}
		#else
		if ((glossyReflectOffset > 0) && (recursiveDepth < 2)){
			Vec3 dire = ray.d - 2.0 * dot(ray.d, result.fxl) * result.fxl;
			//	direXZ and dire are vertical crossing in xz plane
			Vec3 direXZ(dire.z, 0, -dire.x);
			Vec3 direY = cross(dire, direXZ);
			reflectiveness *= sampleDown;
			for (int i = 0; i < sampleTime; i++){
				double xzoffs = prorand.exrand01();
				double yoffs = prorand.exrand01() * (1.0 - xzoffs * xzoffs);
				Vec3 realDire = dire + xzoffs * glossyReflectOffset * direXZ + yoffs * glossyReflectOffset * direY;
				Vec3::normalize(realDire);
				Ray reflect(result.inter + DEVIATION * realDire, realDire);
				IntersectResult resultNext;
				Color got = rayTraceRecursive(reflect, resultNext, recursiveDepth + 1, sampleTime * 0.25, sampleDown * 4.0);
				#ifndef TEXTURE
				re = re + reflectiveness * got * result.pri->material()->color;
				#else
				if (result.pri->material()->texExist)
					re = re + reflectiveness * got * result.pri->material()->getTexc(result.pri, result.inter);
				else
					re = re + reflectiveness * got * result.pri->material()->color;
				#endif
			}
		}
		#endif
		else{

			Vec3 dire = ray.d - 2 * dot(ray.d, result.fxl) * result.fxl;
			//	move origin towards direction a little distance
			//	in order to figure out right intersection
			Ray reflect(result.inter + DEVIATION * dire, dire);
			IntersectResult resultNext;
			#ifdef SAMPLEWAY
			Color got = rayTraceRecursive(reflect, resultNext, recursiveDepth + 1, sampleTime, sampleDown);
			#else
			Color got = rayTraceRecursive(reflect, resultNext, recursiveDepth + 1, sampleTime * 0.25, sampleDown * 4.0);
			#endif
			#ifndef TEXTURE
			re = re + reflectiveness * got * result.pri->material()->color;
			#else
			if (result.pri->material()->texExist)
				re = re + reflectiveness * got * result.pri->material()->getTexc(result.pri, result.inter);
			else
				re = re + reflectiveness * got * result.pri->material()->color;
			#endif
		}
	}	

	return re;
}
#ifdef THREAD

void render(MyMat& img, Camera& camera)
{
	double rows = img.row();
	double cols = img.col();	
	const Scene::Primitive* lastObject = 0;
	const double rowsDeviation = 0.5 / rows;
	const double colsDeviation = 0.5 / cols;
	camera.aperture = 1.5;
	#ifdef APERTURE
	double objectDist = 45.0;
	double objectDistSqr = objectDist * objectDist;
	Vec3 cameraFront = camera.getFront();
	Vec3 cameraUp = camera.getUp();
	Vec3 cameraRight = camera.getRight();
	#endif
	for (double r = 0; r < rows; r++){
//	for (double r = rows - 1; r >= 0; r--){
		double v = 1 - r / rows;			
		for (double c = 0; c < cols; c++){
//			double u = 1.0 - c / cols;		
			double u = c / cols;
			#ifdef APERTURE
			Ray ray;
			camera.transmit(u, v, ray);	
			Vec3 focusInter = ray.o + objectDist / dot(ray.d, cameraFront) * ray.d;
			IntersectResult result;
			Color color = rayTraceRecursive(ray, result, 0, GLOSSYSAMPLETIME, GLOSSYSAMPLEDOWN);		
			const double sampt = 299;
			for (int i = 0; i < sampt; i++){
				Ray newRay;
				double lroffs = prorand.exrand01();
				double udoffs = prorand.exrand01() * (1.0 - lroffs * lroffs);
				newRay.o = ray.o + camera.aperture  * lroffs * cameraRight + camera.aperture * udoffs * cameraUp;
				newRay.d = focusInter - newRay.o;
				Vec3::normalize(newRay.d);
				result.dist = INF;
				color = color + rayTraceRecursive(newRay, result, 0, GLOSSYSAMPLETIME, GLOSSYSAMPLEDOWN);
			}
			color = 1.0 / (sampt + 1.0) * color;
			img.pixel(r, c) = color.imgcolor();
			
			#else
			Ray ray;
			camera.transmit(u, v, ray);	
			IntersectResult result;
			Color color = rayTraceRecursive(ray, result, 0, GLOSSYSAMPLETIME, GLOSSYSAMPLEDOWN);		
			//	supersample for the boundary between two objects
			#ifdef SUPERSAMPLE
			if (result.pri != lastObject){
				lastObject = result.pri;
				result.dist = INF;
				ray = camera.transmit(u - colsDeviation, v - rowsDeviation);
				std::future<const Color> color1(std::async(std::launch::async, rayTraceRecursive, std::cref(ray) , std::ref(result), 0, GLOSSYSAMPLETIME, GLOSSYSAMPLEDOWN));
				
				IntersectResult result1;
				Ray ray1 = camera.transmit(u + colsDeviation, v - rowsDeviation);
				std::future<const Color> color2(std::async(std::launch::async, rayTraceRecursive, std::cref(ray1), std::ref(result1), 0, GLOSSYSAMPLETIME, GLOSSYSAMPLEDOWN));				
				color = 0.33 * (color + color1.get() + color2.get());
			}
			#endif
			img.pixel(r, c) = color.imgcolor();
			#endif
		}
		img.show("fifth");
		img.waitKey(10);
	}
}
#else
void render(MyMat& img, Camera& camera)
{
	double rows = img.row();
	double cols = img.col();	
	const Scene::Primitive* lastObject = 0;
	const double rowsDeviation = 0.5 / rows;
	const double colsDeviation = 0.5 / cols;
	camera.aperture = 1.5;
	#ifdef APERTURE
	double objectDist = 89.0;
	double objectDistSqr = objectDist * objectDist;
	Vec3 cameraFront = camera.getFront();
	Vec3 cameraUp = camera.getUp();
	Vec3 cameraRight = camera.getRight();
	#endif
	for (double r = 0; r < rows; r++){
//	for (double r = rows - 1; r >= 0; r--){
		double v = 1 - r / rows;			
		for (double c = 0; c < cols; c++){
//			double u = 1.0 - c / cols;		
			double u = c / cols;
			#ifdef APERTURE
			Ray ray;
			camera.transmit(u, v, ray);	
			Vec3 focusInter = ray.o + objectDist / dot(ray.d, cameraFront) * ray.d;
			IntersectResult result;
			Color color = rayTraceRecursive(ray, result, 0, GLOSSYSAMPLETIME, GLOSSYSAMPLEDOWN);		
			//	aperture sample 
			const double sampt = 299;
			for (int i = 0; i < sampt; i++){
				Ray newRay;
				double lroffs = prorand.exrand01();
				double udoffs = prorand.exrand01() * (1.0 - lroffs * lroffs);
				newRay.o = ray.o + camera.aperture  * lroffs * cameraRight + camera.aperture * udoffs * cameraUp;
				newRay.d = focusInter - newRay.o;
				Vec3::normalize(newRay.d);
				result.dist = INF;
				color = color + rayTraceRecursive(newRay, result, 0, GLOSSYSAMPLETIME, GLOSSYSAMPLEDOWN);
			}
			color = 1.0 / (sampt + 1.0) * color;
			img.pixel(r, c) = color.imgcolor();
			
			#else
			Ray ray;
			camera.transmit(u, v, ray);	
			IntersectResult result;
			Color color = rayTraceRecursive(ray, result, 0, GLOSSYSAMPLETIME, GLOSSYSAMPLEDOWN);		
			//	supersample for the boundary between two objects
			#ifdef SUPERSAMPLE
			if (result.pri != lastObject){
				lastObject = result.pri;
				result.dist = INF;
				ray = camera.transmit(u - colsDeviation, v - rowsDeviation);
				color = color + rayTraceRecursive(ray, result, 0, GLOSSYSAMPLETIME, GLOSSYSAMPLEDOWN);
				result.dist = INF;
				ray = camera.transmit(u + colsDeviation, v - rowsDeviation);
				color = color + rayTraceRecursive(ray, result, 0, GLOSSYSAMPLETIME, GLOSSYSAMPLEDOWN);
				color = 0.33 * color;
			}
			#endif
			img.pixel(r, c) = color.imgcolor();
			#endif
		}
		img.show("fifth");
		img.waitKey(10);
	}
}
#endif
#endif