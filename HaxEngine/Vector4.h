#pragma once
#include <cmath>

typedef struct Vector4_t
{
	/*
	Data poitns
	*/
	float x, y, z, w;

	/*
	Constructors
	*/

	Vector4_t()
	{
		this->x = this->y = this->z = this->w = 0;
	}

	Vector4_t(float scalar)
	{
		this->x = scalar;
		this->y = scalar;
		this->z = scalar;
		this->w = scalar;
	}

	Vector4_t(float i, float j, float k, float l)
	{
		this->x = i;
		this->y = j;
		this->z = k;
		this->w = l;
	}

	/*
	Vector Assignment
	*/

	Vector4_t& operator=(const Vector4_t& rhs)
	{
		this->x = rhs.x;
		this->y = rhs.y;
		this->z = rhs.z;
		this->w = rhs.w;
		return *this;
	}

	Vector4_t& operator+=(const Vector4_t& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		this->w += rhs.w;
		return *this;
	}

	Vector4_t& operator-=(const Vector4_t& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		this->w -= rhs.w;
		return *this;
	}

	Vector4_t& operator*=(const Vector4_t& rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
		this->z *= rhs.z;
		this->w *= rhs.w;
		return *this;
	}

	Vector4_t& operator/=(const Vector4_t& rhs)
	{
		this->x /= rhs.x;
		this->y /= rhs.y;
		this->z /= rhs.z;
		this->w /= rhs.w;
		return *this;
	}

	Vector4_t& operator%=(const Vector4_t& rhs)
	{
		this->x = fmod(this->x, rhs.x);
		this->y = fmod(this->y, rhs.y);
		this->z = fmod(this->z, rhs.z);
		this->w = fmod(this->w, rhs.w);
		return *this;
	}

	/*
	Vector Arithmetic
	*/

	Vector4_t operator+(const Vector4_t& rhs)
	{
		return Vector4_t(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z, this->w + rhs.w);
	}

	Vector4_t operator-(const Vector4_t& rhs)
	{
		return Vector4_t(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z, this->w - rhs.w);
	}

	Vector4_t operator*(const Vector4_t& rhs)
	{
		return Vector4_t(this->x * rhs.x, this->y * rhs.y, this->z * rhs.z, this->w * rhs.w);
	}

	Vector4_t operator/(const Vector4_t& rhs)
	{
		return Vector4_t(this->x / rhs.x, this->y / rhs.y, this->z / rhs.z, this->w / rhs.w);
	}

	Vector4_t operator%(const Vector4_t& rhs)
	{
		return Vector4_t(fmod(this->x, rhs.x), fmod(this->y, rhs.y), fmod(this->z, rhs.z), fmod(this->w, rhs.w));
	}

	/*
	Vector Comparisons
	*/

	bool operator==(const Vector4_t& rhs)
	{
		return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w);
	}

	bool operator!=(const Vector4_t& rhs)
	{
		return (this->x != rhs.x && this->y != rhs.y && this->z != rhs.z && this->w != rhs.w);
	}

	bool operator>(const Vector4_t& rhs)
	{
		return (this->x > rhs.x && this->y > rhs.y && this->z > rhs.z && this->w > rhs.w);
	}

	bool operator<(const Vector4_t& rhs)
	{
		return (this->x < rhs.x && this->y < rhs.y && this->z < rhs.z && this->w < rhs.w);
	}

	bool operator>=(const Vector4_t& rhs)
	{
		return (this->x >= rhs.x && this->y >= rhs.y && this->z >= rhs.z && this->w >= rhs.w);
	}

	bool operator<=(const Vector4_t& rhs)
	{
		return (this->x <= rhs.x && this->y <= rhs.y && this->z <= rhs.z && this->w <= rhs.w);
	}
} Vector4;

