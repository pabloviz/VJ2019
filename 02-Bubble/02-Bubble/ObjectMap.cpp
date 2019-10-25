#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "ObjectMap.h"


using namespace std;


ObjectMap::ObjectMap(const string &levelFile)
{
	loadLevel(levelFile);
}

bool ObjectMap::loadLevel(const string &levelFile)
{
	ifstream fin;
	string line, tilesheetFile;
	stringstream sstream;
	fin.open(levelFile.c_str());
	if (!fin.is_open())
		return false;
	getline(fin, line);
	if (line.compare(0, 9, "OBJECTMAP") != 0)
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> size;
	
	
	enemyTypes = vector<int>(size);
	enemyPositions = vector<glm::ivec2>(size);

	for (int i = 0; i < size; i++)
	{
		sstream.clear();
		getline(fin, line);
		sstream.str(line);

		
		sstream >> enemyTypes[i] >> enemyPositions[i].x >> enemyPositions[i].y;
	}
	fin.close();

	return true;
}

glm::ivec2 ObjectMap::getEnemyPos(int i) {
	return enemyPositions[i];
}

int ObjectMap::getEnemyType(int i) {
	return enemyTypes[i];
}

int ObjectMap::getSize() {
	return size;
}