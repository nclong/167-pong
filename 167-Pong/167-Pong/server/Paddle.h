#include "Entity.h"
#include "Ball.h"
#include "Vector2.h"

//Screen Size : 1200 x 500

namespace
{
	const int STARTING_X_POS = 60; 
	const int STARTING_Y_POS = 250; 
	const Vector2 STARTING_POSITION(STARTING_X_POS, STARTING_X_POS);

	const int PADDLE_WIDTH = 20; 
	const int PADDLE_HEIGHT = 50; 
}

class Paddle : Entity
{
public:
	Paddle() : Entity(STARTING_POSITION, PADDLE_WIDTH, PADDLE_HEIGHT) {};

	void moveUp(int delta_y = 1);

	int get_y_pos();
	int get_x_pos();
	bool isColliding(Ball ball);

private:

};
