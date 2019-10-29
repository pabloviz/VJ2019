#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"

#define HORIZ_VEL 8
#define VERT_VEL 8
#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4

#define TILESHEET_H 0.5f
#define TILESHEET_V 0.25f

enum PowerupAnims
{
	SPREAD, BULLETS, FAST, CAMOUFLAGE
};

void Powerup::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Scene *scene, int type)
{

	this->type = type;

	spritesheet.loadFromFile("images/powerup.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(32, 16), glm::vec2(TILESHEET_H, TILESHEET_V), &spritesheet, &shaderProgram, scene);
	sprite->setNumberAnimations(4);

	sprite->setAnimationSpeed(SPREAD, 8);
	sprite->addKeyframe(SPREAD, glm::vec2(0.f, 0.f));

	sprite->setAnimationSpeed(BULLETS, 8);
	sprite->addKeyframe(BULLETS, glm::vec2(TILESHEET_H, 0.f));

	sprite->setAnimationSpeed(FAST, 8);
	sprite->addKeyframe(FAST, glm::vec2(0.f, TILESHEET_V));

	sprite->setAnimationSpeed(CAMOUFLAGE, 8);
	sprite->addKeyframe(CAMOUFLAGE, glm::vec2(TILESHEET_H, TILESHEET_V));


	sprite->changeAnimation(type);
	tileMapDispl = tileMapPos;

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPowerup.x), float(tileMapDispl.y + posPowerup.y)));

}

void Powerup::update(int deltaTime)
{
	sprite->update(deltaTime);

	posPowerup.y += FALL_STEP;
	map->collisionMoveDown(posPowerup, glm::ivec2(32, 16), &posPowerup.y, false);

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPowerup.x), float(tileMapDispl.y + posPowerup.y)));
}

void Powerup::render(glm::vec2 posPlayer, float angle)
{
	sprite->render(posPlayer, angle);
}

void Powerup::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void Powerup::setPosition(const glm::vec2 &pos)
{
	posPowerup = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPowerup.x), float(tileMapDispl.y + posPowerup.y)));
}


bool Powerup::farFromPlayer(const glm::vec2 &posPlayer) {
	glm::vec2 aux = posPlayer;
	if ((posPowerup.x > (aux.x + 500)) || (posPowerup.x < (aux.x - 500))) return true;
	if ((posPowerup.y > (aux.y + 500)) || (posPowerup.y < (aux.y - 500))) return true;
	else return false;

}

glm::ivec2 Powerup::getPowerupPos() {
	return posPowerup;
}

int Powerup::getType() {
	return type;
}
