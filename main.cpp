#include "all.h"
#include "opencv.h"
#include <fstream>

using namespace std;
using namespace Trace;
using namespace cv;
typedef Scene::Sphere Sphere;
typedef Scene::Plane Plane;
typedef Scene::PointLight PointLight;
typedef Scene::AreaLight AreaLight;
typedef Scene::Triangle Triangle;
Vec3 obj[500000];
//#define PATHDRAGON
#define DRAGON
#define DIFF 0
#define SPEC 1
#define REFR 2

//#define PATHTRACE
//#define CUBE
void readObj(const char* filename, Object& scene)
{
	ifstream fin(filename);
	int now = 0;
	double minx = INF, maxx = -INF;
	double miny = INF, maxy = -INF;
	double minz = INF, maxz = -INF;

	while (! fin.eof()){
		char ch;
		fin >> ch;
		if (fin.eof())
			break;		
		if (ch == '#'){	//	annotation
			fin >> noskipws;
			while (ch != '\n') fin >> ch;
			fin >> skipws;
		}
		else {		
			if (ch == 'v'){	//	vertex
				now++;
				fin >> obj[now].x >> obj[now].y >> obj[now].z;
				#ifdef DRAGON
				double beishu = 0.3;
				obj[now].x = obj[now].x / beishu;
				obj[now].y = obj[now].y / beishu - 2;
				obj[now].z = obj[now].z / beishu + 6;
				minx = min(minx, obj[now].x);
				maxx = max(maxx, obj[now].x);
				miny = min(miny, obj[now].y);
				maxy = max(maxy, obj[now].y);
				minz = min(minz, obj[now].z);
				maxz = max(maxz, obj[now].z);
				
				#endif
				
				#ifdef PATHDRAGON
				double beishu = 0.05;
				obj[now].x = obj[now].x / beishu + 50;
				obj[now].y = obj[now].y / beishu + 16;
				obj[now].z = obj[now].z / beishu + 60;
			//	cout << obj[now] << endl;
				#endif
				#ifdef CUBE
				double beishu = 1;
				obj[now].x = obj[now].x / beishu;
				obj[now].y = obj[now].y / beishu;
				obj[now].z = obj[now].z / beishu;
		//		cout << obj[now] << endl;
				#endif
			}
			else if (ch == 'f'){	//	face			
				int a, b, c;
				fin >> a >> b >> c;
			//	Vec3 fxl = cross(obj[b] - obj[a], obj[a] - obj[c]);
				Vec3 fxl = cross(obj[c] - obj[a], obj[a] - obj[b]);
				if (fxl.x * fxl.x + fxl.y * fxl.y + fxl.z * fxl.z > 1e-12){
					Vec3::normalize(fxl);		
					#ifdef PATHTRACE
					scene.triv.push_back(new Triangle(obj[a], obj[b], obj[c], fxl, new BRDF(red, Vec3(), REFR, 1.5)));	
					#else
					scene.triv.push_back(new Triangle(obj[a], obj[b], obj[c], fxl, new Phong(red, 0.8, 0.8, 20, 0.0, 0.0, 1.3)));	
					#endif
				}
			}
		}
	}
	fin.close();
}
//	long 27 tuzi 50
#define scene Trace::scene	
Camera camera;
void S1()
{
	//	light
	scene.pltv.push_back(new PointLight(Vec3(0, 5, 5), 0.1, Color(0.4, 0.4, 0.4)));
	scene.pltv.push_back(new PointLight(Vec3(-3, 5, 1), 0.1, Color(0.6, 0.6, 0.6)));
	//	plane
	scene.plav.push_back(new Plane(Vec3(0, 1, 0), 4.4, new Phong(Color(0.4, 0.3, 0.3), 1, 0, 0, 0, 0, 1.5, 0, "wood.png")));
	scene.plav.push_back(new Plane(Vec3(0.4, 0, -1), 12, new Phong(Color(0.5, 0.3, 0.5), 0.6, 0, 0, 0, 0, 1.5, 0, "naruto.jpg")));
	scene.plav.push_back(new Plane(Vec3(0, -1, 0), 7.4, new Phong(Color(0.4, 0.7, 0.7), 0.5, 0, 0, 0, 0, 1.5, 0, "sky.jpg")));
	//	Sphere
	scene.sphv.push_back(new Sphere(Vec3(2, 0.8, 3), 2.5, new Phong(white, 0.2, 0.8, 20, 0, 0.8, 1.3)));
	scene.sphv.push_back(new Sphere(Vec3(-5.5, -0.5, 7), 2, new Phong(Color(0.7, 0.7, 1), 0.1, 0.8, 20, 0.5, 0, 1.3)));
	scene.sphv.push_back(new Sphere(Vec3(-1.5, -3.8, 1), 1.5, new Phong(Color(1, 0.4, 0.4), 0.1, 0.8, 20, 0, 0.8, 1.5)));
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 7; y++){
			scene.sphv.push_back(new Sphere(Vec3(-4.5 + x * 1.5, -4.3 + y * 1.5, 10), 0.3, new Phong(Color(0.3, 1, 0.4), 0.6, 0.6, 20, 0, 0, 1.5)));	
		}
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++){
			scene.sphv.push_back(new Sphere(Vec3(-4.5 + x * 1.5, -4.3, 10.0 - y * 1.5), 0.3, new Phong(Color(0.3, 1, 0.4), 0.6, 0.6, 20, 0, 0, 1.5)));
		}
	for (int x = 0; x < 16; x++)
		for (int y = 0; y < 8; y++){
			scene.sphv.push_back(new Sphere(Vec3(-8.5 + x * 1.5, 4.3, 10.0 - y), 0.3, new Phong(Color(0.3, 1, 0.4), 0.6, 0.6, 20, 0, 0, 1.5)));			
		}
	camera = Camera(Vec3(0, 0, -5), Vec3(0, 0, 1), Vec3(0, 1, 0), 60);
}

void S2()
{
	//	x -1-99
	//	y  0 81
	//	z -5 170
	//left
	scene.plav.push_back(new Plane(Vec3(1, 0, 0), 1, new Phong(Color(.75, .25, .25), 1, 0.01, 20, 0, 0, 1.5, 0)));
	//Rght 
	scene.plav.push_back(new Plane(Vec3(-1, 0, 0), 99, new Phong(Color(.25, .25, .75), 1, 0.01, 20, 0, 0, 1.5, 0)));
	//Back 
	scene.plav.push_back(new Plane(Vec3(0, 0, 1), 5, new Phong(Color(.0, .75, .0), 1, 0.01, 20, 0, 0, 1.5, 0)));
	//Frnt 
	scene.plav.push_back(new Plane(Vec3(0, 0, -1), 170, new Phong(Color(.75, .75, .75), 1, 0.01, 20, 0, 0, 1.5, 0)));
	//Botm 
	scene.plav.push_back(new Plane(Vec3(0, 1, 0), 0, new Phong(Color(.75, .75, .75), 1, 0.01, 20, 0, 0, 1.5, 0)));
	//Top 
	scene.plav.push_back(new Plane(Vec3(0, -1, 0), 81.6, new Phong(Color(.75, .75, .75), 1, 0.01, 20, 0, 0, 1.5, 0)));
	

	//scene.pltv.push_back(new PointLight(Sphere(Vec3(50,81.6-16.5,81.6), 1.5), white));//Lite 

	scene.sphv.push_back(new Sphere(Vec3(15, 25, 85), 12, new Phong(white, 0.2, 0.8, 20, 0.5, 0., 1.3, 0.4)));
	scene.sphv.push_back(new Sphere(Vec3(50, 25, 85), 12, new Phong(white, 0.1, 0.8, 20, 0.0, 0.8, 1.3, 0)));
	scene.sphv.push_back(new Sphere(Vec3(83, 25, 85), 12, new Phong(white, 0.1, 0.8, 20, 0.5, 0.0, 1.3)));
	//arealight
	
	scene.altv.push_back(new AreaLight(AabbBox(Vec3(35,81.6-15.5,70), Vec3(60,81.6-15.5+0.2,95)), Color(1, 1, 1)));

	camera = Camera(Vec3(50, 32, 167.0), Vec3(0, -0.052612, -1), Vec3(0, 1, 0), 60.0);

}

void S3()
{
	//	x -1-99
	//	y  0 81
	//	z -5 170
	//scene.pltv.push_back(new PointLight(Vec3(0, 6, 0), 0.1, Color(1, 1, 1)));
	//scene.pltv.push_back(new PointLight(Vec3(0, 6, -6), 0.1, Color(1, 1, 1)));
	//	plane
	//	ceiling
	scene.plav.push_back(new Plane(Vec3(0, -1, 0), 7, new Phong(white, 1, 0.01, 20, 0, 0, 1.5, 0)));
	//	floor
	scene.plav.push_back(new Plane(Vec3(0, 1, 0), 4.6, new Phong(white, 0.2, 0.01, 20, 0.8, 0, 1.5, 0)));
	//	back
	scene.plav.push_back(new Plane(Vec3(0, 0, -1), 7.2, new Phong(white, 0.2, 0.01, 20, 0.8, 0, 1.5, 0)));
	//	left
	scene.plav.push_back(new Plane(Vec3(1, 0, 0), 5.8, new Phong(white, 0.2, 0.01, 20, 0.8, 0, 1.5, 0)));
	//	right
	scene.plav.push_back(new Plane(Vec3(-1, 0, 0), 5.8, new Phong(green, 1, 0.01, 20, 0, 0, 1.5, 0, "naruto.jpg")));
	//arealight
	scene.altv.push_back(new AreaLight(AabbBox(Vec3(-1, 6.2, 3.3), Vec3(1, 6.3, 4.3)), Color(1, 1, 1)));
	//	eye
	camera = Camera(Vec3(0, 1.2, -2), Vec3(0, 0, 1), Vec3(0, 1, 0), 90);
	
	ifstream fin("c.obj");
	int now = 0;
	while (! fin.eof()){
		char ch;
		fin >> ch;
		if (fin.eof())
			break;		
		if (ch == '#'){	//	annotation
			fin >> noskipws;
			while (ch != '\n') fin >> ch;
			fin >> skipws;
		}
		else {		
			if (ch == 'v'){	//	vertex
				now++;
				fin >> obj[now].x >> obj[now].y >> obj[now].z;
				double beishu = 0.3;
				obj[now].x = obj[now].x / beishu;
				obj[now].y = obj[now].y / beishu - 2;
				obj[now].z = obj[now].z / beishu + 6;			
			}
			else if (ch == 'f'){	//	face			
				int a, b, c;
				fin >> a >> b >> c;
			//	Vec3 fxl = cross(obj[b] - obj[a], obj[a] - obj[c]);
				Vec3 fxl = cross(obj[c] - obj[a], obj[a] - obj[b]);
				if (fxl.x * fxl.x + fxl.y * fxl.y + fxl.z * fxl.z > 1e-12){
					Vec3::normalize(fxl);		
					scene.triv.push_back(new Triangle(obj[a], obj[b], obj[c], fxl, new Phong(red, 0.3, 0.8, 20, 0.0, 0.0, 1.3)));	
				}
			}
		}
	}
	fin.close();
}

void S4()
{
	//	x -1-99
	//	y  0 81
	//	z -5 170
	//left
	scene.plav.push_back(new Plane(Vec3(1, 0, 0), 1, new Phong(Color(.75, .25, .25), 1, 0.01, 20, 0, 0, 1.5, 0)));
	//Rght 
	scene.plav.push_back(new Plane(Vec3(-1, 0, 0), 99, new Phong(Color(.25, .25, .75), 1, 0.01, 20, 0, 0, 1.5, 0)));
	//Back 
	scene.plav.push_back(new Plane(Vec3(0, 0, 1), 5, new Phong(Color(.0, .75, .0), 1, 0.01, 20, 0, 0, 1.5, 0)));
	//Frnt 
	scene.plav.push_back(new Plane(Vec3(0, 0, -1), 170, new Phong(Color(.75, .75, .75), 1, 0.01, 20, 0, 0, 1.5, 0)));
	//Botm 
	scene.plav.push_back(new Plane(Vec3(0, 1, 0), 0, new Phong(Color(.75, .75, .75), 0.2, 0.01, 20, 0.8, 0, 1.5, 0)));
	//Top 
	scene.plav.push_back(new Plane(Vec3(0, -1, 0), 81.6, new Phong(Color(.75, .75, .75), 1, 0.01, 20, 0, 0, 1.5, 0, "sky.jpg")));
	

	scene.pltv.push_back(new PointLight(Sphere(Vec3(50,81.6-16.5,81.6), 1.5), white));//Lite 
	scene.pltv.push_back(new PointLight(Sphere(Vec3(50,80.0,150), 1.5), white));//Lite 

	scene.sphv.push_back(new Sphere(Vec3(17, 40, 85), 10, new Phong(white, 0.8, 0.2, 20, 0., 0., 1.3, 0, "b1.jpg")));
	scene.sphv.push_back(new Sphere(Vec3(43, 30, 105), 10, new Phong(white, 0.8, 0.2, 20, 0., 0., 1.3, 0, "b2.jpg")));
	scene.sphv.push_back(new Sphere(Vec3(62, 20, 125), 10, new Phong(white, 0.8, 0.2, 20, 0., 0.0, 1.3, 0, "b3.jpg")));
	//arealight
	cout << sqrt(dot(Vec3(17, 40, 85) - Vec3(50, 32, 167.0), Vec3(17, 40, 85) - Vec3(50, 32, 167.0))) << endl;
	cout << sqrt(dot(Vec3(43, 30, 105) - Vec3(50, 32, 167.0), Vec3(43, 30, 105) - Vec3(50, 32, 167.0))) << endl;
	cout << sqrt(dot(Vec3(62, 20, 125) - Vec3(50, 32, 167.0),Vec3(62, 20, 125) - Vec3(50, 32, 167.0))) << endl;
	
	//scene.altv.push_back(new AreaLight(AabbBox(Vec3(29,81.6-15.5,60.5), Vec3(71,81.6-15.5+0.2,92.6)), Color(1, 1, 1)));
	camera = Camera(Vec3(50, 32, 167.0), Vec3(0, -0.052612, -1), Vec3(0, 1, 0), 60.0);
}

void S5()
{
	//Left 
	scene.plav.push_back(new Plane(Vec3(1, 0, 0), 1, new BRDF(Color(.25, .25, .75), Vec3(), DIFF)));
	//Rght 
	scene.plav.push_back(new Plane(Vec3(-1, 0, 0), 99, new BRDF(Color(.75, .25, .25), Vec3(), DIFF)));
	//Back 
	scene.plav.push_back(new Plane(Vec3(0, 0, 1), 5, new BRDF(Color(.75, .75, .75), Vec3(), DIFF)));
	//Frnt 
	scene.plav.push_back(new Plane(Vec3(0, 0, -1), 170, new BRDF(Color(.75, .75, .75), Vec3(), DIFF)));
	//Botm 
	scene.plav.push_back(new Plane(Vec3(0, 1, 0), 0, new BRDF(Color(.75, .75, .75), Vec3(), DIFF)));
	//Top 
	scene.plav.push_back(new Plane(Vec3(0, -1, 0), 81.6, new BRDF(Color(.75, .75, .75), Vec3(), DIFF)));
	scene.sphv.push_back(new Sphere(Vec3(25, 50, 60), 8, new BRDF(Color(0.999, 0.999, 0.999), Vec3(), DIFF)));//Mirr 	
	scene.sphv.push_back(new Sphere(Vec3(65, 50, 60), 8, new BRDF(Color(0.999, 0.999, 0.999), Vec3(), SPEC)));//Mirr 
	scene.sphv.push_back(new Sphere(Vec3(45, 70, 40), 16, new BRDF(Color(175.0 / 255.0,238 / 255.0,238 / 255.0), Vec3(), REFR, 1.5)));//Glas 
	
	scene.pltv.push_back(new PointLight(Sphere(Vec3(50,81.6-16.5,81.6), 1.5, new BRDF(Color(), Vec3(400,400,400), DIFF))));//Lite 
	//scene.pltv.push_back(new PointLight(Sphere(Vec3(0, 5, 7.5), .5, new BRDF(Color(), Vec3(400,400,400), DIFF))));
	
	camera = Camera(Vec3(50,52,295.6), Vec3(0,-0.042612,-1), Vec3(1, 0, 0), 60.0, 60.0);	
	ifstream fin("c.obj");
	int now = 0;
	while (! fin.eof()){
		char ch;
		fin >> ch;
		if (fin.eof())
			break;		
		if (ch == '#'){	//	annotation
			fin >> noskipws;
			while (ch != '\n') fin >> ch;
			fin >> skipws;
		}
		else {		
			if (ch == 'v'){	//	vertex
				now++;
				fin >> obj[now].x >> obj[now].y >> obj[now].z;	
				double beishu = 0.04;
				obj[now].x = obj[now].x / beishu + 70;
				obj[now].y = obj[now].y / beishu + 18;
				obj[now].z = obj[now].z / beishu + 60;
			}
			else if (ch == 'f'){	//	face			
				int a, b, c;
				fin >> a >> b >> c;
			//	Vec3 fxl = cross(obj[b] - obj[a], obj[a] - obj[c]);
				Vec3 fxl = cross(obj[c] - obj[a], obj[a] - obj[b]);
				if (fxl.x * fxl.x + fxl.y * fxl.y + fxl.z * fxl.z > 1e-12){
					Vec3::normalize(fxl);		
					scene.triv.push_back(new Triangle(obj[a], obj[b], obj[c], fxl, new BRDF(Color(1,215.0 / 255.0,0), Vec3(), REFR, 1.5)));	
				}
			}
		}
	}
	fin.close();
	fin.open("horse.obj");
	now = 0;
	while (! fin.eof()){
		char ch;
		fin >> ch;
		if (fin.eof())
			break;		
		if (ch == '#'){	//	annotation
			fin >> noskipws;
			while (ch != '\n') fin >> ch;
			fin >> skipws;
		}
		else {		
			if (ch == 'v'){	//	vertex
				now++;
				fin >> obj[now].x >> obj[now].y >> obj[now].z;	
				double beishu = 0.04;
				obj[now].x = obj[now].x / beishu;
				obj[now].y = obj[now].y / beishu;
				obj[now].z = obj[now].z / beishu;
				Vec3::rotate(obj[now], Vec3(0, 1, 0), 90);
				Vec3::rotate(obj[now], Vec3(0, 0, 1), -90);
				Vec3::rotate(obj[now], Vec3(1, 0, 0), 90);
				obj[now].x += 26.0;
				obj[now].y += 24.0;
				obj[now].z += 100.0;				

			}
			else if (ch == 'f'){	//	face			
				int a, b, c;
				fin >> a >> b >> c;
			//	Vec3 fxl = cross(obj[b] - obj[a], obj[a] - obj[c]);
				Vec3 fxl = cross(obj[c] - obj[a], obj[a] - obj[b]);
				if (fxl.x * fxl.x + fxl.y * fxl.y + fxl.z * fxl.z > 1e-12){
					Vec3::normalize(fxl);		
					scene.triv.push_back(new Triangle(obj[a], obj[b], obj[c], fxl, new BRDF(Color(178.0 / 255.0,34.0 / 255.0,34.0 / 255.0), Vec3(), REFR, 1.5)));	
				}
			}
		}
	}
	fin.close();
	cout << "Triangle total: " << scene.triv.size() << endl;
}

void S6()
{
	//	x -1-99
	//	y  0 81
	//	z -5 170
	//scene.pltv.push_back(new PointLight(Vec3(0, 6, 0), 0.1, Color(1, 1, 1)));
	//scene.pltv.push_back(new PointLight(Vec3(0, 6, -6), 0.1, Color(1, 1, 1)));
	//	plane
	//	ceiling
	scene.plav.push_back(new Plane(Vec3(0, -1, 0), 7, new Phong(white, 1, 0.01, 20, 0, 0, 1.5, 0)));
	//	floor
	scene.plav.push_back(new Plane(Vec3(0, 1, 0), 4.6, new Phong(white, 0.2, 0.01, 20, 0.8, 0, 1.5, 0)));
	//	back
	scene.plav.push_back(new Plane(Vec3(0, 0, -1), 7.2, new Phong(white, 0.2, 0.01, 20, 0.8, 0, 1.5, 0)));
	//	left
	scene.plav.push_back(new Plane(Vec3(1, 0, 0), 5.8, new Phong(white, 0.2, 0.01, 20, 0.8, 0, 1.5, 0)));
	//	right
	scene.plav.push_back(new Plane(Vec3(-1, 0, 0), 5.8, new Phong(green, 1, 0.01, 20, 0, 0, 1.5, 0, "naruto.jpg")));
	//arealight
	scene.altv.push_back(new AreaLight(AabbBox(Vec3(-1, 6.2, 3.3), Vec3(1, 6.3, 4.3)), Color(1, 1, 1)));
	//	eye
	camera = Camera(Vec3(0, 1.2, -2), Vec3(0, 0, 1), Vec3(0, 1, 0), 90);
	
	ifstream fin("c.obj");
	int now = 0;
	while (! fin.eof()){
		char ch;
		fin >> ch;
		if (fin.eof())
			break;		
		if (ch == '#'){	//	annotation
			fin >> noskipws;
			while (ch != '\n') fin >> ch;
			fin >> skipws;
		}
		else {		
			if (ch == 'v'){	//	vertex
				now++;
				fin >> obj[now].x >> obj[now].y >> obj[now].z;
				double beishu = 0.3;
				obj[now].x = obj[now].x / beishu;
				obj[now].y = obj[now].y / beishu - 2;
				obj[now].z = obj[now].z / beishu + 6;			
			}
			else if (ch == 'f'){	//	face			
				int a, b, c;
				fin >> a >> b >> c;
			//	Vec3 fxl = cross(obj[b] - obj[a], obj[a] - obj[c]);
				Vec3 fxl = cross(obj[c] - obj[a], obj[a] - obj[b]);
				if (fxl.x * fxl.x + fxl.y * fxl.y + fxl.z * fxl.z > 1e-12){
					Vec3::normalize(fxl);		
					scene.triv.push_back(new Triangle(obj[a], obj[b], obj[c], fxl, new Phong(green, 0.5, 0.5, 20, 0.0, 0.8, 1.5)));	
				}
			}
		}
	}
	fin.close();
}

int main(int argc, char** argv)
{
	MyMat img(600, 600);
	S2();
	initial();
	render(img, camera);
	
	img.show("fourth");
	waitKey(0);
	img.save("fourth.png");
	return 0;
}
