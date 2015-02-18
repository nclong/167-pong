#include "PlayerManager.h"


void PlayerManager::add_score(int player)
{
	Players[player].score++;
}

int PlayerManager::playerCount = 2;

PlayerInfo PlayerManager::Players[2];