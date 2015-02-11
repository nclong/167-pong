#include "Vector2.h"

Vector2::Vector2()
{
}

Vector2::Vector2(int x, int y)
{
	Vector2::x = x;
	Vector2::y = y;
}

Vector2::Vector2()
{
}

Vector2 Vector2::operator+(Vector2 u)
{
	Vector2 result;
	result.x = x + u.x;
	result.y = y + u.y;
	return result;
}

void Vector2::operator+=(Vector2 u)
{
	x += u.x;
	y += u.y;
}