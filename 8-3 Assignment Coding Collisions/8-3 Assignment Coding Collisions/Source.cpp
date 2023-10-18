/*
* 8-3 Assignment: Coding Collisions
* @author: Dominic Drury dominic.drury@snhu.edu
* Code base provided by CS 330 Module Eight Assignment Guidelines and Rubric
* linmath file provided by module 1 OpenGLSample
* Assistance on key press timing provided by user BDL on stackOverflow (citation at ProcessInput function)
*/

#include <GLFW\glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>

using namespace std;

const float DEG2RAD = 3.14159 / 180;
const int NUMBRICKS = 10; // Variable for number of bricks (10 fills screen)
float paddleXCord = 0; // Variable for paddle starting x cordinate

void processInput(GLFWwindow* window, int key, int scancode, int action, int mods);

enum BRICKTYPE { REFLECTIVE, DESTRUCTABLE };
enum ONOFF { ON, OFF };

class Brick
{
public:
	float red, green, blue;
	float x, y, width;
	int hp;
	BRICKTYPE brick_type;
	ONOFF onoff;

	Brick(BRICKTYPE bt, float xx, float yy, float ww, float rr, float gg, float bb)
	{
		brick_type = bt; x = xx; y = yy, width = ww; red = rr, green = gg, blue = bb;
		onoff = ON;
		hp = 2; // Health for ball
	};

	void drawBrick()
	{
		if (onoff == ON)
		{
			double halfside = width / 2;

			glColor3d(red, green, blue);
			glBegin(GL_POLYGON);

			glVertex2d(x + halfside, y + halfside);
			glVertex2d(x + halfside, y - halfside);
			glVertex2d(x - halfside, y - halfside);
			glVertex2d(x - halfside, y + halfside);

			glEnd();
		}
	}
};


class Circle
{
public:
	float red, green, blue;
	float radius;
	float x;
	float y;
	float speed = 0.01;
	int direction; // 1=up 2=right 3=down 4=left 5 = up right   6 = up left  7 = down right  8= down left

	Circle(double xx, double yy, double rr, int dir, float rad, float r, float g, float b)
	{
		x = xx;
		y = yy;
		radius = rr;
		red = r;
		green = g;
		blue = b;
		radius = rad;
		direction = dir;
	}

	/*
	* Function for getting direction. Takes no input, provides no output, changed direction variable of
	* ball on impact with brick, paddle, or side of window
	*/
	void GetDirection() {
		if (direction == 1) { // if ball going up then ball goes down
			direction = 3;
		}
		else if (direction == 2) { // if ball going right then ball goes left
			direction = 4;
		}
		else if (direction == 3) { // if ball going down then ball goes up
			direction = 1;
		}
		else if (direction == 4) { // if ball going left then ball goes right
			direction = 2;
		}
		else if (direction == 5) { // if ball going up right then ball goes up left
			direction = 6;
		}
		else if (direction == 6) { // if ball going up left then ball goes up right
			direction = 5;
		}
		else if (direction == 7) { // if ball going down right then ball goes down left
			direction = 8;
		}
		else if (direction == 8) { // if ball going down left then ball goes down right
			direction = 7;
		}
		else {
			direction = GetRandomDirection(); // else ball goes in random direction
		}
	}

	void CheckCollisionBrick(Brick* brk)
	{
		if (brk->brick_type == REFLECTIVE)
		{
			if ((x > brk->x - brk->width && x <= brk->x + brk->width) && (y > brk->y - brk->width && y <= brk->y + brk->width))
			{
				GetDirection();
				x = x + 0.03;
				y = y + 0.04;
			}
		}
		else if (brk->brick_type == DESTRUCTABLE)
		{
			if ((x > brk->x - brk->width && x <= brk->x + brk->width) && (y > brk->y - brk->width && y <= brk->y + brk->width))
			{
				if (brk->onoff == ON) { // Check to make sure brick is not destroyed
					speed -= 0.05; // Decrease ball speed if it damages a brick
					brk->hp -= 1; // Brick looses 1 health

					//Brick changes color from green to red to gone
					brk->green = 0; 
					brk->red = 1;
				}
				if (brk->hp <= 0) { // If hp reaches 0 then brick is turned off and disappears
					brk->onoff = OFF;
				}
			}
		}
	}

	int GetRandomDirection()
	{
		return (rand() % 8) + 1;
	}

	void MoveOneStep()
	{
		if (direction == 1 || direction == 5 || direction == 6)  // up
		{
			if (y > -1 + radius)
			{
				y -= speed;
			}
			else
			{
				direction = GetRandomDirection();
			}
		}

		if (direction == 2 || direction == 5 || direction == 7)  // right
		{
			if (x < 1 - radius)
			{
				x += speed;
			}
			else
			{
				direction = GetRandomDirection();
			}
		}

		if (direction == 3 || direction == 7 || direction == 8)  // down
		{
			if (y < 1 - radius) {
				y += speed;
			}
			else
			{
				direction = GetRandomDirection();
			}
		}

		if (direction == 4 || direction == 6 || direction == 8)  // left
		{
			if (x > -1 + radius) {
				x -= speed;
			}
			else
			{
				direction = GetRandomDirection();
			}
		}
	}

	void DrawCircle()
	{
		glColor3f(red, green, blue);
		glBegin(GL_POLYGON);
		for (int i = 0; i < 360; i++) {
			float degInRad = i * DEG2RAD;
			glVertex2f((cos(degInRad) * radius) + x, (sin(degInRad) * radius) + y);
		}
		glEnd();
	}
};


vector<Circle> world; // Vector for storing circle balls
vector<Brick> bricks; // Vector for storing bricks


int main(void) {
	srand(time(NULL));

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow* window = glfwCreateWindow(480, 480, "8-3 Assignment: Coding Collisions", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	/*
	* for loop that creates NUMBRICKS number of bricks along the upper part of the screen with 
	* a random chance of being destructable or reflective. White for reflective and Green for healthy,
	* destroyable, brick
	*/
	for (int i = 0; i < NUMBRICKS; i++) {
		double xCord = (i * 0.2) - 0.9;
		double yCord = 0.5;
		int isDestructable = rand() % 2; // Sets variable to random number (0-1)

		if (isDestructable < 1) { //if variable is 1 then brick is destructable
			Brick brick(DESTRUCTABLE, xCord, yCord, 0.18, 0, 1, 0);
			bricks.push_back(brick);
		}
		else { // Else brick is reflective
			Brick brick(REFLECTIVE, xCord, yCord, 0.18, 1, 1, 1);
			bricks.push_back(brick);
		}
		
	}

	while (!glfwWindowShouldClose(window)) {
		//Setup View
		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSetKeyCallback(window, processInput);

		Brick paddle(REFLECTIVE, paddleXCord, -1.1, 0.4, 1, 1, 1); // Brick for paddle

		paddle.drawBrick(); // Drawing of paddle

		// Drawing all bricks (excluding off bricks)
		for (int i = 0; i < bricks.size(); i++) {
			bricks[i].drawBrick();
		}

		//Movement of balls
		for (int i = 0; i < world.size(); i++)
		{
			// Checks collision of bricks
			for (int j = 0; j < bricks.size(); j++) {
				world[i].CheckCollisionBrick(&bricks[j]);
			}

			// Checks collision of circles
			if (world.size() > 1) {
				for (int j = 0; j < world.size(); j++) {
					Circle* circleA = &world[i];
					Circle* circleB = &world[j];

					if (circleA != circleB) {
						if (((world[i].x >= world[j].x - world[j].radius && world[i].x <= world[j].x + world[j].radius)&& 
							 (world[i].y >= world[j].y - world[j].radius && world[i].y <= world[j].y + world[j].radius))) {
							if (world[i].radius <= 0.1 && world[j].radius <= 0.1) { // If two small balls meet they form a big ball
								world[i].radius += 0.05;
								world[j].y = -2;
								world[j].direction = 1;
							}
							else if (world[i].radius >= 0.1 && world[j].radius >= 0.1) { // If two big balls meet both disappear
								world[i].y = -2;
								world[i].direction = 1;
								world[j].y = -2;
								world[j].direction = 1;
							}
							else if (world[i].radius >= 0.1 && world[j].radius <= 0.1) { // If a big and a small meet, small disappears
								world[j].y = -2;
								world[j].direction = 1;
							}
						}
					}
				}
			}
			world[i].CheckCollisionBrick(&paddle); // Checks collision with paddle
			world[i].MoveOneStep();
			world[i].DrawCircle();

		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate;
	exit(EXIT_SUCCESS);
}

/*
* Code for single key press resulting in single ball taken from user BDL answering a question in 
* stackoverflow
* https://stackoverflow.com/questions/51873906/is-there-a-way-to-process-only-one-input-event-after-a-key-is-pressed-using-glfw
*/
void processInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) { // if space is pressed then summon 1 ball moving up the screen
		double r, g, b;
		// Sets ball to random color
		r = rand() / 10000 + 0.1; // The + 0.1 prevents black balls that dont show against background
		g = rand() / 10000;
		b = rand() / 10000;
		Circle B(paddleXCord, -0.65, 0.02, 3, 0.05, r, g, b);
		world.push_back(B);
	}

	if (key == GLFW_KEY_A && action == GLFW_PRESS && paddleXCord > -0.8) { // A moves paddle to the left
		paddleXCord -= 0.1;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS && paddleXCord < 0.8) { // D moves paddle to the right
		paddleXCord += 0.1;
	}
}