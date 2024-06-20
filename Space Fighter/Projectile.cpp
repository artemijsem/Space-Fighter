#include "Projectile.h"

// Constants
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800
#define SPRITE_SIZE 32
#define FPS 50

void Projectile::updatePosition(float frameTimeSent)
{
	if (xVel > 0) x += floor(xVel * frameTimeSent);
	if (xVel < 0) x += ceil(xVel * frameTimeSent);
	if (yVel > 0) y += floor(yVel * frameTimeSent);
	if (yVel < 0) y += ceil(yVel * frameTimeSent);
}

void Projectile::checkIsOnScreen()
{
	if ((x > SCREEN_WIDTH) || x < 0 || (y > SCREEN_HEIGHT) || y < SPRITE_SIZE / 2)
		isActive = false;
}

void Projectile::fire(float xSent, float ySent, int directionSent)
{
	if (!isActive)
	{
		isActive = true;
		x = xSent + (SPRITE_SIZE / 2);
		y = ySent - (directionSent * (SPRITE_SIZE / 2));
		yVel = -directionSent * speed;
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