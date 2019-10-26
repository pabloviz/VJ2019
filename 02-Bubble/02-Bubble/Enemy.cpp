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
	MOVE_LEFT, SHOOTUL, SHOOTL, SHOOTDL, SHOOTUR, SHOOTR, SHOOTDR, EXPLODE, TV
};

enum EnemyType
{
	RUNNING, SHOOTING, TURRET, CHASE
};


void Enemy::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, int type, Scene *scene, int id)
{
	this->id = id;
	this->type = type;
	this->scene = scene;
	this->ticks = 0;
	this->deathTicks = 0;
	this->dying = false;


	bJumping = false;

	for (int i = 0; i < MAX_BULLETS; ++i) bullets.push_back(NULL);

	spritesheet.loadFromFile("images/enemies_sprites.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(32, 48), glm::vec2(TILESHEET_H, TILESHEET_V), &spritesheet, &shaderProgram, scene);
	sprite->setNumberAnimations(9);

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

	sprite->setAnimationSpeed(TV, 8);
	sprite->addKeyframe(TV, glm::vec2(7 * TILESHEET_H, TILESHEET_V));


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
		posEnemy.x -= 0.5;

		if (map->collisionMoveLeft(posEnemy, glm::ivec2(32, 48), true))
		{
			posEnemy.x += 0.5;
		}

		posEnemy.y += FALL_STEP;
		map->collisionMoveDown(posEnemy, glm::ivec2(32, 48), &posEnemy.y, true);

	}
	else if (type == SHOOTING) {

		posEnemy.y += FALL_STEP;
		map->collisionMoveDown(posEnemy, glm::ivec2(32, 48), &posEnemy.y, true);
		glm::vec2 posPlayer = scene->getPosPlayer();
		glm::vec2 posAux;
		glm::vec2 posEnemyAux = posEnemy;
		float angle;
		
		if (posPlayer.x != -1) { //player must exist, and enemy fires with an interval between bullets
			
			posAux.x = posPlayer.x - posEnemy.x;
			posAux.y = posPlayer.y - posEnemy.y;
			angle = -atan(posAux.y / posAux.x);
			if (posPlayer.x > posEnemy.x) angle += 3.14f;
			

			if (cos(angle) < 0) {
				posEnemyAux.x += 25;
				sprite->changeAnimation(SHOOTR);
				if (sin(angle) > 0.5f) {
					sprite->changeAnimation(SHOOTDR);
				}
				else if (sin(angle) < -0.5f) {
					sprite->changeAnimation(SHOOTUR);
				}
			}

			else {
				sprite->changeAnimation(SHOOTL);
				if (sin(angle) > 0.5f) {
					sprite->changeAnimation(SHOOTDL);
				}
				else if (sin(angle) < -0.5f) {
					sprite->changeAnimation(SHOOTUL);
				}
			}

			posEnemyAux.y += 16;
			if (ticks % 50 == 0)
			scene->addBullet(angle, posEnemyAux, false);
		}

	}

	if (type == CHASE) {

		sprite->changeAnimation(TV);
		glm::vec2 posPlayer = scene->getPosPlayer();
		glm::vec2 posAux;
		glm::vec2 posEnemyAux = posEnemy;
		float angle;

		if (posPlayer.x != -1) { //player must exist, and enemy fires with an interval between bullets

			glm::vec2 posEnemyAux = posEnemy;
			posEnemyAux.x += 16;
			posEnemyAux.y += 28;
			posAux.x = posPlayer.x - posEnemyAux.x;
			posAux.y = posPlayer.y - posEnemyAux.y;
			angle = -atan(posAux.y / posAux.x);
			if (posPlayer.x >= posEnemyAux.x) angle += 3.14f;

			this->angle = angle;


			if (ticks % 200 == 0) {
				float hypothenuse = 33.94f;
				glm::vec2 bulletSpawn = posEnemy;
				//center
				bulletSpawn.x += 16;
				bulletSpawn.y += 24;
				bulletSpawn.x -= cos(angle)*hypothenuse;
				bulletSpawn.y += sin(angle)*hypothenuse;

				scene->addBullet(angle, bulletSpawn, false);
			}

			if (posAux.y >= 50 || posAux.y <= -50) posEnemy.y += sin(angle)*0.2f;
			if (posAux.x >= 50 || posAux.x <= -50) posEnemy.x -= cos(angle)*0.2f;
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

void Enemy::render(glm::vec2 posPlayer, float angle)
{
	if (type == CHASE) sprite->renderChase(posPlayer, angle, -(this->angle - 3.14/2));
	else sprite->render(posPlayer, angle);
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