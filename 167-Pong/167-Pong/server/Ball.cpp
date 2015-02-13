#pragma once
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

Vector2 Ball::GetPos()
{
	return position;
}
void Ball::SetPos(int x, int y)
{
	position.x = x;
	position.y = y;
}
void Ball::BallUpdate(Paddle paddle, Wall t, Wall b, Wall r)
{

	position += velocity;
	//for (int i = 0; i < EntityManager::AllEntities.size(); ++i)
	//{
	//	Entity* currentEnt = &EntityManager::AllEntities[i];
	//	if (currentEnt->name.compare(name) != 0)
	//	{
	//		//Collision Detection Goes here
	//		//top Collision
	//		//Reverse y, increase speed
	//		if (Top() < currentEnt->Bottom() && (Left() < currentEnt->Right() || Right() > currentEnt->Left()))
	//		{
	//			//Top Collision
	//			position.y += currentEnt->Bottom() - Top();
	//			velocity.y *= -1;
	//			addScore(currentEnt);
	//		}
	//		else if (Bottom() > currentEnt->Top() && (Left() < currentEnt->Right() || Right() > currentEnt->Left()))
	//		{
	//			//Bottom Collision
	//			position.y -= Bottom() - currentEnt->Top();
	//			velocity.y *= -1;
	//			addScore(currentEnt);
	//		}
	//		else if (Left() < currentEnt->Right() && (Top() < currentEnt->Bottom() || Bottom() > currentEnt->Top()))
	//		{
	//			//Left Collision
	//			position.x += currentEnt->Right() - Left();
	//			velocity.x *= -1;
	//			addScore(currentEnt);
	//		}
	//		else if (Right() < currentEnt->Left() && (Top() < currentEnt->Bottom() || Bottom() > currentEnt->Top()))
	//		{
	//			//Right Collision
	//			position.x -= Right() - currentEnt->Left();
	//			velocity.x *= -1;
	//			addScore(currentEnt);
	//		}
	//	}
	//}
	int randFactor = rand() & 2 + 1;

	if (Left() < paddle.Right() && ((position.y > paddle.Top() && position.y < paddle.Bottom()) || Bottom() > paddle.Top() && Bottom() < paddle.Bottom()))
	{
		position.x = paddle.Right();
		velocity.x *= randFactor * -1;
		PlayerManager::consecutive_hits++;
		PlayerManager::score++;
	}

	if (Top() < t.Bottom())
	{
		position.y = t.Bottom();
		velocity.y *= randFactor * -1;
	}

	if (Right() > r.Left())
	{
		position.x = r.Left() - width;
		velocity.x *= randFactor * -1;
	}

	if (Bottom() > b.Top())
	{
		//Bottom Wall Collion
		position.y = b.Top() - height;
		velocity.y *= randFactor * -1;
	}


	if (position.x < 0)
	{
		position = startingPos;
		velocity.x = rand() % 6 - 3;
		velocity.y = rand() % 2 + 1;
		PlayerManager::consecutive_hits = 0;
		PlayerManager::failures++;
	}
}

void Ball::addScore(Entity* collider)
{
}