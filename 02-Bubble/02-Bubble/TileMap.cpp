#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "TileMap.h"


using namespace std;


TileMap *TileMap::createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program, Scene *scene)
{
	TileMap *map = new TileMap(levelFile, minCoords, program, scene);

	return map;
}


TileMap::TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program, Scene *scene)
{
	this->scene = scene;
	loadLevel(levelFile);
	prepareArrays(minCoords, program);
}

TileMap::~TileMap()
{
	if (map != NULL)
		delete map;
}


void TileMap::render(glm::vec2 posPlayer, float angle) const
{
	glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * mapSize.x * mapSize.y);
	glDisable(GL_TEXTURE_2D);
}

void TileMap::free()
{
	glDeleteBuffers(1, &vbo);
}

bool TileMap::loadLevel(const string &levelFile)
{
	ifstream fin;
	string line, tilesheetFile;
	stringstream sstream;
	char tile;
	char tile1; //IMPORTANT PABLO

	fin.open(levelFile.c_str());
	if (!fin.is_open())
		return false;
	getline(fin, line);
	if (line.compare(0, 7, "TILEMAP") != 0)
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> mapSize.x >> mapSize.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> tileSize >> blockSize;
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetFile;
	bool b = tilesheet.loadFromFile(tilesheetFile, TEXTURE_PIXEL_FORMAT_RGBA);
	tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
	tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	tilesheet.setMinFilter(GL_NEAREST);
	tilesheet.setMagFilter(GL_NEAREST);
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetSize.x >> tilesheetSize.y;
	tileTexSize = glm::vec2(1.f / tilesheetSize.x, 1.f / tilesheetSize.y);

	map = new int[mapSize.x * mapSize.y];
	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{

			fin.get(tile);
			int num = 0;
			while (tile != ',' && tile != '\n') {
				num = num * 10 + tile - (int('0'));
				fin.get(tile);
			}
			map[j*mapSize.x + i] = num;

			//map[j * mapSize.x + i] = 0;
		}
		fin.get(tile);
#ifndef _WIN32
		fin.get(tile);
#endif
	}
	fin.close();

	return true;
}

void TileMap::prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program)
{
	int tile, nTiles = 0;
	glm::vec2 posTile, texCoordTile[2], halfTexel;
	vector<float> vertices;

	halfTexel = glm::vec2(0.5f / tilesheet.width(), 0.5f / tilesheet.height());
	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			tile = map[j * mapSize.x + i];

			if (tile == 96) { //inici de pont
				vector<Bridge*> newbridge;
				int delay = 40;
				for (int b = 0; b < 8; ++b) {
					Bridge* br = new Bridge();
					br->init(program, tile + (b % 2) + 1, tilesheetSize.x, tilesheetSize.y, delay, b, scene);
					br->setPos(glm::vec2(minCoords.x + (i + b) * 16, minCoords.y + j * 16));
					br->setMapPos(j * mapSize.x + i + b);
					map[j * mapSize.x + i + b] = 94; //just for colisions
					//map[(j + 1) * mapSize.x + i + b] = 114;
					newbridge.push_back(br);
					if (b % 2) delay += 40;
				}
				map[j * mapSize.x + i + 7] = 112;
				//map[(j + 1) * mapSize.x + i] = 113;
				//map[(j + 1) * mapSize.x + i + 7] = 115;
				bridges.push_back(newbridge);

			}
			if (tile < 4 || tile > 13) {
				// Non-empty tile
				nTiles++;
				posTile = glm::vec2(minCoords.x + i * tileSize, minCoords.y + j * tileSize);
				//texCoordTile[0] = glm::vec2(float((tile-1)%2) / tilesheetSize.x, float((tile-1)/2) / tilesheetSize.y);
				//texCoordTile[0] = glm::vec2(float((tile-1) % 16) / tilesheetSize.x, float((tile-1) / 16) / tilesheetSize.y);
				texCoordTile[0] = glm::vec2(float((tile) % 16) / tilesheetSize.x, float((tile) / 16) / tilesheetSize.y);
				texCoordTile[1] = texCoordTile[0] + tileTexSize;
				//texCoordTile[0] += halfTexel;
				texCoordTile[1] -= halfTexel;
				// First triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				// Second triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				vertices.push_back(posTile.x); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);
			}
		}
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * nTiles * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void *)(2 * sizeof(float)));
}

void TileMap::iniWater(ShaderProgram& shaderProgram, const glm::ivec2& tileMapPos) {
	//bool b = tilesheet.loadFromFile("images/tilesheet.png", TEXTURE_PIXEL_FORMAT_RGBA);
	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			int tile = map[j * mapSize.x + i];
			if (tile >= 4 && tile <= 13) {
				Sprite * sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.0625f, 0.0625f), &tilesheet, &shaderProgram, scene);
				sprite->setNumberAnimations(1);
				sprite->setAnimationSpeed(0, 2);
				sprite->addKeyframe(0, glm::vec2(float((tile) % 16) / tilesheetSize.x, float((tile) / 16) / tilesheetSize.y));
				sprite->addKeyframe(0, glm::vec2(float((tile) % 16) / tilesheetSize.x, float((tile + 16) / 16) / tilesheetSize.y));
				sprite->changeAnimation(0);

				sprite->setPosition(glm::vec2(tileMapPos.x + i * 16, tileMapPos.y + j * 16));
				water.push_back(sprite);
			}
		}
	}
}
void TileMap::updateWater(int deltaTime) {
	for (int i = 0; i < water.size(); ++i) {
		water[i]->update(deltaTime);
	}
}

void TileMap::renderWater(glm::vec2 posPlayer, float angle) {
	for (int i = 0; i < water.size(); ++i) {
		water[i]->render(posPlayer, angle);
	}
}

void TileMap::updateBridges(int deltaTime, glm::ivec2 posplayer) {
	for (int i = 0; i < bridges.size(); ++i) {

		if (bridges[i][0]->getPos().x < posplayer.x + 32 && bridges[i][0]->getDestroy() == false)
			bridges[i][0]->setDestroy(1);

		if (bridges[i][0]->getDestroy()) {
			for (int j = 0; j < bridges[i].size(); j++) {
				bridges[i][j]->setDestroy((j % 2) + 1);
			}
		}

		for (int j = 0; j < bridges[i].size(); ++j) {
			bridges[i][j]->updateBridge(deltaTime);
			if (bridges[i][j]->getDelay() <= 0) {
				map[bridges[i][j]->getMapPos()] = 78; //no colision78
			}
		}


	}
}

void TileMap::renderBridges(glm::vec2 posPlayer, float angle) {
	for (int i = 0; i < bridges.size(); ++i) {
		for (int j = bridges[i].size() - 1; j >= 0; --j) {
			if (j != 0 || !bridges[i][j]->getDestroy()) bridges[i][j]->renderBridge(posPlayer, angle);
		}
	}
}

// Collision tests for axis aligned bounding boxes.
// Method collisionMoveDown also corrects Y coordinate if the box is
// already intersecting a tile below.

bool TileMap::collisionMoveLeft(const glm::vec2 &pos, const glm::ivec2 &size) const //changed
{
	int x, y0, y1;

	x = pos.x / tileSize;
	y0 = (pos.y - size.y) / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	//y1 /= 2.0;
	for (int y = y0; y <= y1; y++)
	{
		int col = map[y * mapSize.x + x];
		if (col < 2 || col == 97 || col == 98)
			return true;
	}

	return false;
}

bool TileMap::collisionMoveRight(const glm::vec2 &pos, const glm::ivec2 &size) const //changed
{
	int x, y0, y1;

	x = (pos.x + size.x - 1) / tileSize;
	y0 = (pos.y - size.y) / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	//y1 /= 2.0;
	for (int y = y0; y <= y1; y++)
	{
		int col = map[y * mapSize.x + x];
		if (col < 2 || col == 97 || col == 98)
			return true;
	}

	return false;
}

bool TileMap::collisionMoveDown(const glm::vec2 &pos, const glm::ivec2 &size, float *posY) const //changed
{
	int x0, x1, y;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = ((pos.y + size.y - 1) / tileSize);
	for (int x = x0; x <= x1; x++)
	{
		int col = map[y * mapSize.x + x];
		if (col < 2 || col == 97 || col == 98 || col == 94 || y == 13) { // y==13 map edge
			{
				if (*posY - tileSize * y + size.y <= 4)
				{
					*posY = tileSize * y - size.y;
					return true;
				}
			}
		}
	}

	return false;
}

bool TileMap::detectWater(glm::vec2 &pos, const glm::ivec2 &size) const
{
	int x0, x1, y;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = (pos.y + size.y) / tileSize;
	for (int x = x0; x <= x1; x++)
	{
		if (map[y*mapSize.x + x] == 14)
		{
			return true;
		}
	}

	return false;
}