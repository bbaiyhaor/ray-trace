#ifndef MYVECTOR_H_
#define MYVECTOR_H_
#include "opencv.h"
#include <cmath>

class Vec3
{
	typedef cv::Vec3d Vec3d;
public:
	explicit Vec3(double x = 0, double y = 0, double z = 0):
	x(x), y(y), z(z){}

	bool operator==(const Vec3& other)const
	{
		return ((x == other.x) && 
				(y == other.y) &&
				(z == other.z));
	}

    inline friend const Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
    {
        return Vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }

    inline friend const Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
    {
        return Vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }

    inline friend const Vec3 operator*(double lhs, const Vec3& rhs)
    {
    	return Vec3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
    }

    inline friend const Vec3 operator*(const Vec3& lhs, const Vec3& rhs)
    {
    	return Vec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
    }

    inline friend double dot(const Vec3& lhs, const Vec3& rhs)
    {
    	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    inline friend const Vec3 cross(const Vec3& lhs, const Vec3& rhs)
	{
		return Vec3(lhs.y * rhs.z - lhs.z * rhs.y, 
			lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
	}

	static Vec3& normalize(Vec3& v)
	{
		double o_length = v.x * v.x + v.y * v.y + v.z * v.z;	
		if (std::fabs(o_length - 1) > 1e-6){
			o_length = std::sqrt(o_length);
			v.x /= o_length;
			v.y /= o_length;
			v.z /= o_length;
		}
		return v;
	}

	static const Vec3 normalize(const Vec3& v)
	{
		Vec3 ret(v);
		normalize(ret);
		return ret;
	}

	Vec3 rotate(const Vec3& axis , double theta)
	{
		Vec3 ret;
		double cost = cos( theta );
		double sint = sin( theta );
		ret.x += x * ( axis.x * axis.x + ( 1 - axis.x * axis.x ) * cost );
		ret.x += y * ( axis.x * axis.y * ( 1 - cost ) - axis.z * sint );
		ret.x += z * ( axis.x * axis.z * ( 1 - cost ) + axis.y * sint );
		ret.y += x * ( axis.y * axis.x * ( 1 - cost ) + axis.z * sint );
		ret.y += y * ( axis.y * axis.y + ( 1 - axis.y * axis.y ) * cost );
		ret.y += z * ( axis.y * axis.z * ( 1 - cost ) - axis.x * sint );
		ret.z += x * ( axis.z * axis.x * ( 1 - cost ) - axis.y * sint );
		ret.z += y * ( axis.z * axis.y * ( 1 - cost ) + axis.x * sint );
		ret.z += z * ( axis.z * axis.z + ( 1 - axis.z * axis.z ) * cost );
		return ret;
	}
	
	static void rotate(Vec3& p, const Vec3& axis, double theta)
	{

		double cost = cos( theta );
		double sint = sin( theta );
		p = Vec3(p.x * ( axis.x * axis.x + ( 1 - axis.x * axis.x ) * cost )
		 		+ p.y * ( axis.x * axis.y * ( 1 - cost ) - axis.z * sint )
		 		+ p.z * ( axis.x * axis.z * ( 1 - cost ) + axis.y * sint ),
		p.x * ( axis.y * axis.x * ( 1 - cost ) + axis.z * sint )
		 		+ p.y * ( axis.y * axis.y + ( 1 - axis.y * axis.y ) * cost )
		 		+ p.z * ( axis.y * axis.z * ( 1 - cost ) - axis.x * sint ),
		p.x * ( axis.z * axis.x * ( 1 - cost ) - axis.y * sint )
		 		+ p.y * ( axis.z * axis.y * ( 1 - cost ) + axis.x * sint )
		 		+ p.z * ( axis.z * axis.z + ( 1 - axis.z * axis.z ) * cost ));
	
	}

	const Vec3d imgcolor() const
	{
		double R = r > 1 ? 1 : r;
		R = R < 0 ? 0 : R;		
		double G = g > 1 ? 1 : g;
		G = G < 0 ? 0 : G;
		double B = b > 1 ? 1 : b;
		B = B < 0 ? 0 : B;
		return Vec3d(B, G, R);
	}

	const Vec3 clampColor() const
	{
		double R = r > 1 ? 1 : r;
		R = R < 0 ? 0 : R;		
		double G = g > 1 ? 1 : g;
		G = G < 0 ? 0 : G;
		double B = b > 1 ? 1 : b;
		B = B < 0 ? 0 : B;
		return Vec3(R, G, B);
	}
	union
	{
		struct 
		{
			double x, y, z;
		};
		struct 
		{
			double r, g, b;
		};
		struct 
		{
			double elem[3];
		};
	};
	
};
typedef Vec3 Color;
#endif