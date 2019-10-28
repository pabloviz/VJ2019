#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"
#include "Bullet.h"
#include "Boss.h"
#include "Scene.h"

#define TILESHEET_H 0.5
#define TILESHEET_V 0.5

enum BossAnims
{
	CLOSED, OPEN, DYING
};

enum EnemyType
{
	RUNNING, SHOOTING, TURRET, CHASE
};


void Boss::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Scene *scene)
{
	this->scene = scene;
	this->ticks = 0;
	this->deathTicks = 0;
	this->dying = false;
	this->life = 20;

	spritesheet.loadFromFile("images/boss_sprites.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(128, 128), glm::vec2(TILESHEET_H, TILESHEET_V), &spritesheet, &shaderProgram, scene);
	sprite->setNumberAnimations(3);

	sprite->setAnimationSpeed(CLOSED, 8);
	sprite->addKeyframe(CLOSED, glm::vec2(0.f, 0.f));
	
	sprite->setAnimationSpeed(OPEN, 8);
	sprite->addKeyframe(OPEN, glm::vec2(TILESHEET_H, 0.f));


	sprite->setAnimationSpeed(DYING, 8);
	sprite->addKeyframe(DYING, glm::vec2(0.f, TILESHEET_V));
	sprite->addKeyframe(DYING, glm::vec2(TILESHEET_H, TILESHEET_V));



	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	playerShaderProgram = shaderProgram;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBoss.x), float(tileMapDispl.y + posBoss.y)));

}

void Boss::update(int deltaTime)
{
	ticks++; //times "update" has been called

	sprite->update(deltaTime);

	if (dying) {
		updateDeath();
		return;
	}

	glm::vec2 posPlayer = scene->getPosPlayer();
	glm::vec2 posAux;
	float angle;

	//FIRE BULLETS
	if (posPlayer.x != -1 && (ticks % 40 == 0) && sprite->animation() == OPEN) { 
		//player must exist, and enemy fires with an interval between bullets
		glm::ivec2 posBossAux = posBoss;
		posBossAux.x += 2 * 16;
		posBossAux.y += 5 * 16;

		posAux.x = posPlayer.x + 16 - posBossAux.x;
		posAux.y = posPlayer.y + 28 - posBossAux.y;
		angle = -atan(posAux.y / posAux.x);
		if (posAux.x > 0) angle += 3.14;
		
		scene->addBullet(angle, posBossAux, false);
	}

	//SPAWN LARVAS (TBI)
	else if (posPlayer.x != -1 && (ticks % 20 == 0) && sprite->animation() == CLOSED) {
		//player must exist, and boss spawns enemies in intervals
		
		scene->spawnEnemy(posBoss, RUNNING);
	}
	

	//CHANGE ANIMS
	if (ticks % 100 == 0) {
		if (sprite->animation() == CLOSED) sprite->changeAnimation(OPEN);
		else sprite->changeAnimation(CLOSED);
	}
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBoss.x), float(tileMapDispl.y + posBoss.y)));

}

void Boss::updateDeath() {
	deathTicks++;
	if (sprite->animation() != DYING) sprite->changeAnimation(DYING);
	if (deathTicks >= 70) scene->bossDeath();
}

void Boss::render(glm::vec2 posPlayer, float angle)
{
	sprite->render(posPlayer, angle);
}

void Boss::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void Boss::setPosition(const glm::vec2 &pos)
{
	posBoss = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBoss.x), float(tileMapDispl.y + posBoss.y)));
}

glm::ivec2 Boss::getBossPos() {
	return posBoss;
}

void Boss::setDying(bool dying) {
	this->dying = dying;
}

bool Boss::getDying() {
	return this->dying;
}

void Boss::decrementLife() {
	this->life--;
	if (life == 0) {
		this->dying = true;
	}
}