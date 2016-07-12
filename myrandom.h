#ifndef MYRANDOM_H_
#define MYRANDOM_H_
#include <cstdlib>
class MyRandom
{
public:
	MyRandom()
	{
		randMax = RAND_MAX;
		randMaxHalf = randMax / 2;
		std::srand(time(0));
	}
	double rand01()
	{
		double a = std::rand();
		return a / randMax;
	}
	double exrand01()
	{
		double a = std::rand() - randMaxHalf;
		return a / randMaxHalf;
	}
	double exrand01half()
	{
		return exrand01() * 0.5;
	}
private:
	double randMaxHalf, randMax;
};
#endif