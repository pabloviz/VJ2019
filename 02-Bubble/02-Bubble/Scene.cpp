#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 32
#define SCREEN_Y 16

#define INIT_PLAYER_X_TILES 4 //200
#define INIT_PLAYER_Y_TILES 2
//#define INIT_PLAYER_X_TILES 0
//#define INIT_PLAYER_Y_TILES 0

#define MAX_BULLETS 40

#define BULLET_HEIGHT 8
#define BULLET_WIDTH 8

#define ENEMY_HEIGHT 32
#define ENEMY_WIDTH 32

#define PLAYER_HEIGHT 48
#define PLAYER_WIDTH 32

#define POWERUP_HEIGHT 16
#define POWERUP_WIDTH 32

#define BOSS_HEIGHT 32
#define BOSS_WIDTH 32

#define PLAYER_LIVES 2

#define TILE_SIZE 16


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


void Scene::init(int level) //changed
{
	if (level == 1) TV = false;
	else TV = true;
	initShaders();
	ticks = 0;
	posPlayer.x = 0;
	posPlayer.y = 0;
	win = false;
	angle = 0;
	engine = irrklang::createIrrKlangDevice();
	if (TV) {
		map = TileMap::createTileMap("levels/level02.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram, this);
		obj = new ObjectMap("levels/obj02.txt"); //TODO: change
		maxEnemies = obj->getSize();
		engine->play2D("sound/level2.mp3");
	}
	else {
		map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram, this);
		map->iniWater(texProgram, glm::ivec2(SCREEN_X, SCREEN_Y));
		obj = new ObjectMap("levels/obj01.txt");
		maxEnemies = obj->getSize();
		engine->play2D("sound/level1.mp3");
	}
	for (int i = 0; i < MAX_BULLETS; ++i) bullets.push_back(NULL);

	for (int i = 0; i < maxEnemies; ++i) {
		enemies.push_back(NULL);
		spawnedEnemies.push_back(false);
	}

	currentTime = 0.0f;
	

	if (!TV) {
		player = new Player();
		player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, this);
		player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
		player->setTileMap(map);
		player->setLives(PLAYER_LIVES);
	}

	else {
		playertv = new PlayerTV();
		playertv->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, this, -(3.14f / 2.f));
		playertv->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
		playertv->setTileMap(map);
		playertv->setLives(PLAYER_LIVES);
	}

	//boss = new Boss();
	//boss->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, this);
	//boss->setPosition(glm::vec2(20 * map->getTileSize(), 2 * map->getTileSize()));
	//boss->setTileMap(map);

	powerup = new Powerup();
	powerup->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, this);
	powerup->setPosition(glm::vec2(15 * map->getTileSize(), 10 * map->getTileSize()));
	powerup->setTileMap(map);
	for (int i = 0; i < PLAYER_LIVES; ++i) {
		Medalla * medalla = new Medalla();
		medalla->iniMedalla(glm::ivec2(1 + 16*i,4), texProgram);
		vides.push_back(medalla);
	}

	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	
	camera = new Camera();
	//camera->setCameraPos(player->getPosPlayer());
	camera->setCameraPos(glm::ivec2(SCREEN_X, SCREEN_Y));
	camera->setTV(TV);
	projection = camera->calcProj();
	//projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1 + 100), float(SCREEN_HEIGHT - 1), 0.f);
	
}

void Scene::update(int deltaTime)
{
	++ticks;
	currentTime += deltaTime;
	if (TV) {
		if (Game::instance().getKey('a')) {
			angle -= 0.1f;
			render();
		}
		else if (Game::instance().getKey('s')) {
			angle += 0.1f;
			render();
		}
	}
	if (player != NULL) {
		posPlayer = player->getPosPlayer();
		player->update(deltaTime);
	}
	if (playertv != NULL) {
		posPlayer = playertv->getPosPlayer();
		playertv->update(deltaTime);
	}
	if (powerup != NULL) powerup->update(deltaTime);
	if (boss != NULL) boss->update(deltaTime);

	if (ticks % 2 == 0)
		if (camera != NULL && (player != NULL || playertv != NULL)) {
			camera->update(deltaTime, posPlayer);
			for (int i = 0; i < PLAYER_LIVES; ++i)
				if (vides[i] != NULL) vides[i]->setPos(camera->getCameraPos());
		}

	if (map != NULL) {
		map->updateWater(deltaTime);
		map->updateBridges(deltaTime, posPlayer);
	}
	for (int i = 0; i < MAX_BULLETS; ++i) {
		if (bullets[i] != NULL) {
			bullets[i]->update(deltaTime);
		}
	}
	bool enemiesRemain = false;
	for (int i = 0; i < maxEnemies; ++i) {
		if (spawnedEnemies[i] == false) {
			glm::vec2 posEnemy = obj->getEnemyPos(i);
			posEnemy.x *= TILE_SIZE;
			posEnemy.y *= TILE_SIZE;
			if (posPlayer.x >= posEnemy.x - (TILE_SIZE * 15)) {
				spawnedEnemies[i] = true;
				enemies[i] = new Enemy();
				enemies[i]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, obj->getEnemyType(i), this, i);
				enemies[i]->setPosition(posEnemy);
				enemies[i]->setTileMap(map);
			}
			
		}
		if (enemies[i] != NULL) enemies[i]->update(deltaTime);
		if (!spawnedEnemies[i] || enemies[i] != NULL) {
			enemiesRemain = true;
		}
	}
	if (!win && !enemiesRemain && TV) win = true; // level 2 win condition, if everyone is dead
	checkEnemyCollisions();
	if (!TV && player != NULL && !player->getInvulnerable()) checkPlayerCollisions();
	if (TV && playertv != NULL && !playertv->getInvulnerable()) checkPlayerCollisions();
	if (win) {
		deleteEntities();
		init(1);
	}
}

void Scene::render()
{
	glm::mat4 modelview;

	texProgram.use();
	projection = camera->calcProj();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(posPlayer.x + 48, posPlayer.y + 32, 0.f));
	modelview = glm::rotate(modelview, angle, glm::vec3(0, 0, 1));
	modelview = glm::translate(modelview, glm::vec3(-(posPlayer.x + 48), -(posPlayer.y + 32), 0.f));
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
	
	map->render(posPlayer, angle);
	map->renderWater(posPlayer, angle);
	map->renderBridges(posPlayer, angle);
	if (boss != NULL) boss->render(posPlayer, angle);
	map->renderWater(posPlayer, angle);
	if (player != NULL) player->render(posPlayer, angle);
	else if (playertv != NULL) playertv->render(posPlayer, angle);
	for (int i = 0; i < maxEnemies; ++i) {
		if (enemies[i] != NULL) enemies[i]->render(posPlayer, angle);
	}

	for (int i = 0; i < MAX_BULLETS; ++i) {
		if (bullets[i] != NULL) bullets[i]->render(posPlayer, angle);
	}
	if (powerup != NULL) powerup->render(posPlayer, angle);
	int lives;
	if (TV && playertv != NULL) lives = playertv->getLives();
	else if (!TV && player != NULL) lives = player->getLives();
	else lives = 0;
	for (int i = 0; i < lives; ++i)
		if (vides[i] != NULL) vides[i]->render();
	
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
				for (int j = 0; j < maxEnemies; ++j) {
					if (enemies[j] != NULL && !enemies[j]->getDying()) {
						enemyPos = enemies[j]->getEnemyPos();
						hit = collides(bulletPos, BULLET_WIDTH, BULLET_HEIGHT,
							enemyPos, ENEMY_WIDTH, ENEMY_HEIGHT);
						if (hit) {
							despawnBullet(i);
							enemies[j]->setDying(true);
							engine->play2D("sound/explode.wav");

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
			if (player != NULL && bullets[i] != NULL && bullets[i]->farFromPlayer(posPlayer)) despawnBullet(i);
			if (playertv != NULL && bullets[i] != NULL && bullets[i]->farFromPlayer(posPlayer)) despawnBullet(i);

		}
	}
}

void Scene::checkPlayerCollisions() {
	glm::ivec2 bulletPos;
	//ADJUSTING HITBOXES
	int height = PLAYER_HEIGHT;
	if (!TV) {
		if (player->getCrouch() || player->getWater()) {
			posPlayer.y += 32;
			height -= 32;
		}
		else {
			posPlayer.y += 16;
			height -= 16;
		}
	}

	bool exists, hit;

	for (int i = 0; i < MAX_BULLETS; ++i) {
		bulletPos = getBulletPos(i, exists);
		if (exists) {
			hit = collides(bulletPos, BULLET_WIDTH, BULLET_HEIGHT,
				posPlayer, PLAYER_WIDTH, height) && !bullets[i]->getFriendly();
			if (hit) {
				despawnBullet(i);
				if (!TV)player->die();
				else playertv->die();
			}
			else if (bullets[i]->farFromPlayer(posPlayer)) despawnBullet(i);

		}
	}

	for (int i = 0; i < maxEnemies; ++i) {
		if (enemies[i] != NULL) {
			glm::ivec2 enemyPos = enemies[i]->getEnemyPos();
			hit = collides(enemyPos, ENEMY_WIDTH, ENEMY_HEIGHT,
				posPlayer, PLAYER_WIDTH, PLAYER_HEIGHT);
			if (hit && !enemies[i]->getDying()) {
				if(!TV) player->die();
				else playertv->die();
			}
		}
	}

	if (powerup != NULL) {
		glm::ivec2 powerupPos = powerup->getPowerupPos();
		hit = collides(powerupPos, POWERUP_WIDTH, POWERUP_HEIGHT,
			  posPlayer, PLAYER_WIDTH, PLAYER_HEIGHT);
		if (hit) {
			despawnPowerup();
			if (!TV)player->setSpread(true);
			else playertv->setSpread(true);
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

void Scene::addBullet (float angle, glm::vec2 posBullet, bool friendly) {
	int stop = 1;
	bool spread = false, cond = false;
	if (friendly && !TV) spread = player->getSpread();
	else if (friendly) spread = playertv->getSpread();
	if (spread) {
		stop = 5;
		angle -= 2 * 0.436; //25 degrees * 2
	} 

	for (int i = 0; i < MAX_BULLETS && stop != 0; ++i) {
		if (bullets[i] == NULL) {
			bullets[i] = new Bullet();
			bullets[i]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, angle, posBullet, friendly, this);
			angle += 0.436;
			--stop;
			if (stop == 0) engine->play2D("sound/fire.wav");
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
	engine->play2D("sound/powerup.wav");
}

glm::ivec2 Scene::getPosPlayer() {
	if (player != NULL) posPlayer = player->getPosPlayer();
	else if (playertv != NULL) posPlayer = playertv->getPosPlayer();
	return posPlayer;
}

void Scene::playerDeath() {
	if (!TV) {
		delete player;
		player = NULL;
	}
	else {
		delete playertv;
		playertv = NULL;
	}
	engine->play2D("sound/explode.wav");
}

void Scene::enemyDeath(int id) {
	delete enemies[id];
	enemies[id] = NULL;
}

void Scene::bossDeath() {
	delete boss;
	boss = NULL;
}

void Scene::playerRespawn() { //changed
	glm::vec2 posPlayer = player->getPosPlayer();

	if (!TV) {
		posPlayer.y = 0;
		player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, this);
	}

	if (TV) playertv->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, this, playertv->getAngle());
	player->setPosition(posPlayer);

}

void Scene::spawnEnemy(glm::ivec2 posSpawn, int type) {
	bool stop = false;
	for (int i = 0; i < maxEnemies && !stop; ++i) {
		if (enemies[i] == NULL) {
			enemies[i] = new Enemy();
			enemies[i]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, type, this, i);
			enemies[i]->setPosition(posSpawn);
			enemies[i]->setTileMap(map);
			stop = true;
		}
	}
}

float Scene::getAngle() {
	return angle;
}

void Scene::deleteEntities() {
	if (map != NULL)
		delete map;
	if (player != NULL)
		delete player;
	if (playertv != NULL)
		delete playertv;

	for (int i = maxEnemies-1; i >=0; --i) {
		if (enemies[i] != NULL) {
			delete enemies[i];
			enemies[i] = NULL;
			enemies.pop_back();
		}
		spawnedEnemies.pop_back();
	}
	for (int i = vides.size() - 1; i >= 0; --i) {
		delete vides[i];
		vides[i] = NULL;
		vides.pop_back();
	}
	for (int i = bullets.size() - 1; i >= 0; --i) {
		delete bullets[i];
		bullets[i] = NULL;
		bullets.pop_back();
	}
	if (powerup != NULL) delete powerup;
	map = NULL;
	player = NULL;
	playertv = NULL;
	boss = NULL;
	delete engine;
	engine = NULL;
	powerup = NULL;
	delete camera;
	camera = NULL;
}
