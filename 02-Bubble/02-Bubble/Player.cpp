#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"
#include "Bullet.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 64
#define FALL_STEP 4
#define MAX_BULLETS 4
#define PLAYER_SPEED 1

#define TILESHEET_H 0.125
#define TILESHEET_V 0.1875


enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, MOVE_LEFT_UP, MOVE_RIGHT_UP,
	MOVE_LEFT_DOWN, MOVE_RIGHT_DOWN, JUMP_LEFT, JUMP_RIGHT
};


void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Scene *scene)
{
	bJumping = false;
	x_pressed = false;
	this->scene = scene;

	spritesheet.loadFromFile("images/contraspritesheet.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(32, 48), glm::vec2(TILESHEET_H, TILESHEET_V), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(10);
	
		sprite->setAnimationSpeed(STAND_LEFT, 8);
		sprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.f));
		
		sprite->setAnimationSpeed(STAND_RIGHT, 8);
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.25f, 0.f));
		
		sprite->setAnimationSpeed(MOVE_LEFT, 8);
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 3 * TILESHEET_V));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(7 * TILESHEET_H, 2 * TILESHEET_V));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(6 * TILESHEET_H, 2 * TILESHEET_V));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(5 * TILESHEET_H, 2 * TILESHEET_V));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(4 * TILESHEET_H, 2 * TILESHEET_V));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(5 * TILESHEET_H, 2 * TILESHEET_V));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(6 * TILESHEET_H, 2 * TILESHEET_V));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(7 * TILESHEET_H, 2 * TILESHEET_V));

		sprite->setAnimationSpeed(MOVE_RIGHT, 8);
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(TILESHEET_H, 3 * TILESHEET_V));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(2 * TILESHEET_H, 3 * TILESHEET_V));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(3 * TILESHEET_H, 3 * TILESHEET_V));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(4 * TILESHEET_H, 3 * TILESHEET_V));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(5 * TILESHEET_H, 3 * TILESHEET_V));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(4 * TILESHEET_H, 3 * TILESHEET_V));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(3 * TILESHEET_H, 3 * TILESHEET_V));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(2 * TILESHEET_H, 3 * TILESHEET_V));

		sprite->setAnimationSpeed(MOVE_RIGHT_UP, 8);
		sprite->addKeyframe(MOVE_RIGHT_UP, glm::vec2(3 * TILESHEET_H, TILESHEET_V));
		sprite->addKeyframe(MOVE_RIGHT_UP, glm::vec2(4 * TILESHEET_H, TILESHEET_V));
		sprite->addKeyframe(MOVE_RIGHT_UP, glm::vec2(5 * TILESHEET_H, TILESHEET_V));

		sprite->setAnimationSpeed(MOVE_RIGHT_DOWN, 8);
		sprite->addKeyframe(MOVE_RIGHT_DOWN, glm::vec2(TILESHEET_H, 2 * TILESHEET_V));
		sprite->addKeyframe(MOVE_RIGHT_DOWN, glm::vec2(2 * TILESHEET_H, 2 * TILESHEET_V));
		sprite->addKeyframe(MOVE_RIGHT_DOWN, glm::vec2(3 * TILESHEET_H, 2 * TILESHEET_V));

		sprite->setAnimationSpeed(MOVE_LEFT_UP, 8);
		sprite->addKeyframe(MOVE_LEFT_UP, glm::vec2(0.f, TILESHEET_V));
		sprite->addKeyframe(MOVE_LEFT_UP, glm::vec2(TILESHEET_H, TILESHEET_V));
		sprite->addKeyframe(MOVE_LEFT_UP, glm::vec2(2 * TILESHEET_H, TILESHEET_V));

		sprite->setAnimationSpeed(MOVE_LEFT_DOWN, 8);
		sprite->addKeyframe(MOVE_LEFT_DOWN, glm::vec2(6 * TILESHEET_H, TILESHEET_V));
		sprite->addKeyframe(MOVE_LEFT_DOWN, glm::vec2(7 * TILESHEET_H, TILESHEET_V));
		sprite->addKeyframe(MOVE_LEFT_DOWN, glm::vec2(0.f, 2 * TILESHEET_V));

		sprite->setAnimationSpeed(JUMP_LEFT, 8);
		sprite->addKeyframe(JUMP_LEFT, glm::vec2(6 * TILESHEET_H, 3 * TILESHEET_V));
		sprite->addKeyframe(JUMP_LEFT, glm::vec2(7 * TILESHEET_H, 3 * TILESHEET_V));
		sprite->addKeyframe(JUMP_LEFT, glm::vec2(0.f, 4 * TILESHEET_V));
		sprite->addKeyframe(JUMP_LEFT, glm::vec2(TILESHEET_H, 4 * TILESHEET_V));

		sprite->setAnimationSpeed(JUMP_RIGHT, 8);
		sprite->addKeyframe(JUMP_RIGHT, glm::vec2(2 * TILESHEET_H, 4 * TILESHEET_V));
		sprite->addKeyframe(JUMP_RIGHT, glm::vec2(3 * TILESHEET_H, 4 * TILESHEET_V));
		sprite->addKeyframe(JUMP_RIGHT, glm::vec2(4 * TILESHEET_H, 4 * TILESHEET_V));
		sprite->addKeyframe(JUMP_RIGHT, glm::vec2(5 * TILESHEET_H, 4 * TILESHEET_V));



		
	posPlayer.x = 1;
	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	
}

void Player::update(int deltaTime)
{
	sprite->update(deltaTime);
	bool air = false;

	//Crouch + jump, ignores collision and falls down the ground
	if (!bJumping && Game::instance().getSpecialKey(GLUT_KEY_DOWN) && Game::instance().getKey('z')) {
		posPlayer.y += FALL_STEP;
		air = true;

	}
	else if (bJumping)
	{
		jumpAngle += JUMP_ANGLE_STEP;
		if (jumpAngle == 180)
		{
			bJumping = false;
			posPlayer.y = startY;
		}
		else
		{
			posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
			if (jumpAngle > 90)
				bJumping = !map->collisionMoveDown(posPlayer, glm::ivec2(32, 48), &posPlayer.y);
		}
		air = true;
	}
	else
	{
		posPlayer.y += FALL_STEP;
		if (map->collisionMoveDown(posPlayer, glm::ivec2(32, 48), &posPlayer.y))
		{
			if (Game::instance().getKey('z'))
			{
				bJumping = true;
				jumpAngle = 0;
				startY = posPlayer.y;
			}
		}
		else air = true;
	}

	//LEFT IS PUSHED
	if(Game::instance().getSpecialKey(GLUT_KEY_LEFT))
	{
		//IF PLAYER WASN'T ALREADY RUNNING, IT STARTS TO
		if (!air && sprite->animation() != MOVE_LEFT && sprite->animation() != MOVE_LEFT_UP 
			&& sprite->animation() != MOVE_LEFT_DOWN) {
			sprite->changeAnimation(MOVE_LEFT);
		}
		
		else if (air && sprite->animation() != JUMP_LEFT) sprite->changeAnimation(JUMP_LEFT);

		//IF PLAYER AIMS DIAGONAL AND PREVIOUSLY WASN'T DOING IT, IT STARTS TO
		if (!air && Game::instance().getSpecialKey(GLUT_KEY_UP) && sprite->animation() != MOVE_LEFT_UP) 
			sprite->changeAnimation(MOVE_LEFT_UP);
		else if (! air && Game::instance().getSpecialKey(GLUT_KEY_DOWN) && sprite->animation() != MOVE_LEFT_DOWN)
			sprite->changeAnimation(MOVE_LEFT_DOWN);

		//CHECK COLLISION
		posPlayer.x -= PLAYER_SPEED;
		if(map->collisionMoveLeft(posPlayer, glm::ivec2(32, 48)))
		{
			posPlayer.x += PLAYER_SPEED;
			if (!air) sprite->changeAnimation(STAND_LEFT);
		}
	}

	//RIGHT IS PUSHED
	else if(Game::instance().getSpecialKey(GLUT_KEY_RIGHT))
	{

		//IF PLAYER WASN'T ALREADY RUNNING, IT STARTS TO
		if(!air && sprite->animation() != MOVE_RIGHT && sprite->animation() != MOVE_RIGHT_UP
			&& sprite->animation() != MOVE_RIGHT_DOWN)
			sprite->changeAnimation(MOVE_RIGHT);

		else if (air && sprite->animation() != JUMP_RIGHT) sprite->changeAnimation(JUMP_RIGHT);

		//IF PLAYER AIMS DIAGONAL AND PREVIOUSLY WASN'T DOING IT, IT STARTS TO
		if (!air && Game::instance().getSpecialKey(GLUT_KEY_UP) && sprite->animation() != MOVE_RIGHT_UP)
			sprite->changeAnimation(MOVE_RIGHT_UP);
		else if (!air && Game::instance().getSpecialKey(GLUT_KEY_DOWN) && sprite->animation() != MOVE_RIGHT_DOWN)
			sprite->changeAnimation(MOVE_RIGHT_DOWN);

		//CHECK COLLISION
		posPlayer.x += PLAYER_SPEED;
		if(map->collisionMoveRight(posPlayer, glm::ivec2(32, 48)))
		{
			posPlayer.x -= PLAYER_SPEED;
			if (!air) sprite->changeAnimation(STAND_RIGHT);

		}
	}
	else
	{
		if(!air && (sprite->animation() == MOVE_LEFT || sprite->animation() == MOVE_LEFT_UP
			|| sprite->animation() == MOVE_LEFT_DOWN || sprite->animation() == JUMP_LEFT))
			sprite->changeAnimation(STAND_LEFT);
		else if(!air && (sprite->animation() == MOVE_RIGHT || sprite->animation() == MOVE_RIGHT_UP
			|| sprite->animation() == MOVE_RIGHT_DOWN || sprite->animation() == JUMP_RIGHT))
			sprite->changeAnimation(STAND_RIGHT);
		else if (air && (sprite->animation() == MOVE_LEFT || sprite->animation() == MOVE_LEFT_UP
			|| sprite->animation() == MOVE_LEFT_DOWN || sprite->animation() == STAND_LEFT))
			sprite->changeAnimation(JUMP_LEFT);
		else if (air && (sprite->animation() == MOVE_RIGHT || sprite->animation() == MOVE_RIGHT_UP
			|| sprite->animation() == MOVE_RIGHT_DOWN || sprite->animation() == STAND_RIGHT))
			sprite->changeAnimation(JUMP_RIGHT);
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
	
	(*scene).addBullet(direction, posPlayer, true);
}


glm::ivec2 Player::getPosPlayer() {
	return posPlayer;
}