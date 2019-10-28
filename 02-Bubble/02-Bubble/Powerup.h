#ifndef _POWERUP_INCLUDE
#define _POWERUP_INCLUDE


#include "TileMap.h"



class Powerup
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Scene *scene, int type);
	void update(int deltaTime);
	void render(glm::vec2 posPlayer, float angle);
	void setTileMap(TileMap *tilemap);
	void setPosition(const glm::vec2 &pos);

	bool farFromPlayer(const glm::vec2 &posPlayer);

	int getType();


	glm::ivec2 getPowerupPos();

private:
	Texture spritesheet;
	Sprite *sprite;
	TileMap *map;
	glm::ivec2 tileMapDispl;
	glm::vec2 posPowerup;
	bool friendly;
	int type;

};

#endif // _POWERUP_INCLUDE


