#include "Ball.h"
#include "EntityManager.h"


Ball::Ball()
{

}

Ball::~Ball()
{

}

Ball::Ball(Vector2 initialPosition)
{
	position = initialPosition;
}
Ball::Ball(std::string startName)
{
	name = startName;
}
Ball::Ball(Vector2 initialPosition, std::string startName)
{
	position = initialPosition;
	name = startName;
}

void Ball::Update()
{
	for (int i = 0; i < EntityManager::AllEntities.size; ++i)
	{
		if (EntityManager::AllEntities[i].name.compare(name) != 0)
		{
			//Collision Detection Goes here

		}
	}
}