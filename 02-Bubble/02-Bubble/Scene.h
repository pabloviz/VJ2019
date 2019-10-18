#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Powerup.h"
#include "Boss.h"


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
    void playerDeath();
	void playerRespawn();
	void enemyDeath(int id);
	void bossDeath();
	void spawnEnemy(glm::ivec2 posSpawn, int type);

private:
	void initShaders();
	void checkEnemyCollisions();
	void despawnBullet(int i);
	void despawnPowerup();
	void checkPlayerCollisions();
	

	glm::ivec2 getBulletPos(int i, bool &exists);


	bool collides(glm::ivec2 pos1, int width1, int height1, glm::ivec2 pos2, int width2, int height2);


private:
	TileMap *map;
	Player *player;
	Boss *boss;
	vector<Enemy*> enemies;
	vector<Bullet*> bullets;
	Powerup *powerup;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;
	int ticks;

};


#endif // _SCENE_INCLUDE

