#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"
#include "Bullet.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Player
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Scene *scene);
	void update(int deltaTime);
	void render(glm::vec2 posPlayer, float angle);

	void setTileMap(TileMap *tileMap);
	void setPosition(const glm::vec2 &pos);
	void fireBullet();

	bool getCrouch();
	bool getWater();

	bool getSpread();
	void setSpread(bool spread);

	bool getInvulnerable();

	void die();
	void update_death();

	void setLives(int lives);
	int getLives();

	glm::vec2 getPosPlayer();

private:
	bool bJumping;
	bool x_pressed, z_pressed;
	bool spread;
	bool dying;
	bool invulnerable;
	bool air, water, crouch;
	glm::ivec2 tileMapDispl;
	glm::vec2 posPlayer;
	int jumpAngle;
	int inv_frames;
	float startY;

	int lives;
	int deathTicks;
	Texture spritesheet;
	Sprite *sprite;
	TileMap *map;
	Scene *scene;
};


#endif // _PLAYER_INCLUDE


