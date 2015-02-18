#pragma once
#include<string>
#include"Paddle.h"
struct PlayerInfo
{
	int score;
	std::string userName;
	int clientId;
	Paddle* paddle;
	Paddle::MovementDirection clientMovementDirection;
	bool packetReceived = false;
};

