#pragma once
#include <vector>
#include "Entity.h"
class EntityManager
{
public:
	EntityManager();
	~EntityManager();
	static std::vector<Entity> AllEntities;
	static void AddEntity(Entity toAdd);
};

