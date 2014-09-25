#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <math.h>
#include <D3DX10math.h>

class Vector{
public:
	float x, y, z;
	Vector() {}
	Vector(float x, float y, float z) : x(x), y(y), z(z) {}
	Vector(const Vector &v) : x(v.x), y(v.y), z(v.z) {}
	bool operator==(const Vector& rhs) const
    {
		return x == rhs.x && y == rhs.y && z == rhs.z;
    }
	bool operator!=(const Vector& rhs) const
    {
		return x != rhs.x || y != rhs.y || z != rhs.z;
    }

	D3DXVECTOR3 d3dvector() {return D3DXVECTOR3((float)x, (float)y, (float)z); }
	
	friend Vector operator*(const Vector& v, const float& rhs) { return Vector(v.x*rhs, v.y*rhs, v.z*rhs); }

	friend Vector operator*(const float& lhs, const Vector& v) { return Vector(v.x *lhs, v.y*lhs, v.z*lhs); }

	Vector operator/(const float& rhs) const { return Vector(x/rhs, y/rhs, z/rhs); }

	Vector operator+(const Vector& v) const {return Vector(x+v.x, y+v.y, z+v.z); }

	Vector operator-(const Vector& v) const {return Vector(x-v.x, y-v.y, z-v.z); }

	float operator*(const Vector& v) const {return x*v.x + y*v.y +z*v.z; }

	Vector cross(const Vector& v) const {return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x); }

	float length() const {return sqrt(x*x + y*y + z*z); }
};




#endif