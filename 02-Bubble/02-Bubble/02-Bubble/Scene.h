#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "PlayerTV.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Powerup.h"
#include "Boss.h"
#include "Camera.h"
#include "ObjectMap.h"
#include "Medalla.h"
#include "Menu.h"
#include "Controls.h"
#include <irrKlang.h>
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void init(int level);
	void update(int deltaTime);
	void render();
	void addBullet(float angle, glm::vec2 posBullet, bool friendly);
	glm::ivec2 getPosPlayer();
    void playerDeath();
	void playerRespawn();
	void enemyDeath(int id);
	void bossDeath();
	void spawnEnemy(glm::ivec2 posSpawn, int type);
	float getAngle();
	void deleteEntities();
	bool getTV();
	int getLevel();

private:
	void initShaders();
	void checkEnemyCollisions();
	void despawnBullet(int i);
	void despawnPowerup(int i);
	void checkPlayerCollisions();

	glm::ivec2 getBulletPos(int i, bool &exists);
	bool collides(glm::ivec2 pos1, int width1, int height1, glm::ivec2 pos2, int width2, int height2);

private:
	TileMap *map;
	ObjectMap *obj;
	Player *player;
	PlayerTV *playertv;
	Boss *boss;
	vector<Enemy*> enemies;
	vector<bool> spawnedEnemies;
	vector<Bullet*> bullets;
	vector<Powerup*> powerups;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;
	int ticks;
	Camera *camera;
	int maxEnemies;
	vector<Medalla*> vides;
	float angle;
	glm::vec2 posPlayer;
	int currentlevel;
	bool TV;
	float win;
	irrklang::ISoundEngine* engine;
	Menu * menu;
	Controls * controls;
	Controls * credits;
	Medalla * camouflage_icon;
	Medalla * player_speed_icon;
	Medalla * bullet_speed_icon;
	bool camouflage_available, player_speed_available, bullet_speed_available;


	int maxPowerups;
};


#endif // _SCENE_INCLUDE
