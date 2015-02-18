#pragma once
#include<string>
#include"PlayerInfo.h"
class PlayerManager
{
public:
	PlayerManager(){};
	~PlayerManager(){};

	static void Init_PlayerManager();
	static PlayerInfo* Players[2];
	static int playerCount;
};

//id
//flag
//score

