#include <iostream>
#include <raylib.h>
#include <math.h>
#include <string>
#include <cstdio>

/**
  * This is the class of the ball, it has the vector space, x and y
  * speed, and radius.
  */
class Ball
{
	public:
		float x, y;
		float xSpeed, ySpeed;
		float radius;
		float friction;

	// Render the circle
	void Draw()
	{
		DrawCircle((int) x, (int) y, radius, WHITE);
	}
};

/**
  * This class is used for the two paddles, it has the vector space,
  * height, width, xSpeed (which is used for determining how fast the
  * paddle is moving in the x-axis), and xDefault (which is where the paddle
  * oscilates back twoards in the x-axis)
  */
class Paddle
{	
	public:
		float x, y;
		float speed;
		float width, height;
		float xSpeed, ySpeed;
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

/**
  * These particles launch off the paddle at the point of impact of the ball.
  * The particles shoot off faster the faster the paddle is moving at the 
  * point of impact of the ball.
  */
class Particle {
public:
	float x, y;
	float xSpeed, ySpeed;
	float radius;
	bool direction; // direction is a boolean, 1 is right, 0 is left
	int launchState;

	// Render the circle
	void Draw()
	{
		DrawCircle((int)x, (int)y, radius, WHITE);
	}
	// Carry out the launch
	void ContinueLaunch() {
		// If the launch faze has not been completed
		if (launchState > 0) {
			xSpeed -= (fabsf(xSpeed) / xSpeed) / 100;
			x += launchState * GetFrameTime() / 5 * ((direction * 2) - 1);
			y += ySpeed * GetFrameTime();
			launchState--;
			/**
			  * If the particle is off the screen, set launch state to 0, ending the launch sequence (which is only
			  * calculated with the x-axis because it never goes off the y-axis since the ball bounces off on those sides)
			  */
			if (x < 0 - radius * 2 || x > GetScreenWidth() + radius * 2) {
				launchState = 0; // end launch sequence
			}
			Draw();
		}
	}
};

int main() 
{	
	// Initializing the window, dimensions 800x600, and named Pong
	InitWindow(1000, 600, "Pong");
	// Stops the render rate from exceding the render rate of the device
	SetWindowState(FLAG_VSYNC_HINT);

	// Initialize the ball member
	Ball ball;
	ball.x = GetScreenWidth() / 2.0f;
	ball.y = GetScreenHeight() / 2.0f;
	ball.radius = 10;
	ball.xSpeed = 100;
	ball.ySpeed = 300;
	ball.friction = 0.025;
	Ball ballInit = ball;

	// Initialize the leftPaddle member
	Paddle leftPaddle;
	leftPaddle.x = 225;
	leftPaddle.y = GetScreenHeight() / 2.0f;
	leftPaddle.width = 50;
	leftPaddle.height = 100;
	leftPaddle.speed = 500;
	leftPaddle.xDefault = leftPaddle.x; 
	leftPaddle.xSpeed = 0;
	// Save the paddle for restarting the game
	Paddle leftPaddleInit = leftPaddle;

	// Initialize the rightPaddle member
	Paddle rightPaddle;
	rightPaddle.x = GetScreenWidth() - 225.0f;
	rightPaddle.y = GetScreenHeight() / 2.0f;
	rightPaddle.width = 50;
	rightPaddle.height = 100;
	rightPaddle.speed = 500;
	rightPaddle.xDefault = rightPaddle.x;
	rightPaddle.xSpeed = 0;
	// Save the paddle for restarting the game
	Paddle rightPaddleInit = rightPaddle;

	// The most recent swing force on impact of the ball
	float swingForce = 0;

	Particle particles[100]; // an array of all the particles
	// how many particles there are
	const int particleCount = sizeof(particles) / sizeof(Particle);
	// Loop through all of the particles and assign default values
	for (int i = 0; i < sizeof(particles) / sizeof(Particle); i++) {
		particles[i].radius = 2;
		particles[i].launchState = 0;
	}
	// This is where the available index's particles are stored
	int firstAvailableParticle = 0;
	// Particle count for the number of particles that launch of a paddle on collision
	int particlesOnHit = 10;
	// The y axis spread of the particles on collision of the paddle
	int particleDiffusion = 50;

	// Game loop until the window is clozed
	while (!WindowShouldClose())
	{
		// Move the ball by the setySpeedin x and y, multipied by frame time
		ball.x += ball.xSpeed * GetFrameTime();
		ball.y += ball.ySpeed * GetFrameTime();
		// Friction for the ball
		ball.xSpeed -= (abs(ball.xSpeed) / ball.xSpeed) * ball.friction;

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

		/* 
		 * If the ball goes off the screen on the left size, reset the ball,
		 * and the left and right paddle.
		 */
		if (ball.x < 0 || ball.x > GetScreenWidth()) {
			// This resets to all the initialized vairables
			ball = ballInit;
			leftPaddle = leftPaddleInit;
			rightPaddle = rightPaddleInit;
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
			leftPaddle.xSpeed += 50;
		}
		// Hold paddle back
		if (IsKeyDown(KEY_A))
		{
			leftPaddle.xSpeed = -20;
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
			rightPaddle.xSpeed -= 50;
		}
		// Hold paddle back
		if (IsKeyDown(KEY_RIGHT))
		{
			rightPaddle.xSpeed = 20;
		}


		// Left paddle and ball collision 
		if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, 
			leftPaddle.GetRect()) && ball.xSpeed < 0)
		{
			ball.xSpeed *= -1;
			ball.xSpeed += leftPaddle.xSpeed;
			swingForce = leftPaddle.xSpeed;

			// Loop through all of the particles that launch on impact
			for (int i = 0; i < particlesOnHit; i++) { 
				for (int j = 0; j < particleCount; j++) { 
					// Check if the launch stat of the particle has been finished
					if (particles[j].launchState == 0) { 
						firstAvailableParticle = j; 
					} 
				}

				particles[firstAvailableParticle].x = ball.x; 
				particles[firstAvailableParticle].y = ball.y; 
				particles[firstAvailableParticle].direction = 1; // right 
				particles[firstAvailableParticle].ySpeed = ((particlesOnHit / 2) * -particleDiffusion) + (i * particleDiffusion); 
				particles[firstAvailableParticle].launchState = abs(ball.xSpeed) * 5 + (swingForce) * 50;
			}
		}

		// Right paddle and ball collision
		if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius,
			rightPaddle.GetRect()) && ball.xSpeed > 0)
		{
			ball.xSpeed *= -1;
			ball.xSpeed += rightPaddle.xSpeed;
			swingForce = rightPaddle.xSpeed;

			// Loop through all of the particles that launch on impact
			for (int i = 0; i < particlesOnHit; i++) {
				for (int j = 0; j < particleCount; j++) {
					// Check if the launch stat of the particle has been finished
					if (particles[j].launchState == 0) {
						firstAvailableParticle = j;
					}
				}

				particles[firstAvailableParticle].x = ball.x;
				particles[firstAvailableParticle].y = ball.y;
				particles[firstAvailableParticle].direction = 0; // left
				particles[firstAvailableParticle].ySpeed = ((particlesOnHit / 2) * -particleDiffusion) + (i * particleDiffusion);
				particles[firstAvailableParticle].launchState = abs(ball.xSpeed) * 5 + (-swingForce) * 50;
			}
		}
		// Loop through all of the particles and draw them
		for (int i = 0; i < sizeof(particles) / sizeof(Particle); i++) {
			particles[i].ContinueLaunch();
		}
		BeginDrawing(); // Begin drawing the window
			ClearBackground(BLACK); // Make the background colour black

			ball.Draw(); // Render the ball
			leftPaddle.Draw(); // Render the left paddle
			leftPaddle.BringBack(); // Oscilate the left paddle back to the origin
			rightPaddle.Draw(); // Render the right paddle
			rightPaddle.BringBack(); // Oscilate the right paddle

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