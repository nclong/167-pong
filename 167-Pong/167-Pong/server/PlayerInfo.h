#pragma once
#include<string>
#include"Paddle.h"
class PlayerInfo
{
public:
	PlayerInfo();
	~PlayerInfo();

	int score;
	std::string userName;
	int clientId;
	Paddle paddle;
	Paddle::MovementDirection movementDirection;
	Paddle::MovementDirection clientMovementDirection;
	bool packetReceived = false;
};

