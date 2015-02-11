#include "PlayerManager.h"


void PlayerManager::add_score()
{
	score += 1;
	consecutive_hits += 1;
}

void PlayerManager::add_failure()
{
	failures += 1;
	consecutive_hits = 0;
}

std::string PlayerManager::id;
bool PlayerManager::flag;
int PlayerManager::score;
int PlayerManager::failures;
int PlayerManager::consecutive_hits;