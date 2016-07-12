all: main
run: all
	./main
main: h cpp libs
	g++ main.cpp trace.cpp kdnode.cpp debug.cpp box.cpp ray.cpp light.cpp -o main `cat opencv_libs` -std=c++11 -O2
h: switchmacro.h kdnode.h triangle.h myrandom.h box.h primitive.h light.h myvector.h ray.h scene.h sphere.h trace.h intersectresult.h camera.h object.h mymat.h opencv.h material.h plane.h
cpp: main.cpp trace.cpp box.cpp ray.cpp light.cpp kdnode.cpp
inc: pathtrace.inc raytrace.inc
libs: opencv_libs

clear:
	rm .o 
