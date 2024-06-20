#include "Enemy.h"
#include "Waypoint.h"
#include <cmath>

// Constants
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800
#define SPRITE_SIZE 32
#define FPS 50

int enemyXStartPos = SCREEN_WIDTH / 3;
int enemyYStartPos = -(SPRITE_SIZE * 8);

void Enemy::updatePosition(float frameTimeSent, Waypoint wp)
{
	if (this->isAlive)
	{

		// Apply velocity
		if (x < wp.x && abs(xVel) < speed) xVel += force;
		if (x > wp.x && abs(xVel) < speed) xVel -= force;
		if (y < wp.y && abs(yVel) < speed) yVel += force;
		if (y > wp.y && abs(yVel) < speed) yVel -= force;


		// Add the current velocity to current position
		// round up /down to nearest pixel depending on direction
		if (xVel > 0) x += floor(xVel * frameTimeSent);
		if (xVel < 0) x += ceil(xVel * frameTimeSent);
		if (yVel > 0) y += floor(yVel * frameTimeSent);
		if (yVel < 0) y += ceil(yVel * frameTimeSent);

		// apply linear drag - set vel to zero once clearly less than 1 pixel speed 
		if (abs(xVel) > 0.1f) xVel *= friction; else xVel = 0;
		if (abs(yVel) > 0.1f) yVel *= friction; else yVel = 0;
	}

}

void Enemy::deathReset()
{

	x = enemyXStartPos;
	y = enemyYStartPos;
	xVel = 0;
	yVel = 0;
	wayPoint = 0;

}

void Enemy::resetStat()
{
	isAlive = false;
	x = 0, y = 0;
	wayPoint = 0;
	spriteTexture = nullptr;
	speed = 100;
	force = 50;
	xVel = 0, yVel = 0;
	friction = 0.75f;
}
void Enemy::damageEnemy()
{
	if (lives > 1) lives--;
	else
	{
		isAlive = false;
		deathReset();
	};
}

// ------------------------------------------------------------------

void HeavyEnemy::resetStat()
{
	isAlive = false;
	x = 0, y = 0;
	wayPoint = 0;
	spriteTexture = nullptr;
	speed = 100;
	force = 25;
	xVel = 0, yVel = 0;
	friction = 0.75f;
	lives = 3;
}
// ------------------------------------------------------------------
void Boss::resetStat()
{
	isAlive = false;
	invincible = false;
	x = SCREEN_WIDTH / 2, y = -(SPRITE_SIZE * 4);
	wayPoint = 0;
	spriteTexture = nullptr;
	speed = 50;
	force = 10;
	xVel = 0, yVel = 0;
	friction = 0.75f;
	lives = 50;
}

void Boss::damageEnemy()
{
	if (lives > 1) lives--;
	else
	{
		isAlive = false;

	};
}