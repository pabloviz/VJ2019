#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 32
#define SCREEN_Y 16

#define INIT_PLAYER_X_TILES 4 //4 
#define INIT_PLAYER_Y_TILES 2

#define INIT_PLAYER_TV_X_TILES 5 //4 
#define INIT_PLAYER_TV_Y_TILES 4

#define INIT_BOSS_X_TILES 6 //4 
#define INIT_BOSS_Y_TILES 0


#define MAX_BULLETS 40

#define BULLET_HEIGHT 8
#define BULLET_WIDTH 8

#define ENEMY_HEIGHT 32
#define ENEMY_WIDTH 32

#define PLAYER_HEIGHT 32
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
	currentlevel = level;
	/*
	if (level != 2) TV = false;
	else TV = true;
	*/
	TV = (level == 2);
	initShaders();
	ticks = 0;
	posPlayer.x = 0;
	posPlayer.y = 0;
	win = false;
	angle = 0;
	engine = irrklang::createIrrKlangDevice();
	maxPowerups = 0;

	if (TV) {
		map = TileMap::createTileMap("levels/level02.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram, this);
		obj = new ObjectMap("levels/obj02.txt"); //TODO: change
		maxEnemies = obj->getSize();
		engine->play2D("sound/level2.mp3", true);
	}
	else {
		if (level == 0) {
			menu = new Menu;
			menu->ini(texProgram, this, glm::vec2(SCREEN_X, SCREEN_Y));
		}
		if (level == 1) {
			map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram, this);
			map->iniWater(texProgram, glm::ivec2(SCREEN_X, SCREEN_Y));
			map->iniGate(texProgram, glm::ivec2(211 * 16, 10 * 16));
			obj = new ObjectMap("levels/obj01.txt");
			maxEnemies = obj->getSize();
			engine->play2D("sound/level1.mp3", true);
			maxPowerups = 4;
		}
		else if (level== 3){
			map = TileMap::createTileMap("levels/level03.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram, this);
			map->iniWater(texProgram, glm::ivec2(SCREEN_X, SCREEN_Y));
			map->iniGate(texProgram, glm::ivec2(211 * 16, 10 * 16));
			obj = new ObjectMap("levels/obj03.txt");
			maxEnemies = 3;
			engine->play2D("sound/level3.mp3", true);
			boss = new Boss();
			boss->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, this);
			boss->setPosition(glm::vec2(INIT_BOSS_X_TILES * map->getTileSize(), INIT_BOSS_Y_TILES * map->getTileSize()));
			boss->setTileMap(map);
		}
		else if (level == 4) {
			controls = new Controls;
			controls->ini(texProgram, this, glm::vec2(SCREEN_X, SCREEN_Y), "images/controls.png");
		}
		else if (level == 5) {
			credits = new Controls;
			credits->ini(texProgram, this, glm::vec2(SCREEN_X, SCREEN_Y), "images/credits.png");
		}
	}
	for (int i = 0; i < MAX_BULLETS; ++i) bullets.push_back(NULL);

	for (int i = 0; i < maxEnemies; ++i) {
		enemies.push_back(NULL);
		spawnedEnemies.push_back(false);
	}

	currentTime = 0.0f;
	if (level == 1 || level == 2 || level == 3) {
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
			playertv->setPosition(glm::vec2(INIT_PLAYER_TV_X_TILES * map->getTileSize(), INIT_PLAYER_TV_Y_TILES * map->getTileSize()));
			playertv->setTileMap(map);
			playertv->setLives(PLAYER_LIVES);
		}
	}
	
	//boss = new Boss();
	//boss->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, this);
	//boss->setPosition(glm::vec2(20 * map->getTileSize(), 2 * map->getTileSize()));
	//boss->setTileMap(map);
	if (level == 1 || level == 2 || level == 3) {
		if (level == 1) {
			for (int i = 0; i < maxPowerups; ++i) {
				powerups.push_back(new Powerup());
				powerups[i]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, this, i);
				powerups[i]->setPosition(glm::vec2((15 + 15*i) * map->getTileSize(), 10 * map->getTileSize()));
				powerups[i]->setTileMap(map);
			}
			camouflage_icon = new Medalla(); camouflage_icon->iniMedalla(glm::ivec2(1 + 16*0, 20), texProgram, "images/camouflage.png");
			player_speed_icon = new Medalla(); player_speed_icon->iniMedalla(glm::ivec2(1 + 16 * 1, 20), texProgram, "images/player_speed.png");
			bullet_speed_icon = new Medalla(); bullet_speed_icon->iniMedalla(glm::ivec2(1 + 16 * 2, 20), texProgram, "images/bullet_speed.png");
			camouflage_available = player_speed_available = bullet_speed_available = false;

		}
		for (int i = 0; i < PLAYER_LIVES; ++i) {
			Medalla * medalla = new Medalla();
			medalla->iniMedalla(glm::ivec2(1 + 16 * i, 4), texProgram, "images/medalla.png");
			vides.push_back(medalla);
		}
	}

	//projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	
	camera = new Camera();
	//camera->setCameraPos(player->getPosPlayer());
	camera->setCameraPos(glm::ivec2(SCREEN_X, SCREEN_Y));
	camera->setTV(TV);
	if (level == 3) camera->setScroll(false);
	else camera->setScroll(true);
	projection = camera->calcProj();
	//projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1 + 100), float(SCREEN_HEIGHT - 1), 0.f);
	
}



void Scene::update(int deltaTime)
{
	++ticks;
	currentTime += deltaTime;
	
	if (Game::instance().getKey('c')) {
		if (camouflage_available) {
			player->setCamouflage(true);
			camouflage_available = false;
		}
	}

	if (Game::instance().getKey('v')) {
		if (player_speed_available) {
			player->setSpeed(true);
			player_speed_available = false;
		}
	}

	if (Game::instance().getKey('b')) {
		if (bullet_speed_available) {
			player->setFastBullets(true);
			bullet_speed_available = false;
		}
	}

	if (currentlevel == 0) {
		if (Game::instance().getSpecialKey(101)) menu->changeselected(1);
		if (Game::instance().getSpecialKey(103)) menu->changeselected(2);
		if (Game::instance().getKey(13)){
			if (menu->getselected() == 1) {
				deleteEntities();
				init(1);
			}
			else if (menu->getselected() == -1) {
				menu->setposini(5);
			}

		}
		if (Game::instance().getKey('c')) {
			deleteEntities();
			init(4);
		}
		menu->update(deltaTime);
	}

	else if (currentlevel == 4) {

		if (Game::instance().getKey('b')) {
			deleteEntities();
			init(0);
		}

		controls->update(deltaTime);
	}else if (currentlevel == 5) {
		credits->update(deltaTime);
	}
	else{

		int lives = 0;
		if (player != NULL) lives = player->getLives();
		else if (playertv != NULL) lives = playertv->getLives();
		if (lives == 0) {
			deleteEntities();
			init(0);
		}

		if (TV) {
			if (Game::instance().getKey('s')) {
				angle -= 0.1f;
				render();
			}
			else if (Game::instance().getKey('a')) {
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
		for (int i = 0; i < maxPowerups; ++i) {
			if (powerups[i] != NULL) powerups[i]->update(deltaTime);
		}
		if (boss != NULL) boss->update(deltaTime);

		if (ticks % 2 == 0)
			if (camera != NULL && (player != NULL || playertv != NULL)) {
				camera->update(deltaTime, posPlayer);
				for (int i = 0; i < PLAYER_LIVES; ++i)
					if (vides[i] != NULL) vides[i]->setPos(camera->getCameraPos());
				if (camouflage_icon != NULL) camouflage_icon->setPos(camera->getCameraPos());
				if (player_speed_icon != NULL) player_speed_icon->setPos(camera->getCameraPos());
				if (bullet_speed_icon != NULL) bullet_speed_icon->setPos(camera->getCameraPos());
			}

		if (map != NULL) {
			map->updateWater(deltaTime);
			map->updateBridges(deltaTime, posPlayer);
			if (!TV && player != NULL) {
				map->updateGate(deltaTime);
				if (map->getGatelives() && player->getPosPlayer().x > 209 * 16)  player->setPosition(player->getPosPlayer() - glm::vec2(1, 0));
				else if (!(map->getGatelives()) && player->getPosPlayer().x > 212 * 16) { win = 1; }
			}
		}
		for (int i = 0; i < MAX_BULLETS; ++i) {
			if (bullets[i] != NULL) {
				bullets[i]->update(deltaTime);
			}
		}
		bool enemiesRemain = false;
		for (int i = 0; i < maxEnemies; ++i) {
			if (spawnedEnemies[i] == false && currentlevel != 3 && currentlevel != 0 && currentlevel != 4 && currentlevel != 5) {
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
		if (currentlevel == 3 && boss == NULL) {
			win = true;
			++currentlevel;
		}
		if (!win && !enemiesRemain && TV) win = true; // level 2 win condition, if everyone is dead
		checkEnemyCollisions();
		if (!TV && player != NULL && !player->getInvulnerable()) checkPlayerCollisions();
		if (TV && playertv != NULL && !playertv->getInvulnerable()) checkPlayerCollisions();
		if (win) {
			deleteEntities();
			init(++currentlevel);
		}
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
	if (map != NULL) {
		map->render(posPlayer, angle);
		map->renderWater(posPlayer, angle);
		map->renderBridges(posPlayer, angle);
	}
	
	if (currentlevel == 0) menu->render(posPlayer, angle);
	else if (currentlevel == 4) controls->render(posPlayer, angle);
	else if (currentlevel == 5) credits->render(posPlayer, angle);
	else {
		if (!TV && map->getGatelives())map->renderGate(posPlayer, angle);

		if (boss != NULL) boss->render(posPlayer, angle);
		map->renderWater(posPlayer, angle);
		if (player != NULL) player->render(posPlayer, angle);
		else if (playertv != NULL) playertv->render(posPlayer, angle);

		if (!TV && !map->getGatelives())map->renderGate(posPlayer, angle);

		for (int i = 0; i < maxEnemies; ++i) {
			if (enemies[i] != NULL) enemies[i]->render(posPlayer, angle);
		}

		for (int i = 0; i < MAX_BULLETS; ++i) {
			if (bullets[i] != NULL) bullets[i]->render(posPlayer, angle);
		}
		for (int i = 0; i < maxPowerups; ++i)
			if (powerups[i] != NULL) powerups[i]->render(posPlayer, angle);
		int lives;
		if (TV && playertv != NULL) lives = playertv->getLives();
		else if (!TV && player != NULL) lives = player->getLives();
		else lives = 0;
		for (int i = 0; i < lives; ++i)
			if (vides[i] != NULL) vides[i]->render();
		if (camouflage_icon != NULL && camouflage_available) camouflage_icon->render();
		if (player_speed_icon != NULL && player_speed_available) player_speed_icon->render();
		if (bullet_speed_icon != NULL && bullet_speed_available) bullet_speed_icon->render();
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
				for (int j = 0; j < maxEnemies; ++j) {
					if (enemies[j] != NULL && !enemies[j]->getDying()) {
						enemyPos = enemies[j]->getEnemyPos();
						enemyPos.y += 16;
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
					bossPos.y += 64;
					hit = collides(bulletPos, BULLET_WIDTH, BULLET_HEIGHT,
						bossPos, BOSS_WIDTH, BOSS_HEIGHT);
					if (hit) {
						despawnBullet(i);
						boss->decrementLife();
						engine->play2D("sound/explode.wav");
					}
				}

				hit = collides(bulletPos, BULLET_WIDTH, BULLET_HEIGHT, glm::ivec2(210*16 , 9*16 ), 132*16, 32*16);
				if (hit) {
					despawnBullet(i);
					map->decGate(texProgram, this);
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

				if (!TV && !player->getDying()) {
					engine->play2D("sound/explode.wav");
					player->die();
				}
				else if (TV) {
					if (!playertv->getDying())
						engine->play2D("sound/explode.wav");
					playertv->die();
				}

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
				if (!TV && !player->getDying()) {
					engine->play2D("sound/explode.wav");
					player->die();
				}
				else if (TV) {
					if (!playertv->getDying())
						engine->play2D("sound/explode.wav");
					playertv->die();
				}
			}
		}
	}

	for (int i = 0; i < maxPowerups; ++i) {
		if (powerups[i] != NULL) {
			glm::ivec2 powerupPos = powerups[i]->getPowerupPos();
			hit = collides(powerupPos, POWERUP_WIDTH, POWERUP_HEIGHT,
				posPlayer, PLAYER_WIDTH, PLAYER_HEIGHT);
			if (hit) {
				//NOTA PEL GERARD: EL setCamouflage, setBulletSpeed I TAL NOMES S'HA DE PODER ACTIVAR SI L'USUARI TOCA UNA TECLA ("Q W E", PER EXEMPLE) I EL TE AVAILABLE. LLAVORS ES POSA UNAVAILABLE
				if (powerups[i]->getType() == 0) {
					if (!TV)player->setSpread(true);
				}
				if (powerups[i]->getType() == 1) {
					//if (!TV)player->setFastBullets(true);
					bullet_speed_available = true;
				}
				if (powerups[i]->getType() == 2) {
					if (!TV) {
						//player->setCamouflage(true);
						camouflage_available = true;
					}
				}
				if (powerups[i]->getType() == 3) {
					//if (!TV)player->setSpeed(true);
					player_speed_available = true;
				}
				despawnPowerup(i);
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

void Scene::addBullet (float angle, glm::vec2 posBullet, bool friendly) {
	int stop = 1;
	bool spread = false, cond = false, fast = false;
	if (friendly && !TV) {
		spread = player->getSpread();
		fast = player->getFastBullets();
	}
	else if (friendly) spread = playertv->getSpread();
	if (spread) {
		stop = 5;
		angle -= 2 * 0.436; //25 degrees * 2
	} 

	for (int i = 0; i < MAX_BULLETS && stop != 0; ++i) {
		if (bullets[i] == NULL) {
			bullets[i] = new Bullet();
			bullets[i]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, angle, posBullet, friendly, this, fast);
			angle += 0.436;
			--stop;
			if (friendly && stop == 0) engine->play2D("sound/fire.wav");
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

void Scene::despawnPowerup(int i) {

	delete powerups[i];
	powerups[i] = NULL;
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
	if (player != NULL) posPlayer = player->getPosPlayer();
	else posPlayer = playertv->getPosPlayer();

	if (!TV) {
		posPlayer.y = 0;
		player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, this);
	}

	if (TV) playertv->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, this, playertv->getAngle());
	if (player != NULL)
	player->setPosition(posPlayer);
	else playertv->setPosition(posPlayer);

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
	for (int i = maxPowerups - 1; i >= 0; --i) {
		if (powerups[i] != NULL) delete powerups[i];
		powerups[i] = NULL;
		powerups.pop_back();
	}
	map = NULL;
	player = NULL;
	playertv = NULL;
	boss = NULL;
	delete engine;
	engine = NULL;
	delete camera;
	camera = NULL;
}

bool Scene::getTV() {
	return TV;
}

int Scene::getLevel() {
	return currentlevel;
}
