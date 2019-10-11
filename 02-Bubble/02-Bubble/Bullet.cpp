#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Bullet.h"
#include "Game.h"

#define HORIZ_VEL 8
#define VERT_VEL 8
#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4

enum BulletAnims
{
	FIRE
};

void Bullet::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, glm::ivec2 &direction, 
				  glm::ivec2 &posPlayer, bool friendly)
{
	this->friendly = friendly;
	spritesheet.loadFromFile("images/bullet.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(8, 8), glm::vec2(1, 1), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(1);

	sprite->setAnimationSpeed(FIRE, 8);
	sprite->addKeyframe(FIRE, glm::vec2(0.f, 0.f));
	sprite->changeAnimation(FIRE);
	tileMapDispl = tileMapPos;
	bulletDirection = direction;
	posBullet = posPlayer;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBullet.x), float(tileMapDispl.y + posBullet.y)));
	
}

void Bullet::update(int deltaTime)
{
	sprite->update(deltaTime);

	if (bulletDirection.x == 1) posBullet.x += HORIZ_VEL;
	if (bulletDirection.x == -1) posBullet.x -= HORIZ_VEL;
	if (bulletDirection.y == 1) posBullet.y += VERT_VEL;
	if (bulletDirection.y == -1) posBullet.y -= VERT_VEL;
	
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBullet.x), float(tileMapDispl.y + posBullet.y)));
}

void Bullet::render()
{
	sprite->render();
}

void Bullet::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void Bullet::setPosition(const glm::vec2 &pos)
{
	posBullet = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBullet.x), float(tileMapDispl.y + posBullet.y)));
}


bool Bullet::farFromPlayer(const glm::vec2 &posPlayer) {
	glm::vec2 aux = posPlayer;
	if ((posBullet.x > (aux.x + 500)) || (posBullet.x < (aux.x - 500))) return true;
	if ((posBullet.y > (aux.y + 500)) || (posBullet.y < (aux.y - 500))) return true;
	else return false;

}

glm::ivec2 Bullet::getBulletPos() {
	return posBullet;
}

bool Bullet::getFriendly() {
	return friendly;
}