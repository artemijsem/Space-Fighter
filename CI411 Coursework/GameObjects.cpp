// SDL Game Object Classes
// ==========================================================

#include "GameObjects.h"
#include <random>
#include <string>

// Constants
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800
#define SPRITE_SIZE 32
#define FPS 50

int enemyXStartPos = SCREEN_WIDTH / 3;
int enemyYStartPos = -(SPRITE_SIZE * 8);
// ------------------------------------------------------------------

void PC::moveXY(char directionSent)
{
	// Apply velocity
	switch (directionSent)
	{
	case 'u': if (abs(yVel) < speed) yVel -= force; break;
	case 'd': if (abs(yVel) < speed) yVel += force; break;
	case 'l': if (abs(xVel) < speed) xVel -= force; break;
	case 'r': if (abs(xVel) < speed) xVel += force; break;
	}
}

void PC::updatePosition(float frameTimeSent)
{
	// Add the current velocity to current position
	// round up /down to nearest pixel depending on direction
	if (xVel > 0) x += floor(xVel * frameTimeSent);
	if (xVel < 0) x += ceil(xVel * frameTimeSent);
	if (yVel > 0) y += floor(yVel * frameTimeSent);
	if (yVel < 0) y += ceil(yVel * frameTimeSent);
}

void PC::screenLimit()
{	// Limit sprite to screen 
	if (x > SCREEN_WIDTH - SPRITE_SIZE)
	{
		xVel = 0;
		x = SCREEN_WIDTH - SPRITE_SIZE;
	}
	if (x < 0)
	{
		xVel = 0;
		x = 0;
	}
	if (y > SCREEN_HEIGHT - SPRITE_SIZE)
	{
		y = SCREEN_HEIGHT - SPRITE_SIZE;
		yVel = 0;
	}
	if (y < 0)
	{
		y = 0;
		yVel = 0;
	}

}

void PC::applyDrag()
{
	// apply linear drag - set vel to zero once clearly less than 1 pixel speed 
	if (abs(xVel) > 0.1f) xVel *= friction; else xVel = 0;
	if (abs(yVel) > 0.1f) yVel *= friction; else yVel = 0;
}

void PC::checkIsAlive()
{
	if (lives < 1)
	{
		isAlive = false;
	}
}

void PC::resetStat()
{
	isAlive = true;
	x = 0, y = 0;
	spriteTexture = nullptr;
	lives = 3;
	speed = 200;
	force = 50;
	xVel = 0, yVel = 0;
	friction = 0.92f;
}
// ---------------------------------------------------------
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
	x = SCREEN_WIDTH/2, y = -(SPRITE_SIZE*4);
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
// ------------------------------------------------------------------
void Projectile::updatePosition(float frameTimeSent)
{
	if (xVel > 0) x += floor(xVel * frameTimeSent);
	if (xVel < 0) x += ceil(xVel * frameTimeSent);
	if (yVel > 0) y += floor(yVel * frameTimeSent);
	if (yVel < 0) y += ceil(yVel * frameTimeSent);
}

void Projectile::checkIsOnScreen()
{
	if ((x > SCREEN_WIDTH) || x < 0 || (y > SCREEN_HEIGHT) || y < SPRITE_SIZE/2)
		isActive = false;
}

void Projectile::fire(float xSent, float ySent, int directionSent )
{
	if (!isActive)
	{
		isActive = true;
		x = xSent + (SPRITE_SIZE / 2);
		y = ySent - (directionSent * (SPRITE_SIZE / 2));
		yVel = - directionSent * speed;
	}
}

void Projectile::resetStat()
{
	isActive = false;
	x = 0, y = 0;
	speed = 500;
	size = 8;
	xVel = 0, yVel = 0;
	spriteTexture = nullptr;
}

// ==================================================================
void Item::respawnItem()
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> randomX(SPRITE_SIZE, SCREEN_WIDTH-SPRITE_SIZE);
	
	x = randomX(rng);
	y = -(SPRITE_SIZE * 2);
	yVel = speed;
}

void Item::updatePosition(float frameTimeSent)
{
	if (xVel > 0) x += floor(xVel * frameTimeSent);
	if (xVel < 0) x += ceil(xVel * frameTimeSent);
	if (yVel > 0) y += floor(yVel * frameTimeSent);
	if (yVel < 0) y += ceil(yVel * frameTimeSent);
}

void Item::checkIsOnScreen()
{
	if ((x > SCREEN_WIDTH) || x < 0 || (y > SCREEN_HEIGHT))
		isActive = false;
}

void Item::addLife(PC sentPC)
{
	sentPC.lives++;
}

void Item::resetStat()
{
	isActive = false;
	x = 0, y = 0;
	speed = 100;
	xVel = 0, yVel = 0;
	spriteTexture = nullptr;
	typeOfItem = 0;
}