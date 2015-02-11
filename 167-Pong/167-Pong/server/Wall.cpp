#pragma once
#include "Wall.h"
Wall::Wall()
{

}

Wall::~Wall()
{

}
Vector2 Wall::GetPos()
{
	return position;
}
void Wall::SetPos(int x, int y)
{
	position.x = x; position.y = y;
}
int Wall::GetWidth()
{
	return width;
}
int Wall::GetHeight()
{
	return height;
}
void Wall::SetWidth(int w)
{
	width = w;
}
void Wall::SetHeight(int h)
{
	height = h;
}