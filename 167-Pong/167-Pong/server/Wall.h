#pragma once
#include "Entity.h"
#include "Vector2.h"

class Wall : public Entity
{
public:
	Wall();
	Wall(int topLeft_x, int topLeft_y, int w, int h, std::string wall_name) 
		: Entity(Vector2(topLeft_x, topLeft_y), w, h, wall_name){};
	~Wall();
	Vector2 GetPos();
	void SetPos(int x, int y);
	int GetWidth();
	int GetHeight();
	void SetWidth(int w);
	void SetHeight(int h);
};
