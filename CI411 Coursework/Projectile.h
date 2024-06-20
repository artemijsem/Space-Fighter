#pragma once
#include <SDL.h>

class Projectile
{
public:
	bool isActive = false;
	int x = 0, y = 0;
	float speed = 500;
	int size = 8;
	float xVel = 0, yVel = 0;
	SDL_Texture* spriteTexture = nullptr;

	// ----------------------------------
	void resetStat();
	void updatePosition(float frameTimeSent);
	void checkIsOnScreen();
	void fire(float xSent, float ySent, int directionSent);
};
