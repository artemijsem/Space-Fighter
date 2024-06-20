#pragma once
#include <SDL.h>
#include "PC.h"

class Item
{
public:
	bool isActive = false;
	int x = 0, y = 0;
	float speed = 100;
	float xVel = 0, yVel = 0;
	SDL_Texture* spriteTexture = nullptr;
	int typeOfItem = 0;
	// -----------------------------------
	void respawnItem();
	void updatePosition(float frameTimeSent);
	void checkIsOnScreen();
	void addLife(PC);
	void resetStat();

};