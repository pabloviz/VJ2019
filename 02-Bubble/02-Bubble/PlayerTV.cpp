#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "PlayerTV.h"
#include "Game.h"
#include "Bullet.h"


#define FALL_STEP 4
#define MAX_BULLETS 4

#define TILESHEET_H 0.5
#define TILESHEET_V 0.5


enum PlayerAnims
{
	STAND, WALK, DEATH
};


void PlayerTV::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Scene *scene, float angle)
{
	bJumping = false;
	x_pressed = false;
	z_pressed = false;
	this->scene = scene;
	this->dying = false;
	this->invulnerable = true;
	this->angle = angle;
	spread = false;

	spritesheet.loadFromFile("images/playerTV.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(32, 32), glm::vec2(TILESHEET_H, TILESHEET_V), &spritesheet, &shaderProgram, scene);
	sprite->setNumberAnimations(3);

	sprite->setAnimationSpeed(STAND, 8);
	sprite->addKeyframe(STAND, glm::vec2(TILESHEET_H, 0.f));

	sprite->setAnimationSpeed(WALK, 8);
	sprite->addKeyframe(WALK, glm::vec2(0.f, 0.f));
	sprite->addKeyframe(WALK, glm::vec2(TILESHEET_H, 0.f));
	sprite->addKeyframe(WALK, glm::vec2(0.f, TILESHEET_V));
	sprite->addKeyframe(WALK, glm::vec2(TILESHEET_H, 0.f));


	sprite->setAnimationSpeed(DEATH, 8);
	sprite->addKeyframe(DEATH, glm::vec2(TILESHEET_H, TILESHEET_V));

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
	if (inv_frames >= 200000) invulnerable = false;

	air = false;
	water = false;
	crouch = false;

	if (dying) {
		update_death();
		return;
	}

	//CAMERA IS TILTED
	if (Game::instance().getKey('s'))
	{
		//sprite->incrementAngle(+0.1f);
		angle += 0.1f;
	}

	//CAMERA IS TILTED
	else if (Game::instance().getKey('a'))
	{
		//sprite->incrementAngle(-0.1f);
		angle -= 0.1f;
	}
	
	bool walking = false;
	glm::vec2 posPlayerAux = posPlayer;
	//UP IS PUSHED
	if (Game::instance().getSpecialKey(GLUT_KEY_UP))
	{
		walking = true;
		posPlayerAux.x -= cos(angle)*1.25f;
		posPlayerAux.y += sin(angle)*1.25f;
	}

	//DOWN IS PUSHED
	else if (Game::instance().getSpecialKey(GLUT_KEY_DOWN))
	{
		walking = true;

		posPlayerAux.x += cos(angle)*1.25f;
		posPlayerAux.y -= sin(angle)*1.25f;
	}

	//DOWN IS PUSHED
	 if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT))
	{
		 walking = true;

		posPlayerAux.x += cos(angle+(3.14/2))*1.25f;
		posPlayerAux.y -= sin(angle+(3.14/2))*1.25f;
	}

	//DOWN IS PUSHED
	else if (Game::instance().getSpecialKey(GLUT_KEY_LEFT))
	{
		 walking = true;

		posPlayerAux.x += cos(angle-(3.14/2))*1.25f;
		posPlayerAux.y -= sin(angle-(3.14/2))*1.25f;
	}

	glm::vec2 size;
	size.x = 32;
	size.y = 32;
	if (posPlayerAux.x < posPlayer.x) {
		if (map->collisionMoveLeft(posPlayerAux, size)) posPlayerAux.x = posPlayer.x;
	}
	else if (posPlayerAux.x > posPlayer.x) {
		if (map->collisionMoveRight(posPlayerAux, size)) posPlayerAux.x = posPlayer.x;
	}

	if (posPlayerAux.y > posPlayer.y) {
		if (map->collisionMoveDown(posPlayerAux, size, &posPlayerAux.y)) posPlayerAux.y = posPlayer.y;
	}

	else if (posPlayerAux.y < posPlayer.y) {
		if (map->collisionMoveUp(posPlayerAux, size, &posPlayerAux.y)) posPlayerAux.y = posPlayer.y;
	}

	posPlayer = posPlayerAux;

	//fire a bullet
	if (Game::instance().getKey('x')) {
		//x_pressed is used to not allow turbo-fire
		if (x_pressed == false) fireBullet();
		x_pressed = true;
	}
	else x_pressed = false;

	if (walking && sprite->animation() != WALK) sprite->changeAnimation(WALK);
	if (!walking) sprite->changeAnimation(STAND);

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void PlayerTV::update_death() {
	if (death_ticks == 0) {
		sprite->changeAnimation(DEATH);
	}
	++death_ticks;
	if (death_ticks >= 100) {
		death_ticks = 0;
		dying = false;
		--lives;
		if (lives <= 0) {
			//0 vides
			scene->playerDeath();

		}
		else {
			//queden vides
			scene->playerRespawn();
		}
	}

}

void PlayerTV::render(glm::vec2 posPlayer, float angle)
{
	if (inv_frames % 2 == 0) sprite->render(posPlayer, 0.f);
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
	float shoot_angle = (3 * 3.14 / 4);
	float hypothenuse = 22.62;
	float diff = angle - (3.14 / 2);
	glm::vec2 bulletSpawn = posPlayer;
	//center
	bulletSpawn.x += 16;
	bulletSpawn.y += 16;
	shoot_angle += diff;
	bulletSpawn.x -= cos(shoot_angle)*hypothenuse;
	bulletSpawn.y += sin(shoot_angle)*hypothenuse;

	scene->addBullet(angle, bulletSpawn, true);
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

float PlayerTV::getAngle() {
	return angle;
}