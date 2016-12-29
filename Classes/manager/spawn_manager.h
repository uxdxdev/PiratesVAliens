/*

Copyright (c) 2016 David Morton

*/
#ifndef MANAGER_SPAWN_MANAGER_H
#define MANAGER_SPAWN_MANAGER_H

#include <memory>

#include "base/game_defines.h"
#include "base/observer.h"
#include "base/game_event.h"

class SpawnManager : public Observer{
public:
	SpawnManager();
	void BuildPlatform(cocos2d::Vec2 positiion);
	void SpawnEnemy(cocos2d::Vec2 positiion, eEnemyType type);
	void CreateLand(cocos2d::Vec2 positiion);
	void BuildCannon(cocos2d::Vec2 position);
	void BuildTower(cocos2d::Vec2 position);
	void OnNotify(std::shared_ptr<GameEvent> tEvent);
	int GetNumberOfSpawnedEnemies();
private:
	int m_iEnemiesSpawned;
	int m_iEnemiesDestroyed;
	cocos2d::Size m_VisibleSize;
};

#endif