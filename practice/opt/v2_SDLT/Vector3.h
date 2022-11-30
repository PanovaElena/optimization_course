#pragma once
#include <cmath>
#include <sdlt/sdlt.h>


using FP = double;  // double or float (floating point type)


struct Vector3 {
	FP x = (FP)0.0, y = (FP)0.0, z = (FP)0.0;

	Vector3(FP x = (FP)0.0, FP y = (FP)0.0, FP z = (FP)0.0) :
		x(x), y(y), z(z) {}

	Vector3& operator+=(const Vector3& v) {
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		return *this;
	}
	
	Vector3& operator-=(const Vector3& v) {
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		return *this;
	}
	
	Vector3& operator*=(const FP& c) {
		this->x *= c;
		this->y *= c;
		this->z *= c;
		return *this;
	}
	
	friend Vector3 operator+(const Vector3& v1, const Vector3& v2) {
		Vector3 res;
		res.x = v1.x + v2.x;
		res.y = v1.y + v2.y;
		res.z = v1.z + v2.z;
		return res;
	}
	
	friend Vector3 operator-(const Vector3& v1, const Vector3& v2) {
		Vector3 res;
		res.x = v1.x - v2.x;
		res.y = v1.y - v2.y;
		res.z = v1.z - v2.z;
		return res;
	}
	
	friend Vector3 operator*(const Vector3& v, const FP& c) {
		Vector3 res;
		res.x = v.x * c;
		res.y = v.y * c;
		res.z = v.z * c;
		return res;
	}

	friend Vector3 operator/(const Vector3& v, const FP& c) {
		Vector3 res;
		res.x = v.x / c;
		res.y = v.y / c;
		res.z = v.z / c;
		return res;
	}
	
	friend Vector3 operator*(const FP& c, const Vector3& v) {
		return v * c;
	}

	FP getNorm() const {
		return sqrt(x * x + y * y + z * z);
	}

	friend Vector3 cross(const Vector3& v1, const Vector3& v2) {
		return Vector3(
			v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x
		);
	}
};

SDLT_PRIMITIVE(Vector3, x, y, z)
