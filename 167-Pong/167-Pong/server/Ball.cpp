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

void Ball::BallUpdate(Paddle* paddle1, Paddle* paddle2, Wall t, Wall b)
{

	position += velocity;
	int randFactor = rand() % 3 + 1;

	if (Left() < paddle1->Right() && ((position.y > paddle1->Top() && position.y < paddle1->Bottom()) || Bottom() > paddle1->Top() && Bottom() < paddle1->Bottom()))
	{
		position.x = paddle1->Right();
		velocity.x *= randFactor * -1;
	}

	if (Right() > paddle2->Left() && ((position.y > paddle2->Top() && position.y < paddle2->Bottom()) || Bottom() > paddle2->Top() && Bottom() < paddle2->Bottom()))
	{
		position.x = paddle2->Left() - BALL_WIDTH;
		velocity.x *= randFactor * -1;
	}

	if (Top() < t.Bottom())
	{
		position.y = t.Bottom();
		velocity.y *= randFactor * -1;
	}

	if (Bottom() > b.Top())
	{
		//Bottom Wall Collion
		position.y = b.Top() - height;
		velocity.y *= randFactor * -1;
	}


	if (position.x < 0)
	{
		//Player 2 Needs to score
		position = startingPos;
		while (velocity.x == 0)
		{
			velocity.x = rand() % 6 - 3;
		}
		velocity.y = rand() % 2 + 1;
		PlayerManager::Players[1]->score++;
	}
	
	if (position.x + BALL_WIDTH > SCREEN_WIDTH)
	{
		//Player 1 Needs to score
		position = startingPos;
		velocity.x = rand() % 6 - 3;
		while (velocity.x == 0)
		{
			velocity.x = rand() % 6 - 3;
		}
		velocity.y = rand() % 3 + 1;
		PlayerManager::Players[0]->score++;
	}
}