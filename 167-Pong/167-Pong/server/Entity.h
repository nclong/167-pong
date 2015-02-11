#pragma once
#include "Vector2.h"
class Entity
{
public:
	Vector2 position;
	Entity();
	~Entity();

	int height;
	int width;
	int Top();
	int Bottom();
	int Left();
	int Right();
	void Update();


};

