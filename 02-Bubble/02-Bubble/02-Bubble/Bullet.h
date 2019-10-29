#ifndef _BULLET_INCLUDE
#define _BULLET_INCLUDE


#include "Sprite.h"
#include "TileMap.h"

// Bullet is a sprite of a bullet that can harm enemies and/or the player


class Bullet
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, float &angle, glm::vec2 &posPlayer, bool friendly, Scene *scene, bool fast);
	void update(int deltaTime);
	void render(glm::vec2 posPlayer, float angle);
	void setTileMap(TileMap *tilemap);
	void setPosition(const glm::vec2 &pos);

	bool farFromPlayer(const glm::vec2 &posPlayer);
	bool getFriendly();


	glm::ivec2 getBulletPos();

private:
	Texture spritesheet;
	Sprite *sprite;
	TileMap *map;
	glm::ivec2 tileMapDispl;
	glm::vec2 posBullet;
	float angle;
	bool friendly;
	bool fast;

};


#endif // _BULLET_INCLUDE


