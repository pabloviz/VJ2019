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

#define TILESHEET_H 0.125
#define TILESHEET_V 0.1875

enum EnemyAnims
{
	MOVE_LEFT, SHOOTUL, SHOOTL, SHOOTDL, SHOOTUR, SHOOTR, SHOOTDR, EXPLODE
};

enum EnemyType
{
	RUNNING, SHOOTING, TURRET, CHASE
};


void Enemy::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Player *player, int type, Scene *scene, int id)
{
	this->id = id;
	this->player = player;
	this->type = type;
	this->scene = scene;
	this->ticks = 0;
	this->deathTicks = 0;
	this->dying = false;

	bJumping = false;

	for (int i = 0; i < MAX_BULLETS; ++i) bullets.push_back(NULL);

	spritesheet.loadFromFile("images/enemies_sprites.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(32, 48), glm::vec2(TILESHEET_H, TILESHEET_V), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(8);

	sprite->setAnimationSpeed(MOVE_LEFT, 8);
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(6 * TILESHEET_H, 0.f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(7 * TILESHEET_H, 0.f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, TILESHEET_V));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(TILESHEET_H, TILESHEET_V));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(2 * TILESHEET_H, TILESHEET_V));

	sprite->setAnimationSpeed(SHOOTUL, 8);
	sprite->addKeyframe(SHOOTUL, glm::vec2(0.f, 0.f));

	sprite->setAnimationSpeed(SHOOTL, 8);
	sprite->addKeyframe(SHOOTL, glm::vec2(TILESHEET_H, 0.f));

	sprite->setAnimationSpeed(SHOOTDL, 8);
	sprite->addKeyframe(SHOOTDL, glm::vec2(2 * TILESHEET_H, 0.f));

	sprite->setAnimationSpeed(SHOOTUR, 8);
	sprite->addKeyframe(SHOOTUR, glm::vec2(5 * TILESHEET_H, 0.f));

	sprite->setAnimationSpeed(SHOOTR, 8);
	sprite->addKeyframe(SHOOTR, glm::vec2(4 * TILESHEET_H, 0.f));

	sprite->setAnimationSpeed(SHOOTDR, 8);
	sprite->addKeyframe(SHOOTDR, glm::vec2(3 * TILESHEET_H, 0.f));

	sprite->setAnimationSpeed(EXPLODE, 4);
	sprite->addKeyframe(EXPLODE, glm::vec2(4 * TILESHEET_H, TILESHEET_V));
	sprite->addKeyframe(EXPLODE, glm::vec2(5 * TILESHEET_H, TILESHEET_V));
	sprite->addKeyframe(EXPLODE, glm::vec2(6 * TILESHEET_H, TILESHEET_V));


	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	playerShaderProgram = shaderProgram;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));

}

void Enemy::update(int deltaTime)
{
	ticks++; //times "update" has been called

	sprite->update(deltaTime);
	
	if (dying) {
		updateDeath();
		return;
	}

	if (type == RUNNING) {
		if (sprite->animation() != MOVE_LEFT)
			sprite->changeAnimation(MOVE_LEFT);
		posEnemy.x -= 1;

		if (map->collisionMoveLeft(posEnemy, glm::ivec2(32, 48)))
		{
			posEnemy.x += 1;
		}

		posEnemy.y += FALL_STEP;
		map->collisionMoveDown(posEnemy, glm::ivec2(32, 48), &posEnemy.y);

	}
	else if (type == SHOOTING) {

		posEnemy.y += FALL_STEP;
		map->collisionMoveDown(posEnemy, glm::ivec2(32, 48), &posEnemy.y);
		glm::ivec2 posPlayer = scene->getPosPlayer();
		glm::ivec2 direction;
		glm::ivec2 posEnemyAux = posEnemy;
		posEnemyAux.y += 16;
		
		if (posPlayer.x != -1) { //player must exist, and enemy fires with an interval between bullets
			
			if (posPlayer.x < posEnemy.x - 30) {
				direction.x = -1;
				sprite->changeAnimation(SHOOTL);
				if (posPlayer.y < posEnemy.y - 30) {
					direction.y = -1;
					sprite->changeAnimation(SHOOTUL);
				}
				else if (posPlayer.y > posEnemy.y + 30) {
					direction.y = 1;
					sprite->changeAnimation(SHOOTDL);
				}
				else direction.y = 0;
				if (ticks % 50 == 0)
				scene->addBullet(direction, posEnemyAux, false);

			}
			else if (posPlayer.x > posEnemy.x + 30) {
				direction.x = 1;
				sprite->changeAnimation(SHOOTR);
				if (posPlayer.y < posEnemy.y - 30) {
					direction.y = -1;
					sprite->changeAnimation(SHOOTUR);
				}
				else if (posPlayer.y > posEnemy.y + 30) {
					direction.y = 1;
					sprite->changeAnimation(SHOOTDR);
				}
				else direction.y = 0;
				if (ticks % 50 == 0)
				scene->addBullet(direction, posEnemyAux, false);

			}
			//+-30 on enemy positions establishes the boundaries of enemy vision. Tweak if desired
		}

	}

	else if (type == TURRET) {

		glm::ivec2 posPlayer = scene->getPosPlayer();
		glm::ivec2 direction;
		if (posPlayer.x != -1 && (ticks % 50 == 0)) { //player must exist, and enemy fires with an interval between bullets
			if (posPlayer.x < posEnemy.x - 30) {
				direction.x = -1;
				//sprite->changeAnimation(STAND_LEFT);
			}
			else if (posPlayer.x > posEnemy.x + 30) {
				direction.x = 1;
				//sprite->changeAnimation(STAND_RIGHT);
			}
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

void Enemy::updateDeath() {
	deathTicks++;
	if (deathTicks <= 30) posEnemy.y -= 2;
	else if (sprite->animation() != EXPLODE) sprite->changeAnimation(EXPLODE);
	if (deathTicks >= 70) scene->enemyDeath(this->id);
	else sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));

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

void Enemy::setDying(bool dying) {
	this->dying = dying;
}

bool Enemy::getDying() {
	return this->dying;
}