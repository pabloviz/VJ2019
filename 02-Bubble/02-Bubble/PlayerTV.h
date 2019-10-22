#ifndef _PLAYERTV_INCLUDE
#define _PLAYERTV_INCLUDE


#include "Sprite.h"
#include "TileMap.h"
#include "Bullet.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.

class Scene;

class PlayerTV
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Scene *scene);
	void update(int deltaTime);
	void render();

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

	glm::ivec2 getPosPlayer();

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
	float startY, angle;
	int inv_frames;
	int lives;
	Texture spritesheet;
	Sprite *sprite;
	TileMap *map;
	Scene *scene;
};


#endif // _PLAYERTV_INCLUDE


