#include "Entity.h"
#include "Ball.h"
#include "Vector2.h"

//Screen Size : 1200 x 500
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 500

namespace
{
	const int STARTING_X_POS = 60; 
	const int STARTING_Y_POS = 250; 
	const Vector2 STARTING_POSITION(STARTING_X_POS, STARTING_X_POS);

	const int PADDLE_WIDTH = 20; 
	const int PADDLE_HEIGHT = 50; 

	const int DEFAULT_SPEED = 1;
}

class Paddle : Entity
{
public:
	Paddle() : Entity(STARTING_POSITION, PADDLE_WIDTH, PADDLE_HEIGHT, "Paddle"), speed(DEFAULT_SPEED) {};

	void moveUp();
	void moveDown();

private:

	int speed;

};
