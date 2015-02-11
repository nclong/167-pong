#include "EntityManager.h"


EntityManager::EntityManager()
{
}


EntityManager::~EntityManager()
{
}

void EntityManager::AddEntity(Entity* toAdd)
{
	EntityManager::AllEntities.push_back(*toAdd);
}

std::vector<Entity> EntityManager::AllEntities;
