#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 32
#define SCREEN_Y 16

#define INIT_PLAYER_X_TILES 2
#define INIT_PLAYER_Y_TILES 5

#define MAX_BULLETS 4
#define MAX_ENEMIES 1

#define BULLET_HEIGHT 8
#define BULLET_WIDTH 8

#define ENEMY_HEIGHT 32
#define ENEMY_WIDTH 32

#define PLAYER_HEIGHT 32
#define PLAYER_WIDTH 48

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
	for (int i = 0; i < MAX_BULLETS; ++i) bullets.push_back(NULL);
	map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, this);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);

	enemies.push_back(NULL);
	enemies[0] = new Enemy();
	enemies[0]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, player, TURRET, this);
	enemies[0]->setPosition(glm::vec2(15 * map->getTileSize(), 20 * map->getTileSize()));
	enemies[0]->setTileMap(map);

	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	currentTime = 0.0f;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
	if (player != NULL)player->update(deltaTime);
	if (enemies[0] != NULL) enemies[0]->update(deltaTime);
	for (int i = 0; i < MAX_BULLETS; ++i) {
		if (bullets[i] != NULL) {
			bullets[i]->update(deltaTime);
			glm::ivec2 bulletPos = bullets[i]->getBulletPos();
			if (bulletPos.x > SCREEN_WIDTH || bulletPos.x < 0 || bulletPos.y > SCREEN_HEIGHT || bulletPos.y < 0) despawnBullet(i);
			if (bulletPos.x > SCREEN_WIDTH || bulletPos.x < 0 || bulletPos.y > SCREEN_HEIGHT || bulletPos.y < 0) despawnBullet(i);
		}
	}
	checkEnemyCollisions();
	if (player != NULL) checkPlayerCollisions();
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
	if (player != NULL) player->render();
	if (enemies[0] != NULL) enemies[0]->render();
	for (int i = 0; i < MAX_BULLETS; ++i) {
		if (bullets[i] != NULL) bullets[i]->render();
	}
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
					if (enemies[j] != NULL) {
						enemyPos = enemies[j]->getEnemyPos();
						hit = collides(bulletPos, BULLET_WIDTH, BULLET_HEIGHT,
							enemyPos, ENEMY_WIDTH, ENEMY_HEIGHT);
						if (hit) {
							despawnBullet(i);
							//enemies[j]->kill();    to be implemented
							delete enemies[j];
							enemies[j] = NULL;
						}
					}
				}
			}
		}
	}
}

void Scene::checkPlayerCollisions() {
	glm::ivec2 bulletPos;
	glm::ivec2 playerPos = player->getPosPlayer();
	bool exists, hit;
	for (int i = 0; i < MAX_BULLETS; ++i) {
		bulletPos = getBulletPos(i, exists);
		if (exists) {
			hit = collides(bulletPos, BULLET_WIDTH, BULLET_HEIGHT,
				playerPos, PLAYER_WIDTH, PLAYER_HEIGHT) && !bullets[i]->getFriendly(); //Descubrir en qué esquina de la caja contenedora
			//del jugador se encuentra el punto posPlayer
			if (hit) {
				despawnBullet(i);
				//enemies[j]->kill();    to be implemented
				delete player;
				player = NULL;
			}
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
	bool stop = false;
	for (int i = 0; i < MAX_BULLETS && !stop; ++i) {
		if (bullets[i] == NULL) {
			stop = true;
			bullets[i] = new Bullet();
			bullets[i]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, direction, posBullet, friendly);
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

glm::ivec2 Scene::getPosPlayer() {
	glm::ivec2 aux;
	aux.x = -1; // if this equals -1 afterwards, it means player was NULL
	if (player != NULL) aux = player->getPosPlayer();
	return aux;
}