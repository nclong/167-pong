#include "Paddle.h"

void Paddle::moveUp()
{
	int next_pos = position.y - speed;

	if (next_pos >= 0)
		position.y = next_pos;
}

void Paddle::moveDown()
{
	int next_pos = position.y + speed;

	if(next_pos + height <= SCREEN_HEIGHT)
		position.y = next_pos;

}



