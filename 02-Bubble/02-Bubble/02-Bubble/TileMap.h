#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"
#include "Sprite.h"
#include "Bridge.h"

// Class Tilemap is capable of loading a tile map from a text file in a very
// simple format (see level01.txt for an example). With this information
// it builds a single VBO that contains all tiles. As a result the render
// method draws the whole map independently of what is visible.


class TileMap
{

public:
	// Tile maps can only be created inside an OpenGL context
	static TileMap *createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program, Scene *scene);

	TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program, Scene *scene);
	~TileMap();

	void render(glm::vec2 posPlayer, float angle) const;
	void free();
	
	int getTileSize() const { return tileSize; }

	bool collisionMoveLeft(const glm::vec2 &pos, const glm::ivec2 &size, const bool TV) const;
	bool collisionMoveRight(const glm::vec2 &pos, const glm::ivec2 &size, const bool TV) const;
	bool collisionMoveDown(const glm::vec2 &pos, const glm::ivec2 &size, float *posY, const bool TV) const;
	bool TileMap::collisionMoveUp(const glm::vec2 &pos, const glm::ivec2 &size, float *posY, const bool TV) const;
	bool detectWater(glm::vec2 &pos, const glm::ivec2 &size) const;

	
	void iniWater(ShaderProgram& shaderProgram, const glm::ivec2& tileMapPos);
	void renderWater(glm::vec2 posPlayer, float angle);
	void updateWater(int deltaTime);
	void iniBridges(ShaderProgram& shaderProgram, const glm::ivec2& tileMapPos);
	void renderBridges(glm::vec2 posPlayer, float angle);
	void updateBridges(int deltaTime, glm::ivec2 posplayer);
	void iniGate(ShaderProgram& shaderProgram, const glm::ivec2& tileMapPos);
	void renderGate(glm::vec2 posPlayer, float angle);
	void updateGate(int deltaTime);
	void decGate(ShaderProgram& program, Scene* scene);
	bool getGatelives();
	
private:
	bool loadLevel(const string &levelFile);
	void prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program);

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	glm::ivec2 position, mapSize, tilesheetSize;
	int tileSize, blockSize;
	Texture tilesheet;
	glm::vec2 tileTexSize;
	int *map;
	vector<Sprite *> water;
	vector<vector <Bridge *> > bridges;
	Sprite *gate;
	int gate_hp;
	Scene *scene;
	vector<Explosion *> fin_explosions;


};


#endif // _TILE_MAP_INCLUDE


