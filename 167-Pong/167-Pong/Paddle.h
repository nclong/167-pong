#include "Ball.h"

//Screen Size : 1200 x 500

namespace
{
	const int STARTING_X_POS = 60; 
	const int STARTING_Y_POS = 250; 

	const int PADDLE_WIDTH = 20; 
	const int PADDLE_HEIGHT = 50; 
}

class Paddle
{
public:
	Paddle() : x_pos (STARTING_X_POS), y_pos(STARTING_Y_POS), width (PADDLE_WIDTH), height(PADDLE_HEIGHT) {};

	void moveUp(int delta_y = 1);

	int get_y_pos();
	int get_x_pos();
	bool isColliding(Ball ball);

private:
	int y_pos;
	int x_pos;

	int width;
	int height;
};
