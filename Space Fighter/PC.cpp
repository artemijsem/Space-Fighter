#include "PC.h"
#include <cmath>

// Constants
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800
#define SPRITE_SIZE 32
#define FPS 50

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
	if (abs(xVel) > 0.1f) xVel *= friction; else xVel = 0.0f;
	if (abs(yVel) > 0.1f) yVel *= friction; else yVel = 0.0f;
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