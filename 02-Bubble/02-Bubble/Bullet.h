#ifndef _BULLET_INCLUDE
#define _BULLET_INCLUDE


#include "Sprite.h"
#include "TileMap.h"

// Bullet is a sprite of a bullet that can harm enemies and/or the player


class Bullet
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, glm::ivec2 &direction, glm::ivec2 &posPlayer, bool friendly);
	void update(int deltaTime);
	void render();
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
	glm::vec2 posBullet, bulletDirection;
	bool friendly;


};


#endif // _BULLET_INCLUDE


