#ifndef TRACE_H_
#define TRACE_H_

class Camera;
class MyMat;
class Ray;
#include "box.h"
#include "object.h"
#include <array>
using std::array;
namespace Trace
{
	void render(MyMat&, Camera&);
	void initial();
	extern Object scene;
};

#endif