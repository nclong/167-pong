#pragma once
#include<string>
class PlayerManager
{
public:
	PlayerManager(){};
	~PlayerManager(){};

	static void Init_PlayerManager();

	static void add_score();
	static void add_failure();

	static std::string id;
	static bool flag;
	static int score;
	static int failures;
	static int consecutive_hits;

};

//id
//flag
//score

