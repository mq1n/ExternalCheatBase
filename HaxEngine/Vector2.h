#pragma once
#include <cmath>

typedef struct Vector2_t
{
	template<class T>
	constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
		return clamp(v, lo, hi, std::less<>());
	}
	template<class T, class Compare>
	constexpr const T& clamp(const T& v, const T& lo, const T& hi, Compare comp)
	{
		return assert(!comp(hi, lo)),
			comp(v, lo) ? lo : comp(hi, v) ? hi : v;
	}

	/*
	Data poitns
	*/
	float x, y;

	/*
	Constructors
	*/

	Vector2_t()
	{
		this->x = this->y = 0;
	}

	Vector2_t(float scalar)
	{
		this->x = scalar;
		this->y = scalar;
	}

	Vector2_t(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	/*
	Vector Assignment
	*/

	Vector2_t& operator=(const Vector2_t& rhs)
	{
		this->x = rhs.x;
		this->y = rhs.y;
		return *this;
	}

	Vector2_t& operator+=(const Vector2_t& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}

	Vector2_t& operator-=(const Vector2_t& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		return *this;
	}

	Vector2_t& operator*=(const Vector2_t& rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
		return *this;
	}

	Vector2_t& operator/=(const Vector2_t& rhs)
	{
		this->x /= rhs.x;
		this->y /= rhs.y;
		return *this;
	}

	Vector2_t& operator%=(const Vector2_t& rhs)
	{
		this->x = fmod(this->x, rhs.x);
		this->y = fmod(this->y, rhs.y);
		return *this;
	}

	/*
	Vector Arithmetic
	*/

	Vector2_t operator+(const Vector2_t& rhs)
	{
		return Vector2_t(this->x + rhs.x, this->y + rhs.y);
	}

	Vector2_t operator-(const Vector2_t& rhs)
	{
		return Vector2_t(this->x - rhs.x, this->y - rhs.y);
	}

	Vector2_t operator*(const Vector2_t& rhs)
	{
		return Vector2_t(this->x * rhs.x, this->y * rhs.y);
	}

	Vector2_t operator/(const Vector2_t& rhs)
	{
		return Vector2_t(this->x / rhs.x, this->y / rhs.y);
	}

	Vector2_t operator%(const Vector2_t& rhs)
	{
		return Vector2_t(fmod(this->x, rhs.x), fmod(this->y, rhs.y));
	}

	/*
	Vector Comparisons
	*/

	bool operator==(const Vector2_t& rhs)
	{
		return (this->x == rhs.x && this->y == rhs.y);
	}

	bool operator!=(const Vector2_t& rhs)
	{
		return (this->x != rhs.x && this->y != rhs.y);
	}

	bool operator>(const Vector2_t& rhs)
	{
		return (this->x > rhs.x && this->y > rhs.y);
	}

	bool operator<(const Vector2_t& rhs)
	{
		return (this->x < rhs.x && this->y < rhs.y);
	}

	bool operator>=(const Vector2_t& rhs)
	{
		return (this->x >= rhs.x && this->y >= rhs.y);
	}

	bool operator<=(const Vector2_t& rhs)
	{
		return (this->x <= rhs.x && this->y <= rhs.y);
	}

	/*
	Other Vector Operations
	*/

	float GetX() { return x; };
	float GetY() { return y; };
	bool IsValid() { return x && y; };

	float Length()
	{
		return std::sqrt(std::pow(this->x, 2) + std::pow(this->y, 2));
	}

	float Distance(const Vector2_t& rhs)
	{
		return std::sqrt(std::pow(this->x - rhs.x, 2) + std::pow(this->y - rhs.y, 2));
	}

	void Normalize()
	{
		float flLength = this->Length();
		if (flLength == 0)
			return;

		this->x /= flLength;
		this->y /= flLength;
	}

	bool IsNAN()
	{
		return std::isnan(this->x) || std::isnan(this->y);
	}

	/*
	Clamp Functions
	*/

	void Clamp(Vector2_t & rhs)
	{
		this->x = clamp(std::remainder(rhs.x, 180.f), -rhs.x, +rhs.x);
		this->y = clamp(std::remainder(rhs.y, 360.f), -rhs.y, +rhs.y);
	}
} Vector2;
