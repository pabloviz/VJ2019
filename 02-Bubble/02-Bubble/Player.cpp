#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"
#include "Bullet.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 70
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


void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Scene *scene)
{
	bJumping = false;
	x_pressed = false;
	z_pressed = false;
	this->scene = scene;
	this->dying = false;
	this->invulnerable = true;
	spread = false;

	spritesheet.loadFromFile("images/contraspritesheet.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(32, 48), glm::vec2(TILESHEET_H, TILESHEET_V), &spritesheet, &shaderProgram, scene);
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

void Player::update(int deltaTime) //changed
{
	sprite->update(deltaTime);
	if (!invulnerable) inv_frames = 0;
	else ++inv_frames;
	if (inv_frames >= 200) invulnerable = false;

	air = false;
	water = false;
	crouch = false;

	if (posPlayer.y + 48 >= 14 * 16) dying = true;

	if (dying) {
		update_death();
		return;
	}

	//WATER LOGIC
	if (map->detectWater(posPlayer, glm::ivec2(32, 48))) {
		if(Game::instance().getSpecialKey(GLUT_KEY_LEFT)) sprite->changeAnimation(SWIM_LEFT);
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
		jumpAngle += JUMP_ANGLE_STEP;
		if (jumpAngle >= 180)
		{
			bJumping = false;
			posPlayer.y = startY;
		}
		else
		{
			posPlayer.y = startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f);
			if (jumpAngle > 90)
				bJumping = !map->collisionMoveDown(posPlayer, glm::ivec2(32, 48), &posPlayer.y, true);
		}
		air = true;
	}
	else
	{
		posPlayer.y += FALL_STEP;
		if (map->collisionMoveDown(posPlayer, glm::ivec2(32, 48), &posPlayer.y, true))
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
		glm::vec2 posAux = posPlayer;
		posAux.y += 32;
		if (!crouch && !(water && map->collisionMoveLeft(posAux, glm::ivec2(32, 16), true))) {
			posPlayer.x -= 0.75;
			
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
		glm::ivec2 posAux = posPlayer;
		posAux.y += 32;
		if (!crouch && !(water && map->collisionMoveRight(posAux, glm::ivec2(32, 16),true))) {
			posPlayer.x += 0.75;
			
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

void Player::update_death() { //changed

	if (sprite->animation() != DIE_LEFT) {
		sprite->changeAnimation(DIE_LEFT);
		bJumping = true;
		jumpAngle = 0;
		startY = posPlayer.y;
	}

	bool dead = false;

	if (bJumping)
	{
		jumpAngle += JUMP_ANGLE_STEP;
		if (jumpAngle == 180)
		{
			bJumping = false;
			posPlayer.y = startY;
		}
		else
		{
			posPlayer.y = startY - 96 * sin(3.14159f * jumpAngle / 180.f);
			if (jumpAngle > 90)
				bJumping = !map->collisionMoveDown(posPlayer, glm::ivec2(32, 48), &posPlayer.y, true);
		}
	}
	else
	{
		posPlayer.y += FALL_STEP;
		if (map->collisionMoveDown(posPlayer, glm::ivec2(32, 48), &posPlayer.y, true)) {
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
		if (map->collisionMoveLeft(posPlayer, glm::ivec2(32, 32), true))
		{
			posPlayer.x += 2;
		}

		sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	}

}

void Player::render(glm::vec2 posPlayer, float angle)
{
	if (inv_frames % 2 == 0) sprite->render(posPlayer, angle);
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


	glm::vec2 direction; //shooting direction
	float angle;
	bool pressed = false; //This will tell us if the user is pressing a direction
	bool reverse = false;
	if (Game::instance().getSpecialKey(GLUT_KEY_DOWN)) {
		direction.y = 3;
		pressed = true;
		if (!crouch) reverse = true;
	}
	else if (Game::instance().getSpecialKey(GLUT_KEY_UP)) {
		direction.y = -3;
		pressed = true;
		reverse = true;
	}
	if (Game::instance().getSpecialKey(GLUT_KEY_LEFT)) {
		direction.x = -3;
		pressed = true;
		reverse = false;
	}
	else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) {
		direction.x = 3;
		pressed = true;
		reverse = false;
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
	
	if (sprite->animation() == STAND_RIGHT || sprite->animation() == MOVE_RIGHT || sprite->animation() == MOVE_RIGHT_DOWN ||
		sprite->animation() == MOVE_RIGHT_UP || sprite->animation() == JUMP_RIGHT || sprite->animation() == CROUCH_RIGHT ||
		sprite->animation() == SWIM_RIGHT)
		reverse = true;

	angle = -atan(direction.y / direction.x) + (reverse * 3.14f);
	glm::ivec2 bulletSpawn = posPlayer;
	bulletSpawn.y += 16;
	if (crouch || water) bulletSpawn.y += 16;
	scene->addBullet(angle, bulletSpawn, true);
}


glm::vec2 Player::getPosPlayer() {
	return posPlayer;
}

bool Player::getSpread() {
	return spread;
}

bool Player::getInvulnerable() {
	return invulnerable;
}

void Player::setSpread(bool spread) {
	this->spread = spread;
}

void Player::die() {
	this->dying = true;
}

void Player::setLives(int lives) {
	this->lives = lives;
}

int Player::getLives() {
	return lives;
}

bool Player::getCrouch() {
	return crouch;
}

bool Player::getWater() {
	return water;
}