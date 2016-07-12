#pragma once
#ifdef PATHTRACE
const Color rayTraceRecursive(const Ray& ray, IntersectResult& result, int recursiveDepth, int E = 1)
{
	double in_out = findNearest(ray, result);
	if (in_out == 0)
		return black;
	Vec3 nl = in_out * result.fxl;
	Color color = result.pri->material()->color;
	const Color& e = result.pri->material()->getEmission();
	double p = (color.r > color.g && color.r > color.b) ? color.r : (color.g > color.b ? color.g : color.b);
	if (recursiveDepth + 1 > TRACEMAXRECUR || p == 0)
		//	Russian Roulette
		if (prorand.rand01() < p)
			color = (1.0 / p) * color;
		else
			return E * e;
	IntersectResult resultNext;
	switch(result.pri->material()->getBRDFType()){
	case DIFF:	{
		//	angle around
		double r1 = 2 * MYPI * prorand.rand01();
		//	distance from center
		double r2 = prorand.rand01();
		double r2s = sqrt(r2);
		//	w:z	u:x	v:y
		Vec3 w = nl;
		Vec3 u = cross(fabs(w.x) > 0.1 ? Vec3(0, 1, 0) : Vec3(1, 0, 0), w);
		Vec3::normalize(u);
		Vec3 v = cross(w, u);
		//	random reflection ray
		Vec3 d = (cos(r1) * r2s * u + sin(r1) * r2s * v + sqrt(1 - r2) * w);
		Vec3::normalize(d);
		Vec3 el;
		for (auto& it : scene.pltv){
			const Scene::Sphere& po = it->get();
			const Vec3& sp = po.getCenter();
			//	create random direction towards sphere using method from realistic ray tracing
			//	sw su sv: coordinate system for sampling  
			Vec3 sw = sp - result.inter;
			Vec3 su = cross((fabs(sw.x) > .1 ? Vec3(0, 1, 0) : Vec3(1, 0, 0)), sw);
			Vec3::normalize(su);
			Vec3 sv = cross(sw, su);
			//	max angle
			double cos_a_max = sqrt(1 - po.getR2() / dot(result.inter - sp, result.inter - sp));
			//	calculate sample direction based on random numbers according to equation from
			//	realistic ray tracing:
			//	dir = u*cos(phi)*sin(alpha) + v*sin(phi)*sin(alpha) + w*cos(alpha)
			double eps1 = prorand.rand01(), eps2 = prorand.rand01();
			double cos_a = 1 - eps1 + eps1 * cos_a_max;
			double sin_a = sqrt(1 - cos_a * cos_a);
			double phi = 2 * MYPI * eps2;
			Vec3 L = cos(phi) * sin_a * su + sin(phi) * sin_a * sv + cos_a * sw;
			Vec3::normalize(L);
			Ray shadowRay(result.inter + DEVIATION * L, L);
			IntersectResult temp;
			findNearest(shadowRay, temp);	

			if (temp.pri != it)
				continue;
			double omega = 2 * MYPI * (1 - cos_a_max);
			el = el + (MY_1_PI * omega * dot(L, nl)) * (color * (po.material()->getEmission()));
		}
		Color got = rayTraceRecursive(Ray(result.inter + DEVIATION * d, d), resultNext, recursiveDepth + 1, 0);
		//	E is the emissive term which turn on only in the first recursive 
		return E * e + el + color * got;
		}
	case SPEC:{
		Vec3 nd = ray.d - 2.0 * dot(ray.d, result.fxl) * result.fxl;
		Color got = rayTraceRecursive(Ray(result.inter + DEVIATION * nd, nd), resultNext, recursiveDepth + 1);			
		return e + color * got;
		}
	}

	Vec3 nd = ray.d - 2.0 * dot(ray.d, result.fxl) * result.fxl;
	Ray reflRay(result.inter + DEVIATION * nd, nd);
	double tIndex = result.pri->material()->getRefraIndex();
	double nnt = (in_out == 1) ? (1.0 / tIndex) : (tIndex / 1.0);
	double ddn = dot(ray.d, nl), sqrcost;
	// Total internal reflection
	if ((sqrcost = 1 - nnt * nnt * (1 - ddn * ddn)) < 0)    
		return e + color * rayTraceRecursive(reflRay, resultNext, recursiveDepth + 1); 
	
	Vec3 tdir = (nnt * ray.d - (in_out * (ddn * nnt + sqrt(sqrcost))) * result.fxl);
	Vec3::normalize(tdir); 

	//	schlick's approximation
	double a = tIndex - 1.0;
	double b = tIndex + 1.0;
	double c = 1 - (in_out == 1 ? -ddn : dot(tdir, result.fxl));   
	double R0 = a * a / (b * b);
	double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re;
	double P = 0.25 + 0.5 * Re, RP = Re / P, TP = Tr / (1.0 - P); 
	IntersectResult resultNext1;
	return e + color * (recursiveDepth > 1 
		? 
		(prorand.rand01() < P ? 
		RP * rayTraceRecursive(reflRay, resultNext, recursiveDepth + 1) : 
		TP * rayTraceRecursive(Ray(result.inter + DEVIATION * tdir, tdir), resultNext, recursiveDepth + 1)) 
		: 
	Re * rayTraceRecursive(reflRay, resultNext, recursiveDepth + 1) + 
	Tr * rayTraceRecursive(Ray(result.inter + DEVIATION * tdir, tdir), resultNext1, recursiveDepth + 1)); 
}
#ifdef THREAD
void render(MyMat& img, Camera& camera)
{
	double rows = img.row();
	double cols = img.col();
	const int samps = 1;
	for (double r = rows - 1; r >= 0; r--){
//	for (double r = 0; r < rows; r++){
		fprintf(stderr,"\rRendering (%d spp) %5.2f%%",samps*4,100.*r/(rows-1)); 
		for (double c = 0; c < cols; c++){
			Color color;
			Color temp;
			Color temp1;
			Color temp2;
			Color temp3;

			for (int i = 0; i < samps; i++){
				double r1, r2, dc, dr, v, u;
				
				r1 = 2 * prorand.rand01(), dc = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1); 
				r2 = 2 * prorand.rand01(), dr = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2); 
				v = 1.0 - (r + (0.0 + .5 + dr) / 2) / rows;
				u = (c + (0.0 + .5 + dc) / 2) / cols;							
				Ray ray;						
				camera.transmit(u
				 	, v
				 	, ray, 0);


				r1 = 2 * prorand.rand01(), dc = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1); 
				r2 = 2 * prorand.rand01(), dr = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2); 
				v = 1.0 - (r + (0.0 + .5 + dr) / 2) / rows;
				u = (c + (1.0 + .5 + dc) / 2) / cols;							
				Ray ray1;						
				camera.transmit(u
				 	, v
				 	, ray1, 0);	


				r1 = 2 * prorand.rand01(), dc = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1); 
				r2 = 2 * prorand.rand01(), dr = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2); 
				v = 1.0 - (r + (1.0 + .5 + dr) / 2) / rows;
				u = (c + (0.0 + .5 + dc) / 2) / cols;							
				Ray ray2;						
				camera.transmit(u
				 	, v
				 	, ray2, 0);


				r1 = 2 * prorand.rand01(), dc = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1); 
				r2 = 2 * prorand.rand01(), dr = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2); 
				v = 1.0 - (r + (1.0 + .5 + dr) / 2) / rows;
				u = (c + (1.0 + .5 + dc) / 2) / cols;													
				Ray ray3;						
				camera.transmit(u
				 	, v
				 	, ray3, 0);	
				        

				IntersectResult result;
				IntersectResult result1;
				IntersectResult result2;
				IntersectResult result3;

				std::future<const Color> color1(std::async(std::launch::async, rayTraceRecursive, std::cref(ray), std::ref(result), 0, 1));
				std::future<const Color> color2(std::async(std::launch::async, rayTraceRecursive, std::cref(ray1), std::ref(result1), 0, 1));
				std::future<const Color> color3(std::async(std::launch::async, rayTraceRecursive, std::cref(ray2), std::ref(result2), 0, 1));
				std::future<const Color> color4(std::async(std::launch::async, rayTraceRecursive, std::cref(ray3), std::ref(result3), 0, 1));

//         		temp = temp + (1. / samps) * (color1.get() + color2.get() + color3.get() + color4.get());			
				temp = temp + (1. / samps) * color1.get();
				temp1 = temp1 + (1. / samps) * color2.get();
				temp2 = temp2 + (1. / samps) * color3.get();
				temp3 = temp3 + (1. / samps) * color4.get();			

				color = color + temp.clampColor() + temp1.clampColor() + temp2.clampColor() + temp3.clampColor();
			}
			color = 0.25 * color;
			img.pixel(r, c) = color.imgcolor();
			for (int i = 0; i < 3; i++)
				img.pixel(r, c)[i] = pow(img.pixel(r, c)[i], 1.0 / 2.2);
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
	const int samps = 1;
	for (double r = rows - 1; r >= 0; r--){
//	for (double r = 0; r < rows; r++){
		fprintf(stderr,"\rRendering (%d spp) %5.2f%%",samps*4,100.*r/(rows-1)); 
		for (double c = 0; c < cols; c++){
			Color color;
			for (int sr = 0; sr < 2; sr++)
				for (int sc = 0; sc < 2; sc++){
					Color temp;
					for (int i = 0; i < samps; i++){
						double r1 = 2 * prorand.rand01(), dc = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1); 
        				double r2 = 2 * prorand.rand01(), dr = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2); 
					//	double theta = prorand.rand01() * 2 * MYPI;
					//	double v = 1 - (r + (sr + .5 + dr * sin(theta)) / 2) / rows;
					//	double u = 1 - (c + (sc + .5 + dc * cos(theta)) / 2) / cols;					
						double v = 1 - (r + (sr + .5 + dr) / 2) / rows;
						double u = (c + (sc + .5 + dc) / 2) / cols;							
						Ray ray;						
						camera.transmit(u
						 	, v
						 	, ray, 0);	
						        
						IntersectResult result;
                 		temp = temp + (1. / samps) * rayTraceRecursive(ray, result, 0); 
					}
					color = color + temp.clampColor();
				}
			color = 0.25 * color;
			img.pixel(r, c) = color.imgcolor();
			//	gamma correction
			for (int i = 0; i < 3; i++)
				img.pixel(r, c)[i] = pow(img.pixel(r, c)[i], 1.0 / 2.2);
		}
		img.show("fifth");
		img.waitKey(10);
	}
}
#endif
#endif