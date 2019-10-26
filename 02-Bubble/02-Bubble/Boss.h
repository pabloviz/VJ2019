#ifndef _BOSS_INCLUDE
#define _BOSS_INCLUDE


#include "Sprite.h"
#include "TileMap.h"
#include "Bullet.h"
#include "Player.h"


class Boss
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Scene *scene);
	void update(int deltaTime);
	void render(glm::vec2 posPlayer, float angle);

	void setTileMap(TileMap *tileMap);
	void setPosition(const glm::vec2 &pos);

	void fireBullet();
	void setDying(bool dying);
	void updateDeath();

	bool getDying();
	void decrementLife();

	glm::ivec2 getBossPos();

private:
	vector<Bullet *> bullets;
	glm::ivec2 tileMapDispl, posBoss;
	Texture spritesheet;
	Sprite *sprite;
	TileMap *map;
	ShaderProgram playerShaderProgram;
	Scene *scene;
	int ticks, deathTicks;
	bool dying;
	int life;
};


#endif // _BOSS_INCLUDE


