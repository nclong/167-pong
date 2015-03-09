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

	if (position.x < paddle1->position.x + PADDLE_WIDTH && ((position.y > paddle1->Top() && position.y < paddle1->Bottom()) || Bottom() > paddle1->Top() && Bottom() < paddle1->Bottom()))
	{
		position.x = position.x + PADDLE_WIDTH;
		velocity.x *= randFactor * -1;
	}

	if (position.x + 60 > paddle2->position.x && ((position.y > paddle2->Top() && position.y < paddle2->Bottom()) || Bottom() > paddle2->Top() && Bottom() < paddle2->Bottom()))
	{
		position.x = paddle2->position.x - 60;
		velocity.x *= randFactor * -1;
	}

	if (Top() < 0)
	{
		position.y = 0;
		velocity.y *= -1;
	}

	if (position.y + 80 >SCREEN_HEIGHT)
	{
		//Bottom Wall Collion
		position.y = SCREEN_HEIGHT - 80;
		velocity.y *= -1;
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
	
	if (position.x > SCREEN_WIDTH)
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

	if (velocity.y == 0)
	{
		velocity.y = 1;
	}
}