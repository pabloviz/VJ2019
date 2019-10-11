#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"


// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void init();
	void update(int deltaTime);
	void render();
	void addBullet(glm::ivec2 direction, glm::ivec2 posBullet, bool friendly);
	glm::ivec2 getPosPlayer();


private:
	void initShaders();
	void checkEnemyCollisions();
	void despawnBullet(int i);
	void checkPlayerCollisions();

	glm::ivec2 getBulletPos(int i, bool &exists);


	bool collides(glm::ivec2 pos1, int width1, int height1, glm::ivec2 pos2, int width2, int height2);


private:
	TileMap *map;
	Player *player;
	vector<Enemy*> enemies;
	vector<Bullet*> bullets;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;
	

};


#endif // _SCENE_INCLUDE
