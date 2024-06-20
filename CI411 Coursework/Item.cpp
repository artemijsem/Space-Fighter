#include "Item.h"
#include <random>

// Constants
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800
#define SPRITE_SIZE 32
#define FPS 50

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