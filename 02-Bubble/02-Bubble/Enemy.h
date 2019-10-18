#ifndef _ENEMY_INCLUDE
#define _ENEMY_INCLUDE


#include "Sprite.h"
#include "TileMap.h"
#include "Bullet.h"
#include "Player.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.

class Scene;

class Enemy
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Player *player, int type, Scene *scene, int id);
	void update(int deltaTime);
	void render();

	void setTileMap(TileMap *tileMap);
	void setPosition(const glm::vec2 &pos);

	void fireBullet();
	void setDying(bool dying);
	void updateDeath();

	bool getDying();

	glm::ivec2 getEnemyPos();

private:
	vector<Bullet *> bullets;
	bool bJumping;
	glm::ivec2 tileMapDispl, posEnemy;
	int jumpAngle, startY;
	Texture spritesheet;
	Sprite *sprite;
	TileMap *map;
	ShaderProgram playerShaderProgram;
	Player *player;
	int type; //what type of enemy is it?
	Scene *scene;
	int ticks, deathTicks;
	int id;
	bool dying;
};


#endif // _ENEMY_INCLUDE


