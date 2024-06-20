#pragma once
#include <SDL.h>

class PC
{
public:
	bool isAlive = true;
	int x = 0, y = 0;
	SDL_Texture* spriteTexture = nullptr;
	int lives = 3;
	float speed = 200;
	float force = 50;
	float xVel = 0, yVel = 0;
	float friction = 0.92f;

	// -----------------------------------
	void resetStat();
	void moveXY(char directionSent);
	void updatePosition(float frameTimeSent);
	void screenLimit();
	void applyDrag();
	void checkIsAlive();

};