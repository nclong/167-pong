class Vector2
{
	public:
		Vector2();
		Vector2(int x, int y);
		~Vector2();
		int x;
		int y;
		Vector2 operator+(Vector2 u);
		void operator+=(Vector2 u);

};