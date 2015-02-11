#include "Entity.h"



Entity::Entity()
{
}


Entity::~Entity()
{
}

int Entity::Top()
{
	return position.y;
}

int Entity::Bottom()
{
	return position.y + height;
}

int Entity::Left()
{
	return position.x;
}

int Entity::Right()
{
	return position.x + width;
}

void Entity::Update()
{

}
