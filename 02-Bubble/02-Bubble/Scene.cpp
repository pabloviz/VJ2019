#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 32
#define SCREEN_Y 16

#define INIT_PLAYER_X_TILES 2
#define INIT_PLAYER_Y_TILES 5

#define MAX_BULLETS 40
#define MAX_ENEMIES 4

#define BULLET_HEIGHT 8
#define BULLET_WIDTH 8

#define ENEMY_HEIGHT 32
#define ENEMY_WIDTH 32

#define PLAYER_HEIGHT 32
#define PLAYER_WIDTH 48

#define POWERUP_HEIGHT 16
#define POWERUP_WIDTH 32

#define BOSS_HEIGHT 32
#define BOSS_WIDTH 32

#define PLAYER_LIVES 2

enum EnemyType
{
	RUNNING, SHOOTING, TURRET, CHASE
};

Scene::Scene()
{
	map = NULL;
	player = NULL;
}

Scene::~Scene()
{
	if(map != NULL)
		delete map;
	if(player != NULL)
		delete player;
	for (int i = 0; i < enemies.size(); ++i) {
		if (enemies[i] != NULL)
			delete enemies[i];
	}
}


void Scene::init()
{
	initShaders();
	ticks = 0;

	for (int i = 0; i < MAX_BULLETS; ++i) bullets.push_back(NULL);
	for (int i = 0; i < MAX_ENEMIES; ++i) enemies.push_back(NULL);
	map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, this);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);
	player->setLives(PLAYER_LIVES);
	
	enemies[0] = new Enemy();
	enemies[0]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, player, RUNNING, this, 0);
	enemies[0]->setPosition(glm::vec2(15 * map->getTileSize(), 20 * map->getTileSize()));
	enemies[0]->setTileMap(map);

	boss = new Boss();
	boss->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, player, this);
	boss->setPosition(glm::vec2(20 * map->getTileSize(), 2 * map->getTileSize()));
	boss->setTileMap(map);

	powerup = new Powerup();
	powerup->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	powerup->setPosition(glm::vec2(15 * map->getTileSize(), 10 * map->getTileSize()));
	powerup->setTileMap(map);

	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	currentTime = 0.0f;
}

void Scene::update(int deltaTime)
{
	++ticks;
	currentTime += deltaTime;
	if (player != NULL)player->update(deltaTime);
	if (powerup != NULL) powerup->update(deltaTime);
	if (boss != NULL) boss->update(deltaTime);
	if (ticks % 2 == 0)
	for (int i = 0; i < MAX_BULLETS; ++i) {
		if (bullets[i] != NULL) {
			bullets[i]->update(deltaTime);
			glm::ivec2 bulletPos = bullets[i]->getBulletPos();
			if (bulletPos.x > SCREEN_WIDTH || bulletPos.x < 0 || bulletPos.y > SCREEN_HEIGHT || bulletPos.y < 0) despawnBullet(i);
			if (bulletPos.x > SCREEN_WIDTH || bulletPos.x < 0 || bulletPos.y > SCREEN_HEIGHT || bulletPos.y < 0) despawnBullet(i);
		}
	}
	for (int i = 0; i < MAX_ENEMIES; ++i) {
		if (enemies[i] != NULL) enemies[i]->update(deltaTime);
	}
	checkEnemyCollisions();
	if (player != NULL && !player->getInvulnerable()) checkPlayerCollisions();
}

void Scene::render()
{
	glm::mat4 modelview;

	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
	map->render();
	if (boss != NULL) boss->render();

	if (player != NULL) player->render();
	for (int i = 0; i < MAX_ENEMIES; ++i) {
		if (enemies[i] != NULL) enemies[i]->render();
	}

	for (int i = 0; i < MAX_BULLETS; ++i) {
		if (bullets[i] != NULL) bullets[i]->render();
	}
	if (powerup != NULL) powerup->render();
}

void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if(!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}

void Scene::checkEnemyCollisions() {
	glm::ivec2 bulletPos;
	glm::ivec2 enemyPos;
	bool exists, hit;
	for (int i = 0; i < MAX_BULLETS; ++i) {
		bulletPos = getBulletPos(i, exists);
		if (exists) {
			if (bullets[i]->getFriendly()) {
				for (int j = 0; j < MAX_ENEMIES; ++j) {
					if (enemies[j] != NULL && !enemies[j]->getDying()) {
						enemyPos = enemies[j]->getEnemyPos();
						hit = collides(bulletPos, BULLET_WIDTH, BULLET_HEIGHT,
							enemyPos, ENEMY_WIDTH, ENEMY_HEIGHT);
						if (hit) {
							despawnBullet(i);
							enemies[j]->setDying(true);
						}
					}
				}

				if (boss != NULL && !boss->getDying()) {
					glm::ivec2 bossPos = boss->getBossPos();
					//adjusting the hitbox
					bossPos.x += 16;
					bossPos.y += 32;
					hit = collides(bulletPos, BULLET_WIDTH, BULLET_HEIGHT,
						bossPos, BOSS_WIDTH, BOSS_HEIGHT);
					if (hit) {
						despawnBullet(i);
						boss->decrementLife();
					}
				}
			}
		}
	}
}

void Scene::checkPlayerCollisions() {
	glm::ivec2 bulletPos;
	glm::ivec2 playerPos = player->getPosPlayer();
	//ADJUSTING HITBOXES
	if (player->getCrouch() || player->getWater()) playerPos.y += 32;
	else playerPos.y += 16;

	bool exists, hit;

	for (int i = 0; i < MAX_BULLETS; ++i) {
		bulletPos = getBulletPos(i, exists);
		if (exists) {
			hit = collides(bulletPos, BULLET_WIDTH, BULLET_HEIGHT,
				playerPos, PLAYER_WIDTH, PLAYER_HEIGHT) && !bullets[i]->getFriendly(); //Descubrir en qué esquina de la caja contenedora
			//del jugador se encuentra el punto posPlayer
			if (hit) {
				despawnBullet(i);
				player->die();
			}
		}
	}

	for (int i = 0; i < MAX_ENEMIES; ++i) {
		if (enemies[i] != NULL) {
			glm::ivec2 enemyPos = enemies[i]->getEnemyPos();
			hit = collides(enemyPos, ENEMY_WIDTH, ENEMY_HEIGHT,
				playerPos, PLAYER_WIDTH, PLAYER_HEIGHT);
			if (hit && !enemies[i]->getDying()) {
				player->die();
			}
		}
	}

	if (powerup != NULL) {
		glm::ivec2 powerupPos = powerup->getPowerupPos();
		hit = collides(powerupPos, POWERUP_WIDTH, POWERUP_HEIGHT,
			  playerPos, PLAYER_WIDTH, PLAYER_HEIGHT);
		if (hit) {
			despawnPowerup();
			player->setSpread(true);
		}
	}
}

//Poder afegir-ho a un collisions.h que pugui cridar qui sigui
bool Scene::collides(glm::ivec2 pos1, int width1, int height1, glm::ivec2 pos2, int width2, int height2) {

	if ((pos1.x >= pos2.x && pos1.x <= (pos2.x + width2)) ||
		((pos1.x + width1) >= pos2.x && (pos1.x + width1) <= (pos2.x + width2))) {
		if ((pos1.y >= pos2.y && pos1.y <= (pos2.y + height2)) ||
			((pos1.y + height1) >= pos2.y && (pos1.y + height1) <= (pos2.y + height2))) {
			return true;
		}
	}
	return false;
}

void Scene::addBullet(glm::ivec2 direction, glm::ivec2 posBullet, bool friendly) {
	int stop = 1;
	bool spread = false, cond = false;
	if (friendly) spread = player->getSpread();
	if (spread) {
		stop = 5;
		
		if (direction.x == 0) {
			direction.x -= 2;
			cond = true;
		}
		else direction.y -= 2;
	}

	for (int i = 0; i < MAX_BULLETS && stop != 0; ++i) {
		if (bullets[i] == NULL) {
			bullets[i] = new Bullet();
			bullets[i]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, direction, posBullet, friendly);
			if (!cond)direction.y++;
			if (cond)direction.x++;
			--stop;

		}
	}
}

glm::ivec2 Scene::getBulletPos(int i, bool &exists) {
	glm::ivec2 result;
	if (bullets[i] != NULL) {
		exists = true;
		result = (*bullets[i]).getBulletPos();
	}
	else exists = false;
	return result;
}

void Scene::despawnBullet(int i) {
	delete bullets[i];
	bullets[i] = NULL;
}

void Scene::despawnPowerup() {
	delete powerup;
	powerup = NULL;
}

glm::ivec2 Scene::getPosPlayer() {
	glm::ivec2 aux;
	aux.x = -1; // if this equals -1 afterwards, it means player was NULL
	if (player != NULL) aux = player->getPosPlayer();
	return aux;
}

void Scene::playerDeath() {
	delete player;
	player = NULL;
}

void Scene::enemyDeath(int id) {
	delete enemies[id];
	enemies[id] = NULL;
}

void Scene::bossDeath() {
	delete boss;
	boss = NULL;
}

void Scene::playerRespawn() {
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, this);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));

}

void Scene::spawnEnemy(glm::ivec2 posSpawn, int type) {
	bool stop = false;
	for (int i = 0; i < MAX_ENEMIES && !stop; ++i) {
		if (enemies[i] == NULL) {
			enemies[i] = new Enemy();
			enemies[i]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, player, type, this, i);
			enemies[i]->setPosition(posSpawn);
			enemies[i]->setTileMap(map);
			stop = true;
		}
	}
}