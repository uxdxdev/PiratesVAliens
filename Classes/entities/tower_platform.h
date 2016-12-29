/*

Copyright (c) 2016 David Morton

*/
#ifndef ENTITIES_TOWER_PLATFORM_H_
#define ENTITIES_TOWER_PLATFORM_H_

#include "entities/game_object.h"
#include <iostream>

class TowerPlatform : public GameObject{
public:
	TowerPlatform(const std::string &filename);
	void SetState(State state) override;
	virtual ~TowerPlatform();
	bool HasTower();
	void TowerOnArea(bool value);
	void Update(float dt) override;
private:
	bool m_bHasTower;
};

#endif