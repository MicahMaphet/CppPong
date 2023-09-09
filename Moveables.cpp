#include <raylib.h>

class Moveable
{
	public:
		float x, y;
		float xSpeed, ySpeed;
		void Draw() { }
};

class MoveableRect : Moveable
{
	public:
		float x, y;
		float xSpeed, ySpeed;
		float width, height;
		// Returns, the height, width, x, and y of the rectangle
		Rectangle GetRect()
		{
			return Rectangle{ x - width / 2, y - height / 2,  width, height };
		}
		// Draws the rectangle of the paddle
		void Draw()
		{
			DrawRectangleRec(GetRect(), WHITE);
		}
};

class MoveableCircle : Moveable
{
	public:
		float radius;
		// Render the circle
		void Draw()
		{
			DrawCircle((int)x, (int)y, radius, WHITE);
		}
};