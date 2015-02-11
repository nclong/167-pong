#include "Vector2.h"
#include "Entity.h"

class Ball : Entity
{
public:
	Ball();
	Ball(std::string startName);
	Ball(Vector2 initialPosition);
	Ball(Vector2 initialPosition, std::string startName);
	~Ball();
	Vector2 velocity;
	Vector2 startingPos;
	Vector2 startingVel;
	int Speed();
	void Update();

private:
	int speed;
	void addScore(Entity* collider);
};