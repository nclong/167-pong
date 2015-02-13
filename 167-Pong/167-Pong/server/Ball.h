#pragma once
#include "Vector2.h"
#include "Entity.h"
#include "PlayerManager.h"
#include "Paddle.h"
#include "Wall.h"
#include <random>

class Ball : public Entity
{
public:
	Ball();
	Ball(std::string startName);
	Ball(Vector2 initialPosition);
	Ball(Vector2 initialPosition, std::string startName);
	~Ball();
	Vector2 GetPos();
	void SetPos(int x, int y);
	int GetWidth();
	int GetHeight();
	int SetWidth(int w);
	int SetHeight(int h);
	Vector2 velocity;
	Vector2 startingPos;
	Vector2 startingVel;
	int Speed();
	void BallUpdate(Paddle paddle, Wall t, Wall b, Wall r);

private:
	int speed;
	void addScore(Entity* collider);
};