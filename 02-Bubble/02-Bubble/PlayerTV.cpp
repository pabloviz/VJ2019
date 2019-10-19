#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "PlayerTV.h"
#include "Game.h"
#include "Bullet.h"


#define FALL_STEP 4
#define MAX_BULLETS 4

#define TILESHEET_H 0.0625
#define TILESHEET_V 0.09375


enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, STAND_RIGHT_UP, STAND_LEFT_UP, MOVE_LEFT, MOVE_RIGHT, MOVE_LEFT_UP, MOVE_RIGHT_UP,
	MOVE_LEFT_DOWN, MOVE_RIGHT_DOWN, JUMP_LEFT, JUMP_RIGHT, DIE_LEFT, DIE_RIGHT,
	SWIM_LEFT, SWIM_RIGHT, CROUCH_LEFT, CROUCH_RIGHT
};


void PlayerTV::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Scene *scene)
{
	bJumping = false;
	x_pressed = false;
	z_pressed = false;
	this->scene = scene;
	this->dying = false;
	this->invulnerable = true;
	spread = false;

	spritesheet.loadFromFile("images/contraspritesheet.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(32, 48), glm::vec2(TILESHEET_H, TILESHEET_V), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(18);

	sprite->setAnimationSpeed(STAND_LEFT, 8);
	sprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.f));

	sprite->setAnimationSpeed(STAND_RIGHT, 8);
	sprite->addKeyframe(STAND_RIGHT, glm::vec2(2 * TILESHEET_H, 0.f));

	sprite->setAnimationSpeed(STAND_LEFT_UP, 8);
	sprite->addKeyframe(STAND_LEFT_UP, glm::vec2(4 * TILESHEET_H, 0.f));

	sprite->setAnimationSpeed(STAND_RIGHT_UP, 8);
	sprite->addKeyframe(STAND_RIGHT_UP, glm::vec2(6 * TILESHEET_H, 0.f));

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

	sprite->setAnimationSpeed(DIE_RIGHT, 5);
	sprite->addKeyframe(DIE_RIGHT, glm::vec2(4 * TILESHEET_H, 5 * TILESHEET_V));
	sprite->addKeyframe(DIE_RIGHT, glm::vec2(3 * TILESHEET_H, 5 * TILESHEET_V));
	sprite->addKeyframe(DIE_RIGHT, glm::vec2(2 * TILESHEET_H, 5 * TILESHEET_V));
	sprite->addKeyframe(DIE_RIGHT, glm::vec2(TILESHEET_H, 5 * TILESHEET_V));
	sprite->addKeyframe(DIE_RIGHT, glm::vec2(0.f, 5 * TILESHEET_V));

	sprite->setAnimationSpeed(DIE_LEFT, 5);
	sprite->addKeyframe(DIE_LEFT, glm::vec2(0.f, 6 * TILESHEET_V));
	sprite->addKeyframe(DIE_LEFT, glm::vec2(TILESHEET_H, 6 * TILESHEET_V));
	sprite->addKeyframe(DIE_LEFT, glm::vec2(2 * TILESHEET_H, 6 * TILESHEET_V));
	sprite->addKeyframe(DIE_LEFT, glm::vec2(3 * TILESHEET_H, 6 * TILESHEET_V));
	sprite->addKeyframe(DIE_LEFT, glm::vec2(4 * TILESHEET_H, 6 * TILESHEET_V));

	sprite->setAnimationSpeed(SWIM_LEFT, 8);
	sprite->addKeyframe(SWIM_LEFT, glm::vec2(0.f, 7 * TILESHEET_V));

	sprite->setAnimationSpeed(SWIM_RIGHT, 8);
	sprite->addKeyframe(SWIM_RIGHT, glm::vec2(TILESHEET_H, 7 * TILESHEET_V));

	sprite->setAnimationSpeed(CROUCH_LEFT, 8);
	sprite->addKeyframe(CROUCH_LEFT, glm::vec2(0.f, 8 * TILESHEET_V));

	sprite->setAnimationSpeed(CROUCH_RIGHT, 8);
	sprite->addKeyframe(CROUCH_RIGHT, glm::vec2(TILESHEET_H, 8 * TILESHEET_V));

	posPlayer.x = 1;
	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

}

void PlayerTV::update(int deltaTime)
{
	sprite->update(deltaTime);
	if (!invulnerable) inv_frames = 0;
	else ++inv_frames;
	if (inv_frames >= 200) invulnerable = false;

	air = false;
	water = false;
	crouch = false;

	if (dying) {
		update_death();
		return;
	}

	//WATER LOGIC
	if (map->detectWater(posPlayer, glm::ivec2(32, 48))) {
		if (Game::instance().getSpecialKey(GLUT_KEY_LEFT)) sprite->changeAnimation(SWIM_LEFT);
		else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) sprite->changeAnimation(SWIM_RIGHT);
		else if (sprite->animation() == SWIM_RIGHT) sprite->changeAnimation(SWIM_RIGHT); //FIX THIS 
		else sprite->changeAnimation(SWIM_LEFT);
		water = true;
	}

	//Crouch + jump, ignores collision and falls down the ground
	if (!water && !bJumping && Game::instance().getSpecialKey(GLUT_KEY_DOWN) && Game::instance().getKey('z')) {
		if (!z_pressed) {
			posPlayer.y += FALL_STEP;
			air = true;
		}
		z_pressed = true;
	}
	else z_pressed = false;

	//JUMPING LOGIC
	if (bJumping)
	{
		if (jumpAngle == 180)
		{
			bJumping = false;
			posPlayer.y = startY;
		}
		else
		{
			posPlayer.y = int(startY - 96 * sin(3.14159f * jumpAngle / 180.f));
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

	//CROUCH LOGIC
	if (!air && !water && Game::instance().getSpecialKey(GLUT_KEY_DOWN) && !Game::instance().getSpecialKey(GLUT_KEY_LEFT)
		&& !Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) {
		if (sprite->animation() == CROUCH_LEFT || sprite->animation() == MOVE_LEFT
			|| sprite->animation() == MOVE_LEFT_UP || sprite->animation() == MOVE_LEFT_DOWN
			|| sprite->animation() == JUMP_LEFT || sprite->animation() == STAND_LEFT)
			sprite->changeAnimation(CROUCH_LEFT);
		else sprite->changeAnimation(CROUCH_RIGHT);
		crouch = true;
	}


	//LEFT IS PUSHED
	if (Game::instance().getSpecialKey(GLUT_KEY_LEFT))
	{

		//IF PLAYER WASN'T ALREADY RUNNING, IT STARTS TO
		if (!crouch && !water && !air && sprite->animation() != MOVE_LEFT && sprite->animation() != MOVE_LEFT_UP
			&& sprite->animation() != MOVE_LEFT_DOWN) {
			sprite->changeAnimation(MOVE_LEFT);
		}

		else if (air && sprite->animation() != JUMP_LEFT) sprite->changeAnimation(JUMP_LEFT);

		//IF PLAYER AIMS DIAGONAL AND PREVIOUSLY WASN'T DOING IT, IT STARTS TO
		if (!crouch && !water && !air && Game::instance().getSpecialKey(GLUT_KEY_UP) && sprite->animation() != MOVE_LEFT_UP)
			sprite->changeAnimation(MOVE_LEFT_UP);
		else if (!crouch && !water && !air && Game::instance().getSpecialKey(GLUT_KEY_DOWN) && sprite->animation() != MOVE_LEFT_DOWN)
			sprite->changeAnimation(MOVE_LEFT_DOWN);

		//CHECK COLLISION
		if (!crouch) {
			posPlayer.x -= 2;
			if (map->collisionMoveLeft(posPlayer, glm::ivec2(32, 32)))
			{
				posPlayer.x += 2;
				if (!crouch && !water && !air) sprite->changeAnimation(STAND_LEFT);
			}
		}
	}

	//RIGHT IS PUSHED
	else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT))
	{

		//IF PLAYER WASN'T ALREADY RUNNING, IT STARTS TO
		if (!crouch && !water && !air && sprite->animation() != MOVE_RIGHT && sprite->animation() != MOVE_RIGHT_UP
			&& sprite->animation() != MOVE_RIGHT_DOWN)
			sprite->changeAnimation(MOVE_RIGHT);

		else if (air && sprite->animation() != JUMP_RIGHT) sprite->changeAnimation(JUMP_RIGHT);

		//IF PLAYER AIMS DIAGONAL AND PREVIOUSLY WASN'T DOING IT, IT STARTS TO
		if (!crouch && !water && !air && Game::instance().getSpecialKey(GLUT_KEY_UP) && sprite->animation() != MOVE_RIGHT_UP)
			sprite->changeAnimation(MOVE_RIGHT_UP);
		else if (!crouch && !water && !air && Game::instance().getSpecialKey(GLUT_KEY_DOWN) && sprite->animation() != MOVE_RIGHT_DOWN)
			sprite->changeAnimation(MOVE_RIGHT_DOWN);

		//CHECK COLLISION
		if (!crouch) {
			posPlayer.x += 2;
			if (map->collisionMoveRight(posPlayer, glm::ivec2(32, 32)))
			{
				posPlayer.x -= 2;
				if (!water && !air) sprite->changeAnimation(STAND_RIGHT);

			}
		}
	}
	else
	{
		if (!crouch && !water && !air && (sprite->animation() == MOVE_LEFT || sprite->animation() == MOVE_LEFT_UP
			|| sprite->animation() == MOVE_LEFT_DOWN || sprite->animation() == JUMP_LEFT || sprite->animation() == CROUCH_LEFT))
			sprite->changeAnimation(STAND_LEFT);
		else if (!crouch && !water && !air && (sprite->animation() == MOVE_RIGHT || sprite->animation() == MOVE_RIGHT_UP
			|| sprite->animation() == MOVE_RIGHT_DOWN || sprite->animation() == JUMP_RIGHT || sprite->animation() == CROUCH_RIGHT))
			sprite->changeAnimation(STAND_RIGHT);
		else if (air && (sprite->animation() == MOVE_LEFT || sprite->animation() == MOVE_LEFT_UP
			|| sprite->animation() == MOVE_LEFT_DOWN || sprite->animation() == STAND_LEFT
			|| sprite->animation() == SWIM_LEFT || sprite->animation() == CROUCH_LEFT))
			sprite->changeAnimation(JUMP_LEFT);
		else if (air && (sprite->animation() == MOVE_RIGHT || sprite->animation() == MOVE_RIGHT_UP
			|| sprite->animation() == MOVE_RIGHT_DOWN || sprite->animation() == STAND_RIGHT
			|| sprite->animation() == SWIM_RIGHT || sprite->animation() == CROUCH_RIGHT))
			sprite->changeAnimation(JUMP_RIGHT);

		if (sprite->animation() == STAND_RIGHT && Game::instance().getSpecialKey(GLUT_KEY_UP)) sprite->changeAnimation(STAND_RIGHT_UP);
		if (sprite->animation() == STAND_LEFT && Game::instance().getSpecialKey(GLUT_KEY_UP)) sprite->changeAnimation(STAND_LEFT_UP);

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

void PlayerTV::update_death() {
	if (sprite->animation() != DIE_LEFT) {
		sprite->changeAnimation(DIE_LEFT);
		bJumping = true;
		jumpAngle = 0;
		startY = posPlayer.y;
	}

	bool dead = false;

	if (bJumping)
	{
		if (jumpAngle == 180)
		{
			bJumping = false;
			posPlayer.y = startY;
		}
		else
		{
			posPlayer.y = int(startY - 96 * sin(3.14159f * jumpAngle / 180.f));
			if (jumpAngle > 90)
				bJumping = !map->collisionMoveDown(posPlayer, glm::ivec2(32, 48), &posPlayer.y);
		}
	}
	else
	{
		posPlayer.y += FALL_STEP;
		if (map->collisionMoveDown(posPlayer, glm::ivec2(32, 48), &posPlayer.y)) {
			--lives;
			if (lives <= 0) {
				//0 vides
				scene->playerDeath();
				dead = true;
			}
			else {
				//queden vides
				scene->playerRespawn();
			}
		}
	}

	if (!dead) {
		posPlayer.x -= 2;
		if (map->collisionMoveLeft(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.x += 2;
		}

		sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	}

}

void PlayerTV::render()
{
	if (inv_frames % 2 == 0) sprite->render();
}

void PlayerTV::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void PlayerTV::setPosition(const glm::vec2 &pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}


void PlayerTV::fireBullet() {


	glm::ivec2 direction; //shooting direction

	bool pressed = false; //This will tell us if the user is pressing a direction
	if (Game::instance().getSpecialKey(GLUT_KEY_DOWN)) {
		direction.y = 3;
		pressed = true;
	}
	else if (Game::instance().getSpecialKey(GLUT_KEY_UP)) {
		direction.y = -3;
		pressed = true;
	}
	if (Game::instance().getSpecialKey(GLUT_KEY_LEFT)) {
		direction.x = -3;
		pressed = true;
	}
	else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) {
		direction.x = 3;
		pressed = true;
	}

	//if they are not pressing any key, they will fire forwards
	if (!pressed) {
		if (sprite->animation() == STAND_LEFT) direction.x = -3;
		else direction.x = 3;
	}

	//if they are crouching, they will fire forwards
	if (crouch) {
		direction.y = 0;
		if (sprite->animation() == CROUCH_LEFT) direction.x = -3;
		else direction.x = 3;
	}

	glm::ivec2 bulletSpawn = posPlayer;
	bulletSpawn.y += 16;
	if (crouch || water) bulletSpawn.y += 16;
	(*scene).addBullet(direction, bulletSpawn, true);
}


glm::ivec2 PlayerTV::getPosPlayer() {
	return posPlayer;
}

bool PlayerTV::getSpread() {
	return spread;
}

bool PlayerTV::getInvulnerable() {
	return invulnerable;
}

void PlayerTV::setSpread(bool spread) {
	this->spread = spread;
}

void PlayerTV::die() {
	this->dying = true;
}

void PlayerTV::setLives(int lives) {
	this->lives = lives;
}

bool PlayerTV::getCrouch() {
	return crouch;
}

bool PlayerTV::getWater() {
	return water;
}