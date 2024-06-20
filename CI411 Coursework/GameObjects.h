#pragma once
#include <stdio.h>
#include<iostream>
#include <SDL.h>
#include <string>

// Constants
#define SCREEN_WIDTH  600
#define SCREEN_HEIGHT 800
#define SPRITE_SIZE 32
#define FPS 50

// =======================================================
// classes

class Waypoint
{
public:
	int x = 0, y = 0;
	SDL_Texture* spriteTexture = nullptr;

	// ----------------------------------
};

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

class HeavyEnemy: public Enemy
{
public:
	int lives = 2;
	float speed = 100;
	float force = 25;

	// ------------------------------------
	void resetStat();
};

class Boss: public Enemy
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
	void checkIsOnScreen();;
	void addLife(PC);
	void resetStat();
	
};