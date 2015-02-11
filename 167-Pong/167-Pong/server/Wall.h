#include "Entity.h"
#include "Ball.h"
#include "Vector2.h"

class Wall : Entity
{
public:
	Wall(int topLeft_x, int topLeft_y, int w, int h, std::string wall_name) : Entity(Vector2(topLeft_x, topLeft_y), w, h, wall_name){};
};
