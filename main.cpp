#include <iostream>
#include <raylib.h>
#include <math.h>
#include <string>
#include <cstdio>

/**
  * This is the structure of the ball, it has the vector space, x and y
  * speed, and radius.
  */
struct Ball
{
	float x, y;
	float speedX, speedY;
	float radius;

	// Render the circle
	void Draw()
	{
		DrawCircle((int) x, (int) y, radius, WHITE);
	}
};
/**
  * This structure is used for the two paddles, it has the vector space,
  * height, width, swingVel (which is used for determining how fast the
  * paddle is moving in the x-axis), and xDefault (which is where the paddle
  * oscilates back twoards in the x-axis)
  */

struct Paddle
{
	float x, y;
	float speed;
	float width, height;
	float swingVel;
	float xDefault;
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
	// Initializing the window, dimensions 800x600, and named Pong
	InitWindow(800, 600, "Pong");
	// Stops the render rate from exceding the render rate of the device
	SetWindowState(FLAG_VSYNC_HINT);
	// Initialize the ball member
	Ball ball;
	ball.x = GetScreenWidth() / 2.0f;
	ball.y = GetScreenHeight() / 2.0f;
	ball.radius = 5;
	ball.speedX = 100;
	ball.speedY = 300;
	// Initialize the leftPaddle member
	Paddle leftPaddle;
	leftPaddle.x = 125;
	leftPaddle.y = GetScreenHeight() / 2;
	leftPaddle.width = 15;
	leftPaddle.height = 100;
	leftPaddle.speed = 500;
	leftPaddle.xDefault = leftPaddle.x;
	leftPaddle.swingVel = 0;
	// Initialize the rightPaddle member
	Paddle rightPaddle;
	rightPaddle.x = GetScreenWidth() - 125;
	rightPaddle.y = GetScreenHeight() / 2;
	rightPaddle.width = 15;
	rightPaddle.height = 100;
	rightPaddle.speed = 500;
	rightPaddle.xDefault = rightPaddle.x;
	rightPaddle.swingVel = 0;
	// The most recent swing force on impact of the ball
	float swingForce = 0;

	// Game loop until the window is clozed
	while (!WindowShouldClose())
	{
		// Move the ball by the set speed in x and y, multipied by frame time
		ball.x += ball.speedX * GetFrameTime();
		ball.y += ball.speedY * GetFrameTime();
		// Top screen collision
		if (ball.y < 0)
		{
			ball.y = 0;
			ball.speedY *= -1;
		}
		// Bottom screen collision
		if (ball.y > GetScreenHeight())
		{
			ball.y = GetScreenHeight();
			ball.speedY *= -1;
		}

		// Move left paddle up
		if (IsKeyDown(KEY_W))
		{
			leftPaddle.y -= leftPaddle.speed * GetFrameTime();
		}
		// Move left paddle down
		if (IsKeyDown(KEY_S))
		{
			leftPaddle.y += leftPaddle.speed * GetFrameTime();
		}
		// Swing the paddle
		if (IsKeyPressed(KEY_D))
		{
			leftPaddle.swingVel += 20;
		}
		// Hold paddle back
		if (IsKeyDown(KEY_A))
		{
			leftPaddle.swingVel = -10;
		}

		// Move right paddle up
		if (IsKeyDown(KEY_UP))
		{
			rightPaddle.y -= rightPaddle.speed * GetFrameTime();
		}
		// Move right paddle down
		if (IsKeyDown(KEY_DOWN))
		{
			rightPaddle.y += rightPaddle.speed * GetFrameTime();
		}
		// Swing right paddle
		if (IsKeyPressed(KEY_LEFT))
		{
			rightPaddle.swingVel -= 20;
		}
		// Hold paddle back
		if (IsKeyDown(KEY_RIGHT))
		{
			rightPaddle.swingVel = 10;
		}

		// Left paddle and ball collision 
		if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, 
			leftPaddle.GetRect()) && ball.speedX < 0)
		{
			std::cout << ball.speedX << "\n";
			ball.speedX *= -1;
			ball.speedX += leftPaddle.swingVel;
			swingForce = leftPaddle.swingVel;
			//ball.speedX += fabsf(ball.speedX) / ball.speedX * 10.0f;
		}
		// Right paddle and ball collision
		if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius,
			rightPaddle.GetRect()) && ball.speedX > 0)
		{
			std::cout << ball.speedX << "\n";
			ball.speedX *= -1;
			ball.speedX += rightPaddle.swingVel;
			swingForce = rightPaddle.swingVel;
			//ball.speedX += fabsf(ball.speedX) / ball.speedX * 10.0f;
		}

		BeginDrawing(); // Begin drawing the window
			ClearBackground(BLACK); // Make the background colour black

			ball.Draw(); // Render the ball
			leftPaddle.Draw(); // Render the left paddle
			leftPaddle.BringBack(); // Oscilate the left paddle back to the origin
			rightPaddle.Draw(); // Render the right paddle
			rightPaddle.BringBack(); // Oscilate the right padle
			// Displays the speed of the ball
			DrawText(TextFormat("Ball Speed: %f", abs(ball.speedX)), 100, 10, 20, BLUE);
			// Displays the speed of the last paddle to hit the ball on contact
			DrawText(TextFormat("Swing Force: %f", swingForce), 400, 10, 20, BLUE);
			DrawFPS(10, 10); // Displays the fps
		EndDrawing(); // Finish frame
	}

	CloseWindow(); // When the game loop is done, close the window

	return 0;
}