#pragma once
#include <SDL.h>
#include "Waypoint.h"

class Enemy
{
public:
	bool isAlive = false;
	int x = 0, y = -100;
	int wayPoint = 0;
	SDL_Texture* spriteTexture = nullptr;
	int lives = 1;
	float speed = 100;
	float force = 50;
	float xVel = 0, yVel = 0;
	float friction = 0.75f;


	// --------------------------------------
	void resetStat();
	void updatePosition(float frameTimeSent, Waypoint wp);
	void damageEnemy();
	void deathReset();

};

class HeavyEnemy : public Enemy
{
public:
	int lives = 2;
	float speed = 100;
	float force = 25;

	// ------------------------------------
	void resetStat();
};

class Boss : public Enemy
{
public:
	int lives = 50;
	float force = 10;
	float speed = 50;
	bool invincible = false;

	// -----------------------------------
	void resetStat();
	void damageEnemy();

};