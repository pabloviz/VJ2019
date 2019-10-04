#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"
#include "Bullet.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4
#define MAX_BULLETS 4


enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT
};


void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	bJumping = false;
	x_pressed = false;

	for (int i = 0; i < MAX_BULLETS; ++i) bullets.push_back(NULL);

	spritesheet.loadFromFile("images/bub.png", TEXTURE_PIXEL_FORMAT_RGBA);
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
		
	posPlayer.x = 1;
	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	playerShaderProgram = shaderProgram;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	
}

void Player::update(int deltaTime)
{
	sprite->update(deltaTime);
	if(Game::instance().getSpecialKey(GLUT_KEY_LEFT))
	{
		if(sprite->animation() != MOVE_LEFT)
			sprite->changeAnimation(MOVE_LEFT);
		posPlayer.x -= 2;
		if(map->collisionMoveLeft(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.x += 2;
			sprite->changeAnimation(STAND_LEFT);
		}
	}
	else if(Game::instance().getSpecialKey(GLUT_KEY_RIGHT))
	{
		if(sprite->animation() != MOVE_RIGHT)
			sprite->changeAnimation(MOVE_RIGHT);
		posPlayer.x += 2;
		if(map->collisionMoveRight(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.x -= 2;
			sprite->changeAnimation(STAND_RIGHT);
		}
	}
	else
	{
		if(sprite->animation() == MOVE_LEFT)
			sprite->changeAnimation(STAND_LEFT);
		else if(sprite->animation() == MOVE_RIGHT)
			sprite->changeAnimation(STAND_RIGHT);
	}
	
	//Crouch + jump, ignores collision and falls down the ground
	if (!bJumping && Game::instance().getSpecialKey(GLUT_KEY_DOWN) && Game::instance().getKey('z')) {
		posPlayer.y += FALL_STEP;
	}
	else if(bJumping)
	{
		jumpAngle += JUMP_ANGLE_STEP;
		if(jumpAngle == 180)
		{
			bJumping = false;
			posPlayer.y = startY;
		}
		else
		{
			posPlayer.y = int(startY - 96 * sin(3.14159f * jumpAngle / 180.f));
			if(jumpAngle > 90)
				bJumping = !map->collisionMoveDown(posPlayer, glm::ivec2(32, 32), &posPlayer.y);
		}
	}
	else
	{
		posPlayer.y += FALL_STEP;
		if(map->collisionMoveDown(posPlayer, glm::ivec2(32, 32), &posPlayer.y))
		{
			if(Game::instance().getKey('z'))
			{
				bJumping = true;
				jumpAngle = 0;
				startY = posPlayer.y;
			}
		}
	}

	//bullets
	for (int i = 0; i < MAX_BULLETS; ++i) {
		if (bullets[i] != NULL) {
			bullets[i]->update(deltaTime);

			//should one despawn?
			if (bullets[i]->farFromPlayer(posPlayer)) {
				delete bullets[i];
				bullets[i] = NULL;
			}
		}
	}

	//fire a bullet
	if (Game::instance().getKey('x')) {
		//x_pressed is used to not allow turbo-fire
		if (x_pressed == false) fireBullet();
		x_pressed = true;
	}
	else x_pressed = false;
	
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::render()
{
	sprite->render();
	for (int i = 0; i < 4; ++i) {
		if (bullets[i] != NULL) bullets[i]->render();
	}
}

void Player::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2 &pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}


void Player::fireBullet() {
	int i = -1;
	bool stop = false;
	//check for free bullet slots
	while (i < MAX_BULLETS-1 && !stop) {
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

		bullets[i]->init(tileMapDispl, playerShaderProgram, direction, posPlayer);
		bullets[i]->setTileMap(map);
	}
}

glm::ivec2 Player::getBulletPos(int i, bool &exists) {
	glm::ivec2 bulletPos;
	if (bullets[i] != NULL) {
		exists = true;
		bulletPos = bullets[i]->getBulletPos();
	}
	else exists = false;
	return bulletPos;
}

glm::ivec2 Player::getPosPlayer() {
	return posPlayer;
}

void Player::despawnBullet(int i) {
	delete bullets[i];
	bullets[i] = NULL;
}



