// =======================================================
// CI411 SDL Game
// Artemi Sementsenko UoBGames, 2022
// =======================================================
// Libraries to include
#include <SDL.h>
#include <random>
#include <stdio.h> 
#include <iostream>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <math.h>
#include <string>
#include "GameObjects.h"
#include "WayPointMap.h"
using namespace std;

// Function Prototypes
void startSDL();
void startScreen();
void createWayPoints(Waypoint *sentWPArr, int sentPattern[][colNum]);
void gameLoop();
void closeSDL();
void createGameObjects();
void playerInputHandler();
void updateGameObjects();
void updateScreen();
void limitFrameRate(Uint32);
void checkCollision();
int nextBullet();
void updateGUI();
void deathScreen();
void gameStatReset();
void startBossFight();
void winScreen();
void RenderHPBar(int x, int y, int w, int h, float Percent, SDL_Color FGColor, SDL_Color BGColor);
SDL_Color color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);


// Constants
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800
#define SPRITE_SIZE 32
#define FPS 50


// Random generator
std::random_device dev;
std::mt19937 rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> randomInt(0,1);
std::uniform_int_distribution<std::mt19937::result_type> randomItemSpawnTimeGet(10000, 20000);
std::uniform_int_distribution<std::mt19937::result_type> randomChance(0, 100);

// Global Variables
float frameTime;
float rndItemSpawnTime = randomItemSpawnTimeGet(rng);
int bulletIndex = 0;
int score = 0;
Uint32 frameStartTime;
Uint32 heavyEnemyLocalTick = 0;
Uint32 lightEnemyLocalTick = 0;
Uint32 itemSpawnLocalTick = 0;
Uint32 bossEnemyLocalTick = 0;
SDL_Window* gameWindow = NULL;
SDL_Surface* mainGameSurface = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* gameBackground = NULL;
SDL_Event playerInputEvent;
// Game Objects
PC pc;
Projectile bullet[9];
Projectile lightEnemyBullet[10];
Projectile heavyEnemyBullet[5];
Projectile bossEnemyBullet;
Enemy lightEnemies[10];
HeavyEnemy heavyEnemies[5];
Boss bossEnemy;
Item lifeUP, bigBomb;

// Waypoints
int lightEnemyWPNum = numOfWayPoints(pattern1);
Waypoint *lightEnemyWayPoints = new Waypoint[lightEnemyWPNum];

int heavyEnemyWPNum = numOfWayPoints(pattern2);
Waypoint* heavyEnemyWayPoints = new Waypoint[heavyEnemyWPNum];

int bossEnemyWPNum = numOfWayPoints(pattern3);
Waypoint* bossEnemyWayPoints = new Waypoint[bossEnemyWPNum];

// Game Play
bool gamePlaying = true;
bool gameOver = false;
bool doneCreatingEnemies = false;
bool bossFight = false;


// Text variables 
TTF_Font* font = nullptr;
TTF_Font* bigFont = nullptr;
SDL_Color whiteTextColor = { 255, 255, 255 };
SDL_Color greenTextColor = { 0, 255, 0 };
SDL_Color redTextColor = { 255, 0, 0 };
SDL_Surface* textSurface = nullptr;
SDL_Texture* textTexture = nullptr;
SDL_Rect textRect;
string screenText;

// Audio variables
Mix_Music* music = NULL;
Mix_Chunk* shootSound = NULL;
Mix_Chunk* enemyDeath = NULL;
Mix_Chunk* enemyShoot = NULL;
Mix_Chunk* playerHit = NULL;


// =======================================================
int main(int argc, char* args[])
{
	startSDL();
	startScreen();
	gameLoop();
	closeSDL();
	return 0;
}
// =======================================================

void startSDL()
{
	// Intialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) printf("SDL Init Error: %s\n", SDL_GetError());
	else
	{
		// Create Window
		gameWindow = SDL_CreateWindow("Space Fighter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		if (gameWindow == NULL) printf("SDL Window Error %s\n", SDL_GetError());

		// Get Game Window Surface
		mainGameSurface = SDL_GetWindowSurface(gameWindow);

		// Create a renderer for the window
		renderer = SDL_CreateRenderer(gameWindow, -1, 0);

		// Set the draw color of the renderer
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);

		// Inititialize Fonts
		TTF_Init();
		font = TTF_OpenFont("fonts/upheavtt.ttf", 20);
		bigFont = TTF_OpenFont("fonts/upheavtt.ttf", 60);

		// Audio
		Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	}
}
// =======================================================
void startScreen()
{
	// Load Title Imgae
	const char* startImageFile = "images/Title_AS.png";
	SDL_Surface* startImage = IMG_Load(startImageFile);

	// Merge image with surface
	SDL_BlitSurface(startImage, NULL, mainGameSurface, NULL);

	SDL_UpdateWindowSurface(gameWindow);
	SDL_Delay(2000);
	SDL_FreeSurface(startImage);
}
// ========================================================
void gameLoop()
{
	while (!gameOver)
	{
		createWayPoints(lightEnemyWayPoints, pattern1);
		createWayPoints(heavyEnemyWayPoints, pattern2);
		createWayPoints(bossEnemyWayPoints, pattern3);
		createGameObjects();

		Mix_PlayMusic(music, -1);

		// Main Game Loop
		while (gamePlaying)
		{
			// Time since start of the game in ms
			frameStartTime = SDL_GetTicks();
			playerInputHandler();
			updateGameObjects();
			updateScreen();
			limitFrameRate(frameStartTime);
			if (pc.isAlive == false)
			{
				gamePlaying = false;
			}

			if (!bossEnemy.isAlive && bossFight)
			{
				gamePlaying = false;
				winScreen();
			}
			if (score > 10000)
			{
				bossEnemy.isAlive = true;
				bossFight = true;
				startBossFight();
			}
		}
		while (!gamePlaying)
		{
			if (!pc.isAlive)
			{
				deathScreen();
			}
			if (bossFight && pc.isAlive)
			{
				winScreen();
			}
			
		}
	}
}
// ========================================================
void updateScreen()
{
	SDL_Rect spriteRect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	// Clear & Update Screen
	SDL_RenderClear(renderer);
	// Set Background Image
	SDL_RenderCopy(renderer, gameBackground, NULL, &spriteRect);

	// Light Enemy
	for (int i = 0; i < sizeof(lightEnemies) / sizeof(lightEnemies[0]); i++)
	{
		if (lightEnemies[i].isAlive) // exlcude the dead 
		{
			spriteRect = { lightEnemies[i].x,  lightEnemies[i].y, SPRITE_SIZE, SPRITE_SIZE };
			SDL_RenderCopyEx(renderer, lightEnemies[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);

		}

	}

	// Heavy Enemy
	for (int i = 0; i < sizeof(heavyEnemies) / sizeof(heavyEnemies[0]); i++)
	{
		if (heavyEnemies[i].isAlive)
		{
			spriteRect = { heavyEnemies[i].x, heavyEnemies[i].y, SPRITE_SIZE*2, SPRITE_SIZE*2};
			SDL_RenderCopyEx(renderer, heavyEnemies[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}

	// Light Enemy Bullet
	for (int i = 0; i < sizeof(lightEnemyBullet) / sizeof(lightEnemyBullet[0]); i++)
	{
		if (lightEnemyBullet[i].isActive)
		{
			spriteRect = { lightEnemyBullet[i].x, lightEnemyBullet[i].y, lightEnemyBullet[i].size,lightEnemyBullet[i].size };
			SDL_RenderCopyEx(renderer, lightEnemyBullet[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}

	// Heavy Enemy Bullet
	for (int i = 0; i < sizeof(heavyEnemyBullet) / sizeof(heavyEnemyBullet[0]); i++)
	{
		if (heavyEnemyBullet[i].isActive)
		{
			spriteRect = { heavyEnemyBullet[i].x, heavyEnemyBullet[i].y, heavyEnemyBullet[i].size,heavyEnemyBullet[i].size };
			SDL_RenderCopyEx(renderer, heavyEnemyBullet[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}


	// PC Bullet 
	for (int i = 0; i < sizeof(bullet) / sizeof(bullet[0]); i++)
	{
		if (bullet[i].isActive)
		{
			spriteRect = { bullet[i].x,bullet[i].y,bullet[i].size, bullet[i].size };
			SDL_RenderCopyEx(renderer, bullet[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}

	// Light Enemy Waypoint 
	for (int i = 0; i < lightEnemyWPNum; i++)
	{

			spriteRect = { lightEnemyWayPoints[i].x,lightEnemyWayPoints[i].y,SPRITE_SIZE,SPRITE_SIZE };
			SDL_RenderCopyEx(renderer, lightEnemyWayPoints[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		
	}

	// Heavy Enemy Waypoint 
	for (int i = 0; i < heavyEnemyWPNum; i++)
	{

		spriteRect = { heavyEnemyWayPoints[i].x,heavyEnemyWayPoints[i].y,SPRITE_SIZE,SPRITE_SIZE };
		SDL_RenderCopyEx(renderer, heavyEnemyWayPoints[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);

	}

	// Life Up Item
	if (lifeUP.isActive)
	{
		spriteRect = { lifeUP.x, lifeUP.y, SPRITE_SIZE, SPRITE_SIZE };
		SDL_RenderCopyEx(renderer, lifeUP.spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
	}

	// Big Bomb Item
	if (bigBomb.isActive)
	{
		spriteRect = { bigBomb.x, bigBomb.y, SPRITE_SIZE, SPRITE_SIZE };
		SDL_RenderCopyEx(renderer, bigBomb.spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
	}

	// Boss Sprite
	if (bossEnemy.isAlive && bossFight)
	{
 		spriteRect = { bossEnemy.x,bossEnemy.y,78, 120 };
		/*spriteRect = { bossEnemy.x,bossEnemy.y,SPRITE_SIZE, SPRITE_SIZE };*/
		SDL_RenderCopyEx(renderer, bossEnemy.spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
	}

	// Boss Bullet Sprite
	if (bossEnemyBullet.isActive)
	{
		spriteRect = { bossEnemyBullet.x,bossEnemyBullet.y,20, 42 };
		/*spriteRect = { bossEnemyBullet.x,bossEnemyBullet.y,SPRITE_SIZE, SPRITE_SIZE };*/
		SDL_RenderCopyEx(renderer, bossEnemyBullet.spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
	}

	// PC Sprite
	spriteRect = { pc.x, pc.y, SPRITE_SIZE, SPRITE_SIZE };
	SDL_RenderCopyEx(renderer, pc.spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);


	updateGUI();


	// Update screen
	SDL_RenderPresent(renderer);
}
// ========================================================
void playerInputHandler()
{
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_UP]) pc.moveXY('u');
	if (state[SDL_SCANCODE_DOWN]) pc.moveXY('d');
	if (state[SDL_SCANCODE_LEFT]) pc.moveXY('l');
	if (state[SDL_SCANCODE_RIGHT]) pc.moveXY('r');
	if (state[SDL_SCANCODE_J]) pc.lives++;
	if (state[SDL_SCANCODE_K]) score+=100;
	if (state[SDL_SCANCODE_L]) bossEnemy.lives--;

	// Set up the input event handler
	while (SDL_PollEvent(&playerInputEvent) != 0)
	{
		switch (playerInputEvent.type)
		{
		case SDL_QUIT:
			gameOver = true;
			gamePlaying = false;
			closeSDL();
			break;
		case SDL_KEYDOWN:
			switch (playerInputEvent.key.keysym.sym)
			{
			case SDLK_SPACE:
				bullet[nextBullet()].fire(pc.x, pc.y, 1);
				Mix_PlayChannel(-1, shootSound, 0);
				break;
			}break;

		}

	}
}
// ========================================================
void createGameObjects() 
{

	// Background Image
	const char* spriteImageFile = "images/background_02.png";
	SDL_Surface* spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Background Image Error - SDL Error: %s\n", SDL_GetError());
	gameBackground = SDL_CreateTextureFromSurface(renderer, spriteImage);

	// PC Sprite
	spriteImageFile = "images/SpaceShip.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf("PC Image Error - SDL Error: %s\n", SDL_GetError());
	pc.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	pc.x = SCREEN_WIDTH / 2 - SPRITE_SIZE / 2;
	pc.y = SCREEN_HEIGHT - 200;

	// PC Bullet Sprite
	spriteImageFile = "images/circle_8.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf("Bullet Image Error - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(bullet) / sizeof(bullet[0]); i++)
	{
		bullet[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	// Light Enemy Sprite
	spriteImageFile = "images/enemy_01.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf("Light Enemy Image Error - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(lightEnemies) / sizeof(lightEnemies[0]); i++)
	{
		lightEnemies[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);

		// Make first enemy alive 
		if (i == 0)
		{
			lightEnemies[i].isAlive = true;
			lightEnemies[i].x = lightEnemyWayPoints[0].x;
			lightEnemies[i].y = -(SPRITE_SIZE * 2);
		}
		else{ lightEnemies[i].deathReset(); }

	}

	// Heavy Enemy Sprite
	spriteImageFile = "images/enemy_02.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf("Heavy Enemy Image Error - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(heavyEnemies) / sizeof(heavyEnemies[0]); i++)
	{
		heavyEnemies[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);

		heavyEnemies[i].deathReset();

	}

	// Light Enemy Bullet Sprite
	spriteImageFile = "images/red_circle_8.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf("Light Enemy Bullet Image Error - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(lightEnemyBullet) / sizeof(lightEnemyBullet[0]); i++)
	{
		lightEnemyBullet[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	// Heavy Enemy Bullet Sprite
	spriteImageFile = "images/purple_circle_8.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf("Heavy Enemy Bullet Image Error - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(heavyEnemyBullet) / sizeof(heavyEnemyBullet[0]); i++)
	{
		heavyEnemyBullet[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	// Boss Enemy Sprite
	spriteImageFile = "images/Enemy_Boss.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf("Boss Image Error - SDL Error: %s\n", SDL_GetError());
	bossEnemy.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	bossEnemy.deathReset();

	// Boss Enemy Bullet Sprite
	spriteImageFile = "images/boss_shot.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf("Boss Bullet Image Error - SDL Error: %s\n", SDL_GetError());
	bossEnemyBullet.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);


	// Life Up Item Sprite
	spriteImageFile = "images/Life_Up.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf("Life Up Item Image Error - SDL Error: %s\n", SDL_GetError());
	lifeUP.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);

	// Big Bomb Item Sprite
	spriteImageFile = "images/bigBomb.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf("Big Bomb Item Image Error - SDL Error: %s\n", SDL_GetError());
	bigBomb.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);



	SDL_FreeSurface(spriteImage);
	

	// Load audio
	music = Mix_LoadMUS("audio/Defrini_Spookie.mp3");
	shootSound = Mix_LoadWAV("audio/shoot.wav");
	enemyDeath = Mix_LoadWAV("audio/explosion.wav");
	enemyShoot = Mix_LoadWAV("audio/enemyShoot.wav");
	playerHit = Mix_LoadWAV("audio/playerHit.wav");
}
// =======================================================

void updateGUI()
{
	int textW = 0;
	int textH = 0;



	// -------------------------------------------------------

	// Lives
	screenText = " Lives: " + std::to_string(pc.lives);
	textSurface = TTF_RenderText_Solid(font, screenText.c_str(), whiteTextColor);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 100, 760, textW, textH };
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);

	// Score
	screenText = " Score: " + std::to_string(score);
	textSurface = TTF_RenderText_Solid(font, screenText.c_str(), whiteTextColor);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 400, 760, textW, textH };
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);

	// Boss HP
	if (bossEnemy.isAlive && bossFight)
	{
		RenderHPBar(500, 60, -400, 20, (float)bossEnemy.lives/50, color(255, 0, 0, 255), color(0, 0, 0, 255));

		screenText = "Boss HP:";
		textSurface = TTF_RenderText_Solid(font, screenText.c_str(), redTextColor);
		textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

		SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
		textRect = { 250, 30, textW, textH };
		SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

		SDL_FreeSurface(textSurface);
		SDL_DestroyTexture(textTexture);
	}


}


void checkCollision()
{
	SDL_Rect pcRect = { pc.x,pc.y, SPRITE_SIZE, SPRITE_SIZE };
	SDL_Rect bulletRect;
	SDL_Rect lightEnemyRect;
	SDL_Rect lightEnemyWPRect;
	SDL_Rect lightEnemyBulletRect;
	SDL_Rect heavyEnemyRect;
	SDL_Rect heavyEnemyWPRect;
	SDL_Rect heavyEnemyBulletRect;
	SDL_Rect bossEnemyRect = { bossEnemy.x,bossEnemy.y,78,120};
	SDL_Rect bossEnemyWPRect = { bossEnemy.x,bossEnemy.y,1,1};
	SDL_Rect bossEnemyBulletRect = { bossEnemyBullet.x,bossEnemyBullet.y,20,42 };
	SDL_Rect wayPointRect;
	SDL_Rect lifeUpRect = {lifeUP.x,lifeUP.y,SPRITE_SIZE,SPRITE_SIZE};
	SDL_Rect bigBombRect = { bigBomb.x, bigBomb.y, SPRITE_SIZE,SPRITE_SIZE };


	// Prevent heavy enemy collision with each other
	for (int i = 0; i < sizeof(heavyEnemies) / sizeof(heavyEnemies[0]); i++)
	{
		if (heavyEnemies[i].isAlive)
		{
			heavyEnemyRect = { heavyEnemies[i].x,heavyEnemies[i].y,SPRITE_SIZE * 2,SPRITE_SIZE * 2 };
			for (int j = 0; j < sizeof(heavyEnemies) / sizeof(heavyEnemies[0]); j++)
			{
				if (heavyEnemies[j].isAlive)
				{
					SDL_Rect secondHeavyEnemyRect = { heavyEnemies[j].x,heavyEnemies[j].y,SPRITE_SIZE * 2,SPRITE_SIZE * 2 };
					if (SDL_HasIntersection(&heavyEnemyRect, &secondHeavyEnemyRect))
					{
						heavyEnemies[j].isAlive = false;
					}

					heavyEnemies[j].isAlive = true;
				}
			}

		}
	}

	// Prevent light enemy collision with eachother
	for (int i = 0; i < sizeof(lightEnemies) / sizeof(lightEnemies[0]); i++)
	{
		if (lightEnemies[i].isAlive)
		{
			lightEnemyRect = { lightEnemies[i].x,lightEnemies[i].y,SPRITE_SIZE,SPRITE_SIZE};
			for (int j = 0; j < sizeof(lightEnemies) / sizeof(lightEnemies[0]); j++)
			{
				if (lightEnemies[j].isAlive)
				{
					SDL_Rect secondLightEnemyRect = { lightEnemies[j].x,lightEnemies[j].y,SPRITE_SIZE,SPRITE_SIZE};
					if (SDL_HasIntersection(&lightEnemyRect, &secondLightEnemyRect))
					{
						lightEnemies[j].isAlive = false;
					}

					lightEnemies[j].isAlive = true;
				}
			}

		}
	}


	// Check if light enemy collided with waypoint
	for (int i = 0; i < sizeof(lightEnemies) / sizeof(lightEnemies[0]); i++)
	{
		// Exclude dead enemies
		if (lightEnemies[i].isAlive)
		{
			lightEnemyWPRect.x = lightEnemies[i].x;
			lightEnemyWPRect.y = lightEnemies[i].y;
			lightEnemyWPRect.h = lightEnemyWPRect.w = 1;


			// Check every enemy way point
			for (int k = 0; k < lightEnemyWPNum; k++)
			{
				wayPointRect = { lightEnemyWayPoints[k].x, lightEnemyWayPoints[k].y, 1, 1 };
				if (SDL_HasIntersection(&wayPointRect, &lightEnemyWPRect))
				{
					// Light Enemy waypoint switch
					if (lightEnemies[i].isAlive && lightEnemies[i].wayPoint == k)
					{
						lightEnemies[i].wayPoint++;
						if (lightEnemies[i].wayPoint == lightEnemyWPNum)
						{
							lightEnemies[i].isAlive = false;
							lightEnemies[i].deathReset();
							Mix_PlayChannel(-1, enemyDeath, 0);

							// Deduct score if Player missed an enemy
							score -= 100;
	
						}
					}

				}
			}
		}
		
	}


	// Check if Boss collided with the waypoint
	for (int i = 0; i < bossEnemyWPNum; i++)
	{
		if(bossEnemy.isAlive)
		{
			wayPointRect = { bossEnemyWayPoints[i].x,bossEnemyWayPoints[i].y,1,1 };
			if (SDL_HasIntersection(&wayPointRect, &bossEnemyWPRect))
			{
				if (bossEnemy.isAlive && bossEnemy.wayPoint == i)
				{
					bossEnemy.wayPoint++;

					// If Boss passed Player without dying game is over
					if (bossEnemy.wayPoint == bossEnemyWPNum)
					{
						gamePlaying = false;
					}
				}
			}
		}
	}

	// PC Bullet collision
	for (int j = 0; j < sizeof(bullet) / sizeof(bullet[0]); j++)
	{
		if(bullet[j].isActive)
		{
			bulletRect.x = bullet[j].x;
			bulletRect.y = bullet[j].y;
			bulletRect.h = bulletRect.w = bullet[j].size;

			// Collision with Light Enemy
			for (int i = 0; i < sizeof(lightEnemies) / sizeof(lightEnemies[0]); i++)
			{
				if (lightEnemies[i].isAlive)
				{
					lightEnemyRect.x = lightEnemies[i].x;
					lightEnemyRect.y = lightEnemies[i].y;
					lightEnemyRect.h = lightEnemyRect.w = SPRITE_SIZE;
					if (SDL_HasIntersection(&bulletRect, &lightEnemyRect))
					{
						Mix_PlayChannel(-1, enemyDeath, 0);
						bullet[j].isActive = false;
						lightEnemies[i].isAlive = false;
						lightEnemies[i].deathReset();

						// Add score if Player killed an enemy
						score += 100;
					}
				}
			}

			// Collision with Heavy Enemy
			for (int i = 0; i < sizeof(heavyEnemies) / sizeof(heavyEnemies[0]); i++)
			{
				if (heavyEnemies[i].isAlive)
				{
					heavyEnemyRect = { heavyEnemies[i].x,heavyEnemies[i].y,SPRITE_SIZE*2,SPRITE_SIZE*2 };
					if (SDL_HasIntersection(&bulletRect, &heavyEnemyRect))
					{
						Mix_PlayChannel(-1, enemyDeath, 0);
						bullet[j].isActive = false;
						heavyEnemies[i].damageEnemy();

						// If PC kills heavy enemy 
						if (!heavyEnemies[i].isAlive)
						{
							score += 300;
						}

					}
				}
			}

			// Collision with Boss Enemy
			if (bossEnemy.isAlive)
			{
				if (SDL_HasIntersection(&bulletRect, &bossEnemyRect))
				{
					bullet[j].isActive = false;
					if (!bossEnemy.invincible)
					{
						bossEnemy.damageEnemy();
					}
				}
			}

		}

	}

	// Heavy Enemy Waypoint collison
	for (int i = 0; i < sizeof(heavyEnemies) / sizeof(heavyEnemies[0]); i++)
	{
		if (heavyEnemies[i].isAlive)
		{
			heavyEnemyWPRect = { heavyEnemies[i].x,heavyEnemies[i].y,1,1 };

			// Check every enemy way point
			for (int k = 0; k < heavyEnemyWPNum; k++)
			{
				wayPointRect = { heavyEnemyWayPoints[k].x, heavyEnemyWayPoints[k].y, 1, 1 };
				if (SDL_HasIntersection(&wayPointRect, &heavyEnemyWPRect))
				{
					// Light Enemy waypoint switch
					if (heavyEnemies[i].isAlive && heavyEnemies[i].wayPoint == k)
					{
						heavyEnemies[i].wayPoint++;
						if (heavyEnemies[i].wayPoint == heavyEnemyWPNum)
						{
							heavyEnemies[i].isAlive = false;
							heavyEnemies[i].deathReset();
							Mix_PlayChannel(-1, enemyDeath, 0);

							// Deduct score if Player missed an enemy
							score -= 300;

						}
					}

				}
			}

		}
	}

	// PC collsion with Light Enemy bullets
	for (int i = 0; i < sizeof(lightEnemyBullet) / sizeof(lightEnemyBullet[0]); i++)
	{
		if (lightEnemyBullet[i].isActive)
		{
			lightEnemyBulletRect = { lightEnemyBullet[i].x,lightEnemyBullet[i].y,lightEnemyBullet[i].size,lightEnemyBullet[i].size };

			if (SDL_HasIntersection(&pcRect, &lightEnemyBulletRect))
			{
				lightEnemyBullet[i].isActive = false;
				pc.lives--;
				Mix_PlayChannel(-1, playerHit, 0);
			}
		}
	}

	// PC collision with Heavy Enemy bullets
	for (int i = 0; i < sizeof(heavyEnemyBullet) / sizeof(heavyEnemyBullet[0]); i++)
	{
		if (heavyEnemyBullet[i].isActive)
		{
			heavyEnemyBulletRect = { heavyEnemyBullet[i].x,heavyEnemyBullet[i].y,heavyEnemyBullet[i].size,heavyEnemyBullet[i].size };

			if (SDL_HasIntersection(&pcRect, &heavyEnemyBulletRect))
			{
				heavyEnemyBullet[i].isActive = false;
				pc.lives--;
				Mix_PlayChannel(-1, playerHit, 0);
			}
		}
	}


	// PC collsion with Boss Bullet
	if (bossEnemyBullet.isActive)
	{
		if (SDL_HasIntersection(&pcRect, &bossEnemyBulletRect))
		{
			bossEnemyBullet.isActive = false;
			pc.lives--;
			Mix_PlayChannel(-1, playerHit, 0);
		}
	}

	// =====================================================
	// PC collison with Items 

	// Life Up Item
	if (SDL_HasIntersection(&pcRect, &lifeUpRect))
	{
		pc.lives++;
		lifeUP.isActive = false;
		lifeUP.respawnItem();
		score += 30;
	}

	// Big Bomb
	if (SDL_HasIntersection(&pcRect, &bigBombRect))
	{
		bigBomb.isActive = false;

		for (int i = 0; i < sizeof(heavyEnemies) / sizeof(heavyEnemies[0]); i++)
		{
			if (heavyEnemies[i].x > 0 && heavyEnemies[i].x < SCREEN_WIDTH && heavyEnemies[i].y > 0 && heavyEnemies[i].y < SCREEN_HEIGHT)
			{
				heavyEnemies[i].isAlive = false;
				heavyEnemies[i].deathReset();
			}
		}

		for (int i = 0; i < sizeof(lightEnemies) / sizeof(lightEnemies[0]); i++)
		{
			if (lightEnemies[i].x > 0 && lightEnemies[i].x < SCREEN_WIDTH && lightEnemies[i].y > 0 && lightEnemies[i].y < SCREEN_HEIGHT)
			{
				lightEnemies[i].isAlive = false;
				lightEnemies[i].deathReset();
			}
		}
		
		score += 30;
		bigBomb.respawnItem();

	}

}
// =======================================================
void updateGameObjects()
{
	// Collision Detection
	checkCollision();

	// Player Character
	pc.screenLimit();
	pc.applyDrag();
	pc.updatePosition(frameTime);
	pc.checkIsAlive();

	// PC Bullet
	for (int i = 0; i < sizeof(bullet)/sizeof(bullet[0]); i++)
	{
		if (bullet[i].isActive)
		{
			bullet[i].checkIsOnScreen();
			bullet[i].updatePosition(frameTime);
		}
	}

	// Light Enemy Bullet

	for (int i = 0; i < sizeof(lightEnemyBullet) / sizeof(lightEnemyBullet[0]); i++)
	{
		if (lightEnemyBullet[i].isActive)
		{
			lightEnemyBullet[i].checkIsOnScreen();
			lightEnemyBullet[i].updatePosition(frameTime);
		}
	}

	// Heavy Enemy Bullet

	for (int i = 0; i < sizeof(heavyEnemyBullet) / sizeof(heavyEnemyBullet[0]); i++)
	{
		if (heavyEnemyBullet[i].isActive)
		{
			heavyEnemyBullet[i].checkIsOnScreen();
			heavyEnemyBullet[i].updatePosition(frameTime);

		}
	}

	// Boss Enemy
	if (bossEnemy.isAlive && bossFight)
	{
		Uint32 currentLocalTick = SDL_GetTicks();
		if (3000 < (float)(currentLocalTick - bossEnemyLocalTick))
		{
			if (randomChance(rng) < 50) heavyEnemyBullet[0].fire(bossEnemy.x - 39, bossEnemy.y, -1);
			if (randomChance(rng) > 50) lightEnemyBullet[0].fire(bossEnemy.x + 39, bossEnemy.y, -1);
			bossEnemyBullet.fire(bossEnemy.x, bossEnemy.y, -1);
			bossEnemyLocalTick = currentLocalTick;
		}

		bossEnemy.updatePosition(frameTime, bossEnemyWayPoints[bossEnemy.wayPoint]);
	}

	// Boss Enemy Bullet
	if (bossEnemyBullet.isActive)
	{
		bossEnemyBullet.checkIsOnScreen();
		bossEnemyBullet.updatePosition(frameTime);
	}

	if (!bossFight)
	{
	// Items
		if (lifeUP.isActive)
		{
			lifeUP.checkIsOnScreen();
			lifeUP.updatePosition(frameTime);
		}
		if (bigBomb.isActive)
		{
			bigBomb.checkIsOnScreen();
			bigBomb.updatePosition(frameTime);
		}


		// Randomized item spawner
		Uint32 localItemCurrentTick = SDL_GetTicks();
		if (rndItemSpawnTime < (float)(localItemCurrentTick - itemSpawnLocalTick))
		{
			rndItemSpawnTime = randomItemSpawnTimeGet(rng);
			itemSpawnLocalTick = localItemCurrentTick;
			int rndDecision = randomInt(rng);
			if (!lifeUP.isActive && rndDecision == 0)
			{
				lifeUP.isActive = true;
				lifeUP.respawnItem();
			}
			if (!bigBomb.isActive && rndDecision == 1)
			{
				bigBomb.isActive = true;
				bigBomb.respawnItem();
			}
		

		}



		// Light Enemies
		for (int i = 0; i < sizeof(lightEnemies) / sizeof(lightEnemies[0]); i++)
		{
			// Update position only for alive enemies
			if (lightEnemies[i].isAlive)
			{

				// Enemies shoot after 4 second delay
				Uint32 currentLELocalTick = SDL_GetTicks();
				if (4000 < (float)(currentLELocalTick - lightEnemyLocalTick))
				{
					for (int j = 0; j < sizeof(lightEnemyBullet) / sizeof(lightEnemyBullet[0]); j++)
					{

						lightEnemyBullet[j].fire(lightEnemies[j].x, lightEnemies[j].y, -1);
						Mix_PlayChannel(-1, enemyShoot, 0);
					}
					lightEnemyLocalTick = currentLELocalTick;
				}

				lightEnemies[i].updatePosition(frameTime, lightEnemyWayPoints[lightEnemies[i].wayPoint]);


			}



			// Respawns enemies
			if (lightEnemies[i].y > -SPRITE_SIZE)
			{
				if (lightEnemies[sizeof(lightEnemies) / sizeof(lightEnemies[0]) - 1].isAlive && lightEnemies[sizeof(lightEnemies) / sizeof(lightEnemies[0]) - 1].y > -SPRITE_SIZE)
				{
					lightEnemies[0].isAlive = true;
				}
				lightEnemies[i + 1].isAlive = true;
			}
		}

	

		// Heavy Enemies
		if (score > 1500)
		{
			for (int i = 0; i < sizeof(heavyEnemies) / sizeof(heavyEnemies[0]); i++)
			{
				// Update position only for alive enemies
				if (heavyEnemies[i].isAlive)
				{

					// Enemies shoot after 10 second delay
					Uint32 currentHELocalTick = SDL_GetTicks();
					if (5000 < (float)(currentHELocalTick - heavyEnemyLocalTick))
					{
						for (int j = 0; j < sizeof(heavyEnemyBullet) / sizeof(heavyEnemyBullet[0]); j++)
						{
							heavyEnemyBullet[j].fire(heavyEnemies[j].x, heavyEnemies[j].y, -1);
							Mix_PlayChannel(-1, enemyShoot, 0);
						}
						heavyEnemyLocalTick = currentHELocalTick;
					}

					heavyEnemies[i].updatePosition(frameTime, heavyEnemyWayPoints[heavyEnemies[i].wayPoint]);


				}



				// Respawns enemies
				if (heavyEnemies[i].y > -SPRITE_SIZE)
				{
					if (heavyEnemies[sizeof(heavyEnemies) / sizeof(heavyEnemies[0]) - 1].isAlive && heavyEnemies[sizeof(heavyEnemies) / sizeof(heavyEnemies[0]) - 1].y > -SPRITE_SIZE)
					{
						heavyEnemies[0].isAlive = true;
					}
					heavyEnemies[i + 1].isAlive = true;
				}
			}
		}

	}

	



	
}
// =======================================================
void limitFrameRate(Uint32 startTick)
{
	Uint32 currentTick = SDL_GetTicks();

	if ((1000 / FPS) > currentTick - startTick) {
		SDL_Delay(1000 / FPS - (currentTick - startTick));
		frameTime = (float)1 / FPS;
	}
	else
	{
		frameTime = (float)(currentTick - startTick) / 1000;
	}
}
// =======================================================
void deathScreen()
{

	SDL_Rect spriteRect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	// Clear & Update Screen
	SDL_RenderClear(renderer);
	// Set Background Image
	SDL_RenderCopy(renderer, gameBackground, NULL, &spriteRect);


	// Game Over text

	int textW = 0;
	int textH = 0;
	screenText = "GAME OVER";
	textSurface = TTF_RenderText_Solid(bigFont, screenText.c_str(), redTextColor);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 130, 300, textW, textH };
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);

	screenText = " Press R to restart or Q to quit";
	textSurface = TTF_RenderText_Solid(font, screenText.c_str(), redTextColor);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 120, 380, textW, textH };
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);

	SDL_RenderPresent(renderer);
	// Keyboard input handler


	while (SDL_PollEvent(&playerInputEvent) != 0)
	{
		switch (playerInputEvent.type)
		{
		case SDL_QUIT:
			closeSDL();
			break;
		case SDL_KEYDOWN:
			switch (playerInputEvent.key.keysym.sym)
			{
			case SDLK_r:
				gameOver = false; gamePlaying = true;
				gameStatReset();
				break;
			case SDLK_q:
				closeSDL();
				break;
			}break;

		}

	}
}
// =======================================================
void winScreen()
{
	SDL_Rect spriteRect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	// Clear & Update Screen
	SDL_RenderClear(renderer);
	// Set Background Image
	SDL_RenderCopy(renderer, gameBackground, NULL, &spriteRect);


	// Game Over text

	int textW = 0;
	int textH = 0;
	screenText = "YOU WON!";
	textSurface = TTF_RenderText_Solid(bigFont, screenText.c_str(), greenTextColor);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 150, 300, textW, textH };
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);

	screenText = " Press R to restart or Q to quit";
	textSurface = TTF_RenderText_Solid(font, screenText.c_str(), greenTextColor);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 120, 380, textW, textH };
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);

	SDL_RenderPresent(renderer);
	// Keyboard input handler


	while (SDL_PollEvent(&playerInputEvent) != 0)
	{
		switch (playerInputEvent.type)
		{
		case SDL_QUIT:
			closeSDL();
			break;
		case SDL_KEYDOWN:
			switch (playerInputEvent.key.keysym.sym)
			{
			case SDLK_r:
				gameOver = false; gamePlaying = true;
				gameStatReset();
				break;
			case SDLK_q:
				closeSDL();
				break;
			}break;

		}

	}
}

// =======================================================

// Determine which PC bullet in object array will be activated next
int nextBullet()
{
	if (bulletIndex == sizeof(bullet) / sizeof(bullet[0]))
	{
		bulletIndex = 0;
	}
	else
	{
		bulletIndex++;
	}
	return bulletIndex;
}
// =======================================================

// Starting values 
int xStartWP = 16;
int yStartWP = 16;

// Popluates waypoint objects with coordinates from the Waypoint Map
void createWayPoints(Waypoint *sentWPArr, int sentPattern[][colNum])
{
	int currentWPMap[rowNum][colNum];

	// Create waypoint map based on the pattern
	for (int row = 0; row < rowNum; row++)
	{
		for (int col = 0; col < colNum; col++)
		{
			currentWPMap[row][col] = sentPattern[row][col];
		}
	}

	// Define the size of the array containing waypoitns
	int sizeOfSentArr = numOfWayPoints(sentPattern);


	// Add WayPoints to the board
	for (int row = 0; row < rowNum; row++)
	{
		for (int col = 0; col < colNum; col++)
		{
			if (currentWPMap[row][col] != 0)
			{
				sentWPArr[currentWPMap[row][col] - 1].x = xStartWP + col * SPRITE_SIZE;
				sentWPArr[currentWPMap[row][col] - 1].y = yStartWP + row * SPRITE_SIZE;
			}
		}
	}

	// Set up coordinates for the final waypoint
	sentWPArr[sizeOfSentArr -1].x = sentWPArr[sizeOfSentArr - 2].x;
	sentWPArr[sizeOfSentArr - 1].y = SCREEN_HEIGHT + SPRITE_SIZE;

	
}

// =======================================================

// Reset global variables and propeties of all game objects
void gameStatReset()
{
	score = 0;
	bossFight = false;
	bulletIndex = 0;
	pc.resetStat();
	for (int i = 0; i < sizeof(bullet) / sizeof(bullet[0]); i++)
	{
		bullet[i].resetStat();
	}
	for (int i = 0; i < sizeof(lightEnemyBullet) / sizeof(lightEnemyBullet[0]); i++)
	{
		lightEnemyBullet[i].resetStat();
	}
	for (int i = 0; i < sizeof(heavyEnemyBullet) / sizeof(heavyEnemyBullet[0]); i++)
	{
		heavyEnemyBullet[i].resetStat();
	}
	for (int i = 0; i < sizeof(lightEnemies) / sizeof(lightEnemies[0]); i++)
	{
		lightEnemies[i].resetStat();
	}
	for (int i = 0; i < sizeof(heavyEnemies) / sizeof(heavyEnemies[0]); i++)
	{
		heavyEnemies[i].resetStat();
	}

	bossEnemy.resetStat();
	bossEnemyBullet.resetStat();
}
// =======================================================
void startBossFight()
{
	for (int i = 0; i < sizeof(heavyEnemies) / sizeof(heavyEnemies[0]); i++)
	{
		if (heavyEnemies[i].x > 0 && heavyEnemies[i].x < SCREEN_WIDTH && heavyEnemies[i].y > 0 && heavyEnemies[i].y < SCREEN_HEIGHT)
		{
			heavyEnemies[i].isAlive = false;
			heavyEnemies[i].deathReset();
		}
	}

	for (int i = 0; i < sizeof(lightEnemies) / sizeof(lightEnemies[0]); i++)
	{
		if (lightEnemies[i].x > 0 && lightEnemies[i].x < SCREEN_WIDTH && lightEnemies[i].y > 0 && lightEnemies[i].y < SCREEN_HEIGHT)
		{
			lightEnemies[i].isAlive = false;
			lightEnemies[i].deathReset();
		}
	}
}

// ======================================================
void RenderHPBar(int x, int y, int w, int h, float Percent, SDL_Color FGColor, SDL_Color BGColor) {
	Percent = Percent > 1.f ? 1.f : Percent < 0.f ? 0.f : Percent;
	SDL_Color old;
	SDL_GetRenderDrawColor(renderer, &old.r, &old.g, &old.g, &old.a);
	SDL_Rect bgrect = { x, y, w, h };
	SDL_SetRenderDrawColor(renderer, BGColor.r, BGColor.g, BGColor.b, BGColor.a);
	SDL_RenderFillRect(renderer, &bgrect);
	SDL_SetRenderDrawColor(renderer, FGColor.r, FGColor.g, FGColor.b, FGColor.a);
	int pw = (int)((float)w * Percent);
	int px = x + (w - pw);
	SDL_Rect fgrect = { px, y, pw, h };
	SDL_RenderFillRect(renderer, &fgrect);
	SDL_SetRenderDrawColor(renderer, old.r, old.g, old.b, old.a);
}

SDL_Color color(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_Color col = { r,g,b,a };
	return col;
}
// =======================================================
void closeSDL()
{
	SDL_Delay(1000);

	// Audio
	Mix_FreeChunk(shootSound);
	Mix_FreeChunk(enemyDeath);
	Mix_FreeChunk(enemyShoot);
	Mix_FreeChunk(playerHit);
	Mix_FreeMusic(music);
	Mix_CloseAudio();


	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(gameWindow);
	TTF_CloseFont(font);
	TTF_CloseFont(bigFont);
	TTF_Quit();
	SDL_Quit();
}
// =======================================================
