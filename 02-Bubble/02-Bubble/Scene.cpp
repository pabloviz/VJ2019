#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 32
#define SCREEN_Y 16

#define INIT_PLAYER_X_TILES 4
#define INIT_PLAYER_Y_TILES 25

#define MAX_BULLETS 4
#define MAX_ENEMIES 1

#define BULLET_HEIGHT 8
#define BULLET_WIDTH 8

#define ENEMY_HEIGHT 32
#define ENEMY_WIDTH 32


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
	map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);

	enemies.push_back(NULL);
	enemies[0] = new Enemy();
	enemies[0]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, player);
	enemies[0]->setPosition(glm::vec2(10 * map->getTileSize(), 10 * map->getTileSize()));
	enemies[0]->setTileMap(map);

	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	currentTime = 0.0f;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
	player->update(deltaTime);
	if (enemies[0] != NULL) enemies[0]->update(deltaTime);
	checkEnemyCollisions();
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
	player->render();
	if (enemies[0] != NULL) enemies[0]->render();
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
		bulletPos = player->getBulletPos(i, exists);
		if (exists) {
			for (int j = 0; j < MAX_ENEMIES; ++j) {
				if (enemies[j] != NULL) {
					enemyPos = enemies[j]->getEnemyPos();
					hit = collides(bulletPos, BULLET_WIDTH, BULLET_HEIGHT,
						           enemyPos, ENEMY_WIDTH, ENEMY_HEIGHT);
					if (hit) {
						player->despawnBullet(i);
						//enemies[j]->kill();    to be implemented
						delete enemies[j];
						enemies[j] = NULL;
					}
				}
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