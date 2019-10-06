#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Scene.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4
#define MAX_BULLETS 4


enum EnemyAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT
};

enum EnemyType
{
	RUNNING, SHOOTING, TURRET, CHASE
};


void Enemy::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Player *player, int type, Scene *scene)
{
	this->player = player;
	this->type = type;
	this->scene = scene;
	this->ticks = 0;

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
	ticks++; //times "update" has been called

	sprite->update(deltaTime);
	if (type == CHASE) {
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
	}
	else if (type == RUNNING) {
		if (sprite->animation() != MOVE_LEFT)
			sprite->changeAnimation(MOVE_LEFT);
		posEnemy.x -= 1;

		if (map->collisionMoveLeft(posEnemy, glm::ivec2(32, 32)))
		{
			posEnemy.x += 1;
			sprite->changeAnimation(STAND_LEFT);
		}

		posEnemy.y += FALL_STEP;
		map->collisionMoveDown(posEnemy, glm::ivec2(32, 32), &posEnemy.y);

	}
	else if (type == SHOOTING) {
		sprite->changeAnimation(STAND_LEFT);

		posEnemy.y += FALL_STEP;
		map->collisionMoveDown(posEnemy, glm::ivec2(32, 32), &posEnemy.y);

		glm::ivec2 posPlayer = scene->getPosPlayer();
		glm::ivec2 direction;
		if (posPlayer.x != -1 && (ticks%10 == 0)) { //player must exist, and enemy fires with an interval between bullets
			if (posPlayer.x < posEnemy.x - 30) direction.x = -1;
			else if (posPlayer.x > posEnemy.x + 30) direction.x = 1;
			else direction.x = 0;

			if (posPlayer.y < posEnemy.y - 30) direction.y = -1;
			else if (posPlayer.y > posEnemy.y + 30) direction.y = 1;
			else direction.y = 0;

			//+-30 on enemy positions establishes the boundaries of enemy vision. Tweak if desired
			scene->addBullet(direction, posEnemy, false);
		}

	}
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));

}

void Enemy::render()
{
	sprite->render();
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

glm::ivec2 Enemy::getEnemyPos() {
	return posEnemy;
}



