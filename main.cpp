#include <iostream>
#include <raylib.h>
#include <math.h>
#include <string>
#include <cstdio>
 
struct Ball
{
	float x, y;
	float speedX, speedY;
	float radius;

	void Draw()
	{
		DrawCircle((int) x, (int) y, radius, WHITE);
	}
};

struct Paddle
{
	float x, y;
	float speed;
	float width, height;
	float swingVel;
	float xDefault;

	Rectangle GetRect()
	{
		return Rectangle{ x - width / 2, y - height / 2,  width, height };
	}

	void Draw()
	{
		DrawRectangleRec(GetRect(), WHITE);
	}

	// Brings the paddle back to the idle state after a swing
	void BringBack()
	{
		swingVel += (xDefault - x) * 0.1;
		swingVel = swingVel * 0.8;
		x += swingVel;
	}
};

int main() 
{
	InitWindow(800, 600, "Pong");
	SetWindowState(FLAG_VSYNC_HINT);

	Ball ball;
	ball.x = GetScreenWidth() / 2.0f;
	ball.y = GetScreenHeight() / 2.0f;
	ball.radius = 5;
	ball.speedX = 100;
	ball.speedY = 300;

	Paddle leftPaddle;
	leftPaddle.x = 50;
	leftPaddle.y = GetScreenHeight() / 2;
	leftPaddle.width = 15;
	leftPaddle.height = 100;
	leftPaddle.speed = 500;
	leftPaddle.xDefault = leftPaddle.x;
	leftPaddle.swingVel = 0;

	Paddle rightPaddle;
	rightPaddle.x = GetScreenWidth() - 50;
	rightPaddle.y = GetScreenHeight() / 2;
	rightPaddle.width = 15;
	rightPaddle.height = 100;
	rightPaddle.speed = 500;
	rightPaddle.xDefault = rightPaddle.x;
	rightPaddle.swingVel = 0;


	while (!WindowShouldClose())
	{
		ball.x += ball.speedX * GetFrameTime();
		ball.y += ball.speedY * GetFrameTime();

		if (ball.y < 0)
		{
			ball.y = 0;
			ball.speedY *= -1;
		}

		if (ball.y > GetScreenHeight())
		{
			ball.y = GetScreenHeight();
			ball.speedY *= -1;
		}

		if (IsKeyDown(KEY_W))
		{
			leftPaddle.y -= leftPaddle.speed * GetFrameTime();
		}
		if (IsKeyDown(KEY_S))
		{
			leftPaddle.y += leftPaddle.speed * GetFrameTime();
		}
		if (IsKeyPressed(KEY_D))
		{
			leftPaddle.swingVel += 10;
		}


		if (IsKeyDown(KEY_UP))
		{
			rightPaddle.y -= rightPaddle.speed * GetFrameTime();
		}
		if (IsKeyDown(KEY_DOWN))
		{
			rightPaddle.y += rightPaddle.speed * GetFrameTime();
		}
		if (IsKeyPressed(KEY_LEFT))
		{
			rightPaddle.swingVel += -10;
		}

		if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, 
			leftPaddle.GetRect()) && ball.speedX < 0)
		{
			std::cout << ball.speedX << "\n";
			ball.speedX *= -1;
			ball.speedX += fabsf(ball.speedX) / ball.speedX * 10.0f;
		}
		if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius,
			rightPaddle.GetRect()) && ball.speedX > 0)
		{
			std::cout << ball.speedX << "\n";
			ball.speedX *= -1;
			ball.speedX += fabsf(ball.speedX) / ball.speedX * 10.0f;
		}

		BeginDrawing();
			ClearBackground(BLACK);

			ball.Draw();
			leftPaddle.Draw();
			leftPaddle.BringBack();
			rightPaddle.Draw();
			rightPaddle.BringBack();
			DrawText(TextFormat("%f", abs(ball.speedX)), 100, 10, 20, BLUE);
			DrawFPS(10, 10);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}