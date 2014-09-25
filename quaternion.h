#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include <math.h>
#include "vector.h"

class Quaternion{
public:
	float x, y, z, w;
	Quaternion() {}
	Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Quaternion(const Quaternion &q) : x(q.x), y(q.y), z(q.z), w(q.w) {}
	Quaternion(Vector v, float angle) : x(v.x*sin(angle/2)), y(v.y*sin(angle/2)), z(v.z*sin(angle/2)), w(cos(angle/2)) {}

	
	Quaternion(Vector u, Vector v) {
		u = u / u.length();
		v = v / v.length();
		float dot = u*v;
		if (dot >= 1.0){  //same direction
			w = 1;
			x = 0;
			y = 0;
			z = 0;
			return;
		}
		if (dot < (1e-6f - 1.0)){  //opposite direction
			w = 0;
			x = 1;
			y = 0;
			z = 0;
			return;
		}
		float s = sqrt((1+dot)*2);
		float invs = 1 / s;
		Vector cross = u.cross(v);
		x = cross.x * invs;
		y = cross.y *invs;
		z = cross.z * invs;
		w = s * 0.5f;
		float length = sqrt(w*w + x*x + y*y + z*z);
		w = w/length;
		x = x/length;
		y = y/length;
		z = z/length;
	}

	bool operator==(const Quaternion& rhs) const
    {
		return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
    }

	bool operator!=(const Quaternion& rhs) const { return ! (*this == rhs); }

	Quaternion operator*(const Quaternion& q) const{
		return Quaternion(w*q.x + x*q.w + y*q.z - z*q.y, w*q.y - x*q.z + y*q.w + z*q.x, 
						  w*q.z + x*q.y - y*q.x + z*q.w, w*q.w - x*q.x - y*q.y - z*q.z);
	}

	Quaternion operator-(const Quaternion& q) const { return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w); }

	Vector operator*(const Vector& v) const { 
		Quaternion q = Quaternion(v.x, v.y, v.z, 0);
		Quaternion ret = Quaternion(x, y, z, w)*q*Quaternion(-1*x, -1*y, -1*z, w);
		return Vector(ret.x, ret.y, ret.z);
	}

	friend Quaternion operator*(const Quaternion& q, const float& rhs) { return Quaternion(q.x*rhs, q.y*rhs, q.z*rhs, q.w*rhs); }

	friend Quaternion operator*(const float& lhs, const Quaternion& q) { return Quaternion(q.x*lhs, q.y*lhs, q.z*lhs, q.w*lhs); }

	Quaternion operator/(const float& rhs) const { return Quaternion(x/rhs, y/rhs, z/rhs, w/rhs); }

	float length(){  return sqrt(x*x + y*y + z*z + w*w); }

	Quaternion inverse(){
		float linv = 1 / sqrt(x*x + y*y + z*z + w*w);

		return Quaternion(-1*x*linv, -1*y*linv, -1*z*linv, w*linv);
	}
};


#endif