#pragma once
#include<string>
class PlayerManager
{
public:
	PlayerManager(std::string player_id, bool player_flag) : id(player_id), flag(player_flag), score(0), failures(0), consecutive_hits(0) {};

	void add_score();
	void add_failure();

	std::string id;
	bool flag;
	int score;
	int failures;
	int consecutive_hits;

};

//id
//flag
//score

