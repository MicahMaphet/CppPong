#include <iostream>
#include <raylib.h>
#include <math.h>
#include <string>
#include <cstdio>
#include "Moveables.cpp"

/**
  * This is the structure of the ball, it has the vector space, x and y
  * speed, and radius.
  */
struct Ball
{
	float x, y;
	float xSpeed, ySpeed;
	float radius;

	// Render the circle
	void Draw()
	{
		DrawCircle((int) x, (int) y, radius, WHITE);
	}
};
/**
  * This structure is used for the two paddles, it has the vector space,
  * height, width, xSpeed (which is used for determining how fast the
  * paddle is moving in the x-axis), and xDefault (which is where the paddle
  * oscilates back twoards in the x-axis)
  */

struct Paddle
{
	float x, y;
	float speed;
	float width, height;
	float xSpeed;
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
		xSpeed += (xDefault - x) * 0.1f;
		xSpeed = xSpeed * 0.8f;
		x += xSpeed;
	}
};

class Particle : public Moveable 
{

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
	ball.xSpeed = 100;
	ball.ySpeed = 300;
	// Initialize the leftPaddle member
	Paddle leftPaddle;
	leftPaddle.x = 125;
	leftPaddle.y = GetScreenHeight() / 2.0f;
	leftPaddle.width = 15;
	leftPaddle.height = 100;
	leftPaddle.speed = 500;
	leftPaddle.xDefault = leftPaddle.x; 
	leftPaddle.xSpeed = 0;
	// Initialize the rightPaddle member
	Paddle rightPaddle;
	rightPaddle.x = GetScreenWidth() - 125.0f;
	rightPaddle.y = GetScreenHeight() / 2.0f;
	rightPaddle.width = 15;
	rightPaddle.height = 100;
	rightPaddle.speed = 500;
	rightPaddle.xDefault = rightPaddle.x;
	rightPaddle.xSpeed = 0;
	// The most recent swing force on impact of the ball
	float swingForce = 0;

	// Game loop until the window is clozed
	while (!WindowShouldClose())
	{
		// Move the ball by the setySpeedin x and y, multipied by frame time
		ball.x += ball.xSpeed * GetFrameTime();
		ball.y += ball.ySpeed * GetFrameTime();
		// Top screen collision
		if (ball.y < 0)
		{
			ball.y = 0;
			ball.ySpeed *= -1;
		}
		// Bottom screen collision
		if (ball.y > GetScreenHeight())
		{
			ball.y = (float) GetScreenHeight();
			ball.ySpeed *= -1;
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
			leftPaddle.xSpeed += 20;
		}
		// Hold paddle back
		if (IsKeyDown(KEY_A))
		{
			leftPaddle.xSpeed = -10;
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
			rightPaddle.xSpeed -= 20;
		}
		// Hold paddle back
		if (IsKeyDown(KEY_RIGHT))
		{
			rightPaddle.xSpeed = 10;
		}

		// Left paddle and ball collision 
		if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, 
			leftPaddle.GetRect()) && ball.xSpeed < 0)
		{
			std::cout << ball.xSpeed << "\n";
			ball.xSpeed *= -1;
			ball.xSpeed += leftPaddle.xSpeed;
			swingForce = leftPaddle.xSpeed;
			//ball.xSpeed += fabsf(ball.xSpeed) / ball.xSpeed * 10.0f;
		}
		// Right paddle and ball collision
		if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius,
			rightPaddle.GetRect()) && ball.xSpeed > 0)
		{
			std::cout << ball.xSpeed << "\n";
			ball.xSpeed *= -1;
			ball.xSpeed += rightPaddle.xSpeed;
			swingForce = rightPaddle.xSpeed;
			//ball.xSpeed += fabsf(ball.xSpeed) / ball.xSpeed * 10.0f;
		}

		BeginDrawing(); // Begin drawing the window
			ClearBackground(BLACK); // Make the background colour black

			ball.Draw(); // Render the ball
			leftPaddle.Draw(); // Render the left paddle
			leftPaddle.BringBack(); // Oscilate the left paddle back to the origin
			rightPaddle.Draw(); // Render the right paddle
			rightPaddle.BringBack(); // Oscilate the right padle
			// Displays theySpeedof the ball
			DrawText(TextFormat("Ball Speed: %f", abs(ball.xSpeed)), 100, 10, 20, BLUE);
			// Displays theySpeedof the last paddle to hit the ball on contact
			DrawText(TextFormat("Swing Force: %f", swingForce), 400, 10, 20, BLUE);
			DrawFPS(10, 10); // Displays the fps
		EndDrawing(); // Finish frame
	}

	CloseWindow(); // When the game loop is done, close the window

	return 0;
}