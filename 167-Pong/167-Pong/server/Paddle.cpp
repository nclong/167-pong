#include "Paddle.h"

void Paddle::moveUp()
{
	int next_pos = position.y - PADDLE_SPEED;

	if (next_pos < 0)
		position.y = 0;
	else
		position.y = next_pos;
}

void Paddle::moveDown()
{
	int next_pos = position.y + PADDLE_SPEED;

	if (next_pos + height > SCREEN_HEIGHT)
		position.y = SCREEN_HEIGHT - height;
	else
		position.y = next_pos;
}

void Paddle::Update()
{
	if (dir == MOVING_UP)
	{
		position.y -= PADDLE_SPEED;
	}
	else if (dir == MOVING_DOWN)
	{
		position.y += PADDLE_SPEED;
	}

	if (position.y < 0)
	{
		position.y = 0;
	}

	if (position.y + PADDLE_HEIGHT > SCREEN_HEIGHT - HORIZ_WALL_HEIGHT)
	{
		position.y = SCREEN_HEIGHT - HORIZ_WALL_HEIGHT - PADDLE_HEIGHT;
	}
}

Vector2 Paddle::GetPos()
{
	return position;
}
void Paddle::SetPos(int x, int y)
{
	position.x = x; position.y = y;
}
int Paddle::GetWidth()
{
	return width;
}
int Paddle::GetHeight()
{
	return height;
}
void Paddle::SetWidth(int w)
{
	width = w;
}
void Paddle::SetHeight(int h)
{
	height = h;
}



