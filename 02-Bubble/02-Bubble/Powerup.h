#ifndef _POWERUP_INCLUDE
#define _POWERUP_INCLUDE


#include "TileMap.h"



class Powerup
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
	void update(int deltaTime);
	void render();
	void setTileMap(TileMap *tilemap);
	void setPosition(const glm::vec2 &pos);

	bool farFromPlayer(const glm::vec2 &posPlayer);


	glm::ivec2 getPowerupPos();

private:
	Texture spritesheet;
	Sprite *sprite;
	TileMap *map;
	glm::ivec2 tileMapDispl, posPowerup;
	bool friendly;


};

#endif // _POWERUP_INCLUDE


