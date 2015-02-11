#include "Vector2.h"
#include "Entity.h"

class Ball : Entity
{
	public:
		Ball();
		Ball(Vector2 initialPosition);
		~Ball();
		Vector2 velocity;
		int Speed();
		void Update();

private:
	int speed;
};