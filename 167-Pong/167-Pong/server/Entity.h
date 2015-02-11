#pragma once
#include "Vector2.h"
#include <string>
class Entity
{
public:
	Vector2 position;
	Entity();
	Entity(Vector2 pos, int w, int h, std::string name) : position(pos), width(w), height(h), name(name) {};
	~Entity();

	std::string name;
	int height;
	int width;
	int Top();
	int Bottom();
	int Left();
	int Right();
	void Update();


};

