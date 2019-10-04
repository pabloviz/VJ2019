#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"
#include "Bullet.h"
#include "Enemy.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4
#define MAX_BULLETS 4


enum EnemyAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT
};


void Enemy::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Player *player)
{
	this->player = player;
	bJumping = false;

	for (int i = 0; i < MAX_BULLETS; ++i) bullets.push_back(NULL);

	spritesheet.loadFromFile("images/evilbub.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(32, 32), glm::vec2(0.25, 0.25), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(4);

	sprite->setAnimationSpeed(STAND_LEFT, 8);
	sprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.f));

	sprite->setAnimationSpeed(STAND_RIGHT, 8);
	sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.25f, 0.f));

	sprite->setAnimationSpeed(MOVE_LEFT, 8);
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.25f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.5f));

	sprite->setAnimationSpeed(MOVE_RIGHT, 8);
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25, 0.f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25, 0.25f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25, 0.5f));

	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	playerShaderProgram = shaderProgram;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));

}

void Enemy::update(int deltaTime)
{
	sprite->update(deltaTime);

	glm::ivec2 posPlayer = player->getPosPlayer();
	
	if (posPlayer.x < posEnemy.x)
	{
		if (sprite->animation() != MOVE_LEFT)
			sprite->changeAnimation(MOVE_LEFT);
		posEnemy.x -= 1;
		if (map->collisionMoveLeft(posEnemy, glm::ivec2(32, 32)))
		{
			posEnemy.x += 1;
			sprite->changeAnimation(STAND_LEFT);
		}
	}
	else if (posPlayer.x > posEnemy.x)
	{
		if (sprite->animation() != MOVE_RIGHT)
			sprite->changeAnimation(MOVE_RIGHT);
		posEnemy.x += 1;
		if (map->collisionMoveRight(posEnemy, glm::ivec2(32, 32)))
		{
			posEnemy.x -= 1;
			sprite->changeAnimation(STAND_RIGHT);
		}
	}
	else
	{
		if (sprite->animation() == MOVE_LEFT)
			sprite->changeAnimation(STAND_LEFT);
		else if (sprite->animation() == MOVE_RIGHT)
			sprite->changeAnimation(STAND_RIGHT);
	}

	//Crouch + jump, ignores collision and falls down the ground
	if (!bJumping && (posEnemy.y < posPlayer.y)) {
		posEnemy.y += FALL_STEP;
	}
	else if (bJumping)
	{
		jumpAngle += JUMP_ANGLE_STEP;
		if (jumpAngle == 180)
		{
			bJumping = false;
			posEnemy.y = startY;
		}
		else
		{
			posEnemy.y = int(startY - 96 * sin(3.14159f * jumpAngle / 180.f));
			if (jumpAngle > 90)
				bJumping = !map->collisionMoveDown(posEnemy, glm::ivec2(32, 32), &posEnemy.y);
		}
	}
	else
	{
		posEnemy.y += FALL_STEP;
		if (map->collisionMoveDown(posEnemy, glm::ivec2(32, 32), &posEnemy.y))
		{
			if (posPlayer.y < posEnemy.y)
			{
				bJumping = true;
				jumpAngle = 0;
				startY = posEnemy.y;
			}
		}
	}
	
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}

void Enemy::render()
{
	sprite->render();
	for (int i = 0; i < 4; ++i) {
		if (bullets[i] != NULL) bullets[i]->render();
	}
}

void Enemy::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void Enemy::setPosition(const glm::vec2 &pos)
{
	posEnemy = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}

void Enemy::fireBullet() {
	int i = -1;
	bool stop = false;
	//check for free bullet slots
	while (i < MAX_BULLETS - 1 && !stop) {
		++i;
		if (bullets[i] == NULL) stop = true;
	}

	if (stop == true) {
		bullets[i] = new Bullet();
		glm::ivec2 direction; //shooting direction

		bool pressed = false; //This will tell us if the user is pressing a direction
		if (Game::instance().getSpecialKey(GLUT_KEY_DOWN)) {
			direction.y = 1;
			pressed = true;
		}
		else if (Game::instance().getSpecialKey(GLUT_KEY_UP)) {
			direction.y = -1;
			pressed = true;
		}
		if (Game::instance().getSpecialKey(GLUT_KEY_LEFT)) {
			direction.x = -1;
			pressed = true;
		}
		else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) {
			direction.x = 1;
			pressed = true;
		}

		//if they are not pressing any key, they will fire forwards
		if (!pressed) {
			if (sprite->animation() == STAND_LEFT) direction.x = -1;
			else direction.x = 1;
		}

		bullets[i]->init(tileMapDispl, playerShaderProgram, direction, posEnemy);
		bullets[i]->setTileMap(map);
	}
}

glm::ivec2 Enemy::getEnemyPos() {
	return posEnemy;
}



