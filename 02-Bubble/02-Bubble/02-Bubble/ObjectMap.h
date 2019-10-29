#ifndef _OBJECT_MAP_INCLUDE
#define _OBJECT_MAP_INCLUDE


#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"
#include "Sprite.h"
#include "Bridge.h"

class ObjectMap
{

public:

	ObjectMap(const string &levelFile);

	bool loadLevel(const string &levelFile);
	int getEnemyType(int i);
	glm::ivec2 getEnemyPos(int i);
	int getSize();

private:
	glm::ivec2 mapSize;
	vector<int> enemyTypes;
	vector<glm::ivec2> enemyPositions;
	int size, size2;


};


#endif // _OBJECT_MAP_INCLUDE




