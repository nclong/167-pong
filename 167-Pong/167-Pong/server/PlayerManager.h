#pragma once
#include<string>
#include"PlayerInfo.h"
class PlayerManager
{
public:
	PlayerManager(){};
	~PlayerManager(){};

	static void Init_PlayerManager();
	static PlayerInfo Players[2];
	static void add_score(int player);

private:
	static int playerCount;

};

//id
//flag
//score

