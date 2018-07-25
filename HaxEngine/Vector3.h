#pragma once
#define NOMINMAX
#include <cmath>

#define M_PI       3.14159265358979323846   // pi

#define M_RAD2DEG_CONVERSION ( 180.f / M_PI )
#define M_RAD2DEG(rad) float(rad * M_RAD2DEG_CONVERSION)

#define M_DEG2RAD_CONVERSION ( M_PI / 180.f )
#define M_DEG2RAD(deg) float(deg * M_DEG2RAD_CONVERSION)

template<class T>
const T& clamp(const T& x, const T& upper, const T& lower) {
	return (std::min)(upper, (std::max)(x, lower));
}

typedef struct Vector3_t
{
	/*
	Data poitns
	*/
	float x, y, z;

	/*
	Constructors
	*/

	Vector3_t()
	{
		this->x = this->y = this->z = 0;
	}

	Vector3_t(float scalar)
	{
		this->x = scalar;
		this->y = scalar;
		this->z = scalar;
	}

	Vector3_t(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	/*
	Vector Assignment
	*/

	Vector3_t& operator=(const Vector3_t& rhs)
	{
		this->x = rhs.x;
		this->y = rhs.y;
		this->z = rhs.z;
		return *this;
	}

	Vector3_t& operator+=(const Vector3_t& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		return *this;
	}

	Vector3_t& operator-=(const Vector3_t& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		return *this;
	}

	Vector3_t& operator*=(const Vector3_t& rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
		this->z *= rhs.z;
		return *this;
	}

	Vector3_t& operator/=(const Vector3_t& rhs)
	{
		this->x /= rhs.x;
		this->y /= rhs.y;
		this->z /= rhs.z;
		return *this;
	}

	Vector3_t& operator%=(const Vector3_t& rhs)
	{
		this->x = fmod(this->x, rhs.x);
		this->y = fmod(this->y, rhs.y);
		this->z = fmod(this->z, rhs.z);
		return *this;
	}

	/*
	Vector Arithmetic
	*/

	Vector3_t operator+(const Vector3_t& rhs)
	{
		return Vector3_t(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);
	}

	Vector3_t operator-(const Vector3_t& rhs)
	{
		return Vector3_t(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
	}

	Vector3_t operator*(const Vector3_t& rhs)
	{
		return Vector3_t(this->x * rhs.x, this->y * rhs.y, this->z * rhs.z);
	}

	Vector3_t operator/(const Vector3_t& rhs)
	{
		return Vector3_t(this->x / rhs.x, this->y / rhs.y, this->z / rhs.z);
	}

	Vector3_t operator%(const Vector3_t& rhs)
	{
		return Vector3_t(fmod(this->x, rhs.x), fmod(this->y, rhs.y), fmod(this->z, rhs.z));
	}

	/*
	Vector Comparisons
	*/

	bool operator==(const Vector3_t& rhs)
	{
		return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z);
	}

	bool operator!=(const Vector3_t& rhs)
	{
		return (this->x != rhs.x && this->y != rhs.y && this->z != rhs.z);
	}

	bool operator>(const Vector3_t& rhs)
	{
		return (this->x > rhs.x && this->y > rhs.y && this->z > rhs.z);
	}

	bool operator<(const Vector3_t& rhs)
	{
		return (this->x < rhs.x && this->y < rhs.y && this->z < rhs.z);
	}

	bool operator>=(const Vector3_t& rhs)
	{
		return (this->x >= rhs.x && this->y >= rhs.y && this->z >= rhs.z);
	}

	bool operator<=(const Vector3_t& rhs)
	{
		return (this->x <= rhs.x && this->y <= rhs.y && this->z <= rhs.z);
	}

	/*
	Other Vector Operations
	*/

	float GetX() { return x; };
	float GetY() { return y; };
	float GetZ() { return z; };
	bool IsValid() { return x && y; };

	auto IsNull()
	{
		return (x <= 0.0 || y <= 0.0);
	}

	float Length(float Is3D = true)
	{
		return this->Length3D();
	}

	float Length2D()
	{
		return std::sqrt(std::pow(this->x, 2) + std::pow(this->y, 2));
	}

	float Length3D()
	{
		return std::sqrt(std::pow(this->x, 2) + std::pow(this->y, 2) + std::pow(this->z, 2));
	}

	float Distance(const Vector3_t& rhs)
	{
		return std::sqrt(std::pow(this->x - rhs.x, 2) + std::pow(this->y - rhs.y, 2) + std::pow(this->z - rhs.z, 2));
	}

	float Dot(const Vector3_t& rhs)
	{
		return (this->x * rhs.x) + (this->y * rhs.y) + (this->z * rhs.z);
	}

	Vector3_t Cross(const Vector3_t& rhs)
	{
		return Vector3_t(
			((this->y * rhs.z) - (this->z * rhs.y)),
			((this->z * rhs.x) - (this->x * rhs.z)),
			((this->x * rhs.y) - (this->y * rhs.x))
		);
	}

	void Normalize()
	{
		float flLength = this->Length();
		if (flLength == 0)
			return;

		this->x /= flLength;
		this->y /= flLength;
		this->z /= flLength;
	}

	Vector3_t ToAngle()
	{
		Vector3_t vAngle(0.f);

		if (this->x == 0 && this->y == 0)
		{
			vAngle.y = 0;
			if (this->z > 0)
				vAngle.x = 270;
			else
				vAngle.x = 90;
		}
		else
		{
			vAngle.y = float(M_RAD2DEG(atan2(this->y, this->x)));
			if (vAngle.y < 0)
				vAngle.y += 360;

			vAngle.x = float(M_RAD2DEG(atan2(-this->z, this->Length())));
			if (vAngle.x < 0)
				vAngle.x += 360;
		}

		vAngle.z = 0.f;
		return vAngle;
	}

	Vector3_t ToAngleOld()
	{
		Vector3_t vAngle(0.f);
		vAngle.x = (float)(atan(this->z / sqrt(this->x * this->x + this->y * this->y)) * M_RAD2DEG_CONVERSION);
		vAngle.y = (float)(atan(this->y / this->x) * M_RAD2DEG_CONVERSION);
		vAngle.z = 0.f;

		if (this->x >= 0.0)
			vAngle.y += 180.0f;

		return vAngle;
	}

	Vector3_t ToVector()
	{
		float sinYaw = sin(M_DEG2RAD(this->y));
		float cosYaw = cos(M_DEG2RAD(this->y));

		float sinPitch = sin(M_DEG2RAD(this->x));
		float cosPitch = cos(M_DEG2RAD(this->x));

		Vector3_t vVector(0.f);
		vVector.x = cosPitch * cosYaw;
		vVector.y = cosPitch * sinYaw;
		vVector.z = -sinPitch;
		return vVector;
	}

	bool IsNAN()
	{
		return std::isnan(this->x) || std::isnan(this->y) || std::isnan(this->z);
	}

	/*
	Clamp Functions
	*/

	void Clamp(Vector3_t & rhs)
	{
		this->x = clamp(std::remainder(rhs.x, 180.f), -rhs.x, +rhs.x);
		this->y = clamp(std::remainder(rhs.y, 360.f), -rhs.y, +rhs.y);
		this->z = 0.0f;
	}
} Vector3;
