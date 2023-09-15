#include <iostream>
#include <raylib.h>
#include <cmath>
#include <string>
#include <cstdio>

/**
  * Velocity vector with speed and direction
  **/
struct Velocity {
	float speed;
	float direction;
};

/**
  * Triangle class
  * Triangle is computed based on the hypotenuse and angle
  **/
class Triangle {
public:
	float x, y;
	float h; // hypotenuse
	float angle;
	// Set the triangle
	void SetTriangle(float newX, float newY, float newH, float newAngle) {
		x = newX;
		y = newY;
		h = newH;
		angle = newAngle;
	}
	/**
	  * Draws the triangle
	  * Takes in the position, the length fo the hypostenuse and the angle
	  * Draws the opposite, ajecent, and hypotensue of the trianglew
	  **/
	void Draw() {
		// Draw the hypotenuse
		DrawLine(x, y, h * cos(angle) + x, h * sin(angle) + y, WHITE); 
		// Draw agecent side
		DrawLine(x, y, h * cos(angle) + x, y, WHITE);
		// Draw oppositie side
		DrawLine(h * cos(angle) + x, h * sin(angle) + y, h * cos(angle) + x, y, WHITE);
	}
};

/**
  * This is the class of the ball, it has the vector space, x and y
  * speed, and radius.
  **/
class Ball {
	public:
		float x, y;
		float radius;
		float friction;
		Velocity velocity;
	/**
	  * Draws the circle
	  * Updates the x and y speed with the velocity and direction
	  **/
	void Draw()
	{
		x += velocity.speed * cos(velocity.direction) * GetFrameTime();
		y += velocity.speed * sin(velocity.direction) * GetFrameTime();
		DrawCircle((int) x, (int) y, radius, WHITE);
	}
	/**
	  * Change the direction of the paddle
	  * This is in a function to ensure that the direction does not go
	  * below 0, if so it loops past 2PI
	  * If it goes over 2PI it loops past 0
	  **/
	void changeDirection(float changeDirection) {
		velocity.direction += changeDirection;
		if (velocity.direction < 0)
			velocity.direction += 2 * PI;
		while (velocity.direction >= 2 * PI) 
			velocity.direction -= 2 * PI;
	}
	/**
	  * Sets the direction of the paddle
	  * This is in a function to ensure that the direction does not go
	  * below 0, if so it loops past 2PI
	  * If it goes over 2PI it loops past 0
	  **/
	void SetDirection(float newDirection) {
		velocity.direction = newDirection;
		while (velocity.direction < 0)
			velocity.direction += 2 * PI;
		while (velocity.direction >= 2 * PI)
			velocity.direction -= 2 * PI;
	}
};

/**
  * This class is used for the two paddles, it has the vector space,
  * height, width, xSpeed (which is used for determining how fast the
  * paddle is moving in the x-axis), and xDefault (which is where the paddle
  * oscilates back twoards in the x-axis)
  **/
class Paddle {
public:
	float x, y;
	float aimYSpeed;
	float width, height;
	float xDefault;
	Velocity velocity;

	/**
	  * Returns, the height, width, x, and y of the rectangle
	  **/
	Rectangle GetRect() {
		return Rectangle{ x - width / 2, y - height / 2,  width, height };
	}
	/**
	  * Get speed in the x direction of the velocity
	  **/
	float GetXSpeed() {
		return velocity.speed * cos(velocity.direction);
	}
	/**
	  * Get speed in the y direction of the velocity
	  **/
	float GetYSpeed() {
		return velocity.speed * sin(velocity.direction);
	}
	/**
	  * Draws the rectangle of the paddle
	  * Updates the x and y possition to the velocity
	  * Keeps the velocity constant regarless of FPS by multiplying the change is x by
	  * the GetFrameTime() which is the time inbetween each frame.
	  **/
	void Draw() {
		x += GetXSpeed() * GetFrameTime();
		y += GetYSpeed() * GetFrameTime();
		DrawRectangleRec(GetRect(), WHITE);
	}
	/**
	  * Sets the direction of the paddle
	  * This is in a function to ensure that the direction does not go
	  * below 0, if so it loops past 2PI
	  * If it goes over 2PI it loops past 0
	  **/
	void SetDirection(float newDirection) {
		velocity.direction = newDirection;
		while (velocity.direction < 0) 
			velocity.direction += 2 * PI;
		while (velocity.direction >= 2 * PI) 
			velocity.direction -= 2 * PI;
	}
	/**
	  * Sets the y speed in the velocity
	  * Keeps the x speed constant
	  * Modifies the direction and the speed
      **/
	void SetYSpeed(float yOffset) {
		if (!velocity.speed) {
			velocity.speed = yOffset;
			if (yOffset == 0) return;
		}
		velocity.speed = sqrt(pow(GetXSpeed(), 2) + pow(yOffset, 2)); 
		SetDirection(asin(yOffset / velocity.speed));
	}

	/**
	  * Brings the paddle back to the idle state after a swing
	  **/
	void BringBack() { /* Code to be added to bring the paddle back across the X-axis after swing */ }
};

/**
  * These particles launch off the paddle at the point of impact of the ball.
  * The particles shoot off faster the faster the paddle is moving at the 
  * point of impact of the ball.
  **/
class Particle {
public:
	float x, y;
	float xSpeed, ySpeed;
	float radius;
	float direction; // In radians
	int launchState;
	float vectorSpeedMultiplier; // Multiplies the x and y speed of the particles

	/**
	  * Render the particle
	  * Update the position with the x and y speed
	  **/
	void Draw() {
		x += xSpeed;
		y += ySpeed;
		DrawCircle((int)x, (int)y, radius, WHITE);
	}
	/**
	  * Continue the launch, slowing down and calculating movement in direction.
	  **/
	void ContinueLaunch() {
		// If the launch faze has not been completed
		if (launchState > 0) {
			/**
			  * Think os this as a triangle
			  * direction is the angle, in radians
			  * launchState is the hypotenuse
			  * xSpeed is the adjacent angle
			  * ySpeed is the opposite angle
			  * vectorSpeedMultiplier is a flat base speed
			  **/
			xSpeed = launchState * cos(direction) * vectorSpeedMultiplier;
			ySpeed = launchState * sin(direction) * vectorSpeedMultiplier;
			// Decrement the launch state, slows down the particles. If it is 0, the launch stops
			launchState--;
			/**
			  * If the particle is off the screen, set launch state to 0, ending the launch sequence (which is only
			  * calculated with the x-axis because it never goes off the y-axis since the ball bounces off on those sides)
			  **/
			if (x < 0 - radius * 2 || x > GetScreenWidth() + radius * 2)
				launchState = 0; // end launch sequence
			Draw();
		}
	}
};

int main() {	
	// Initializing the window, dimensions 800x600, and named Pong
	InitWindow(1000, 600, "Pong");
	// Stops the render rate from exceding the render rate of the device
	SetWindowState(FLAG_VSYNC_HINT);

	// Initialize the ball member
	Ball ball;
	ball.x = GetScreenWidth() / 2.0f;
	ball.y = GetScreenHeight() / 2.0f;
	ball.radius = 10;
	ball.velocity.speed = 200;
	ball.velocity.direction = PI * 0.75;
	ball.friction = 0.0;
	Ball ballInit = ball;

	// Initialize the leftPaddle member
	Paddle leftPaddle;
	leftPaddle.x = 225;
	leftPaddle.y = GetScreenHeight() / 2.0f;
	leftPaddle.width = 50;
	leftPaddle.height = 100;
	leftPaddle.aimYSpeed = 500; 
	leftPaddle.velocity.speed = 0; 
	leftPaddle.velocity.direction = PI / 2; 
	leftPaddle.xDefault = leftPaddle.x;
	// Save the paddle for restarting the game
	Paddle leftPaddleInit = leftPaddle;

	// Initialize the rightPaddle member
	Paddle rightPaddle;
	rightPaddle.x = GetScreenWidth() - 225.0f;
	rightPaddle.y = GetScreenHeight() / 2.0f;
	rightPaddle.width = 50;
	rightPaddle.height = 100;
	rightPaddle.aimYSpeed = 500;
	rightPaddle.xDefault = rightPaddle.x;
	rightPaddle.velocity.speed = 0;
	rightPaddle.velocity.direction = PI / 2;
	// Save the paddle for restarting the game
	Paddle rightPaddleInit = rightPaddle;

	// The most recent swing force on impact of the ball
	float swingForce = 0;

	Particle particles[100]; // an array of all the particles
	// how many particles there are
	const int particleCount = sizeof(particles) / sizeof(Particle);
	// Loop through all of the particles and assign default values
	for (int i = 0; i < sizeof(particles) / sizeof(Particle); i++) {
		particles[i].radius = 3;
		particles[i].launchState = 0;
		particles[i].vectorSpeedMultiplier = 0.2;
	}
	// This is where the available index's particles are stored
	int firstAvailableParticle = 0;
	// Particle count for the number of particles that launch of a paddle on collision
	int particlesOnHit = 15;

	Triangle lPVT; // Left paddle velocity triangle


	// Game loop until the window is clozed
	while (!WindowShouldClose()) {
		// Move the ball by the setySpeedin x and y, multipied by frame time
		
		// Friction for the ball
		ball.velocity.speed -= (abs(ball.velocity.speed) / ball.velocity.speed) * ball.friction;

		// Top screen collision
		if (ball.y < 0)
		{
			ball.y = 0;
			ball.SetDirection(2 * PI - ball.velocity.direction);
		}
		// Bottom screen collision
		if (ball.y > GetScreenHeight()) {
 			ball.y = (float) GetScreenHeight() - ball.radius;
			ball.SetDirection(2 * PI - ball.velocity.direction); 
		}

		/** 
		  * If the ball goes off the screen on the left size, reset the ball, 
		  * and the left and right paddle.
		  **/
		if (ball.x < 0 || ball.x > GetScreenWidth()) {
			// This resets to all the initialized vairables
			ball = ballInit;
			leftPaddle = leftPaddleInit;
			rightPaddle = rightPaddleInit;
		}

		// Move left paddle up
		if (IsKeyDown(KEY_W))
			leftPaddle.SetYSpeed(-leftPaddle.aimYSpeed);
		// Move left paddle down
		else if (IsKeyDown(KEY_S))
			leftPaddle.SetYSpeed(leftPaddle.aimYSpeed);
		else leftPaddle.SetYSpeed(0);

		// Swing the paddle
		if (IsKeyPressed(KEY_D)) { /** Code to be added to swing the paddle **/ }
		// Hold paddle back
		if (IsKeyDown(KEY_A)) { /** Code to be added to wind back the paddle **/ }

		// Move right paddle up
		if (IsKeyDown(KEY_UP))
			rightPaddle.SetYSpeed(-rightPaddle.aimYSpeed);
		// Move right paddle down
		else if (IsKeyDown(KEY_DOWN))
			rightPaddle.SetYSpeed(rightPaddle.aimYSpeed);
		else rightPaddle.SetYSpeed(0);
		
		// Swing right paddle
		if (IsKeyPressed(KEY_LEFT)) { /** Code to be added to swing the paddle **/ }
		// Hold paddle back
		if (IsKeyDown(KEY_RIGHT)) { /** Code to be added to wind back the paddle **/ }

		// Left paddle and ball collision 
		if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, 
			leftPaddle.GetRect()) && ball.velocity.direction > PI / 2 && ball.velocity.direction < 3 * PI / 2) {
			ball.SetDirection(PI - ball.velocity.direction); // Change direction for bouncing

			/** Code to be added to change the velocity of the ball on collision of the paddle **/

			// Loop through all of the particles that launch on impact
			for (int i = 0; i < particlesOnHit; i++) { 
				// Find the first available particle, first particle in the particles arrray that is not in use
				for (int j = 0; j < particleCount; j++) { 
					// Check if the launch state of the particle has been finished
					if (particles[j].launchState == 0) {
						firstAvailableParticle = j;
						continue; // Now that an available particle is found, continue out of the loop
					} 
				}
				particles[firstAvailableParticle].x = ball.x; 
				particles[firstAvailableParticle].y = ball.y; 
				// This will range through PI radians
				particles[firstAvailableParticle].direction = (PI * i / particlesOnHit) - PI / 2;
				particles[firstAvailableParticle].launchState = abs(ball.velocity.speed) * 0.25 + swingForce;
			}
		}

		// Right paddle and ball collision
		if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius,
			rightPaddle.GetRect()) && (ball.velocity.direction <= PI / 2 || ball.velocity.direction >= 3 * PI / 2)) {
			ball.SetDirection(PI - ball.velocity.direction); // Change direction for bouncing

			/** Code to be added to change the velocity of the ball on collision of the paddle **/

			// Loop through all of the particles that launch on impact
			for (int i = 0; i < particlesOnHit; i++) {
				for (int j = 0; j < particleCount; j++) {
					// Check if the launch stat of the particle has been finished
					if (particles[j].launchState == 0) {
						firstAvailableParticle = j;
						continue; // Now that an available particle is found, continue out of the loop
					}
				}
				particles[firstAvailableParticle].x = ball.x;
				particles[firstAvailableParticle].y = ball.y;
				// This will range through PI radians
				particles[firstAvailableParticle].direction = (-PI * i / particlesOnHit) - PI / 2; 
				particles[firstAvailableParticle].launchState = abs(ball.velocity.speed) * 0.25 - swingForce; 
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
			lPVT.SetTriangle(500, 500, leftPaddle.velocity.speed / 10, leftPaddle.velocity.direction);
			lPVT.Draw();
			rightPaddle.Draw(); // Render the right paddle
			rightPaddle.BringBack(); // Oscilate the right paddle

			// Displays theySpeedof the ball
			DrawText(TextFormat("Ball Speed: %f", ball.velocity.speed), 100, 10, 20, BLUE);
			// Displays theySpeedof the last paddle to hit the ball on contact
			DrawText(TextFormat("Swing Force: %f", swingForce), 400, 10, 20, BLUE); 
			DrawText(TextFormat("Ball Direction: PI%f", ball.velocity.direction / PI), 10, 400, 20, BLUE);
			DrawFPS(10, 10); // Displays the fps
		EndDrawing(); // Finish frame
	}

	CloseWindow(); // When the game loop is done, close the window

	return 0;
}