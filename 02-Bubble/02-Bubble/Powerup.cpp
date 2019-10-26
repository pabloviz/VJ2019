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

void Powerup::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Scene *scene)
{
	spritesheet.loadFromFile("images/powerup.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(32, 16), glm::vec2(1, 0.5f), &spritesheet, &shaderProgram, scene);
	sprite->setNumberAnimations(1);

	sprite->setAnimationSpeed(0, 8);
	sprite->addKeyframe(0, glm::vec2(0.f, 0.f));
	sprite->changeAnimation(0);
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
