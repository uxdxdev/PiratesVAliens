/*

Copyright (c) 2016 David Morton

*/
#ifndef ENTITIES_LAND_H_
#define ENTITIES_LAND_H_

#include "entities/game_object.h"
#include <iostream>

class Land : public GameObject{
public:
	Land(const std::string &filename);
	~Land();
	void SetState(State state) override;
	bool HasTower();
	void TowerOnArea(bool value);
	void Update(float dt) override;
private:
	bool m_bHasTower;
};

#endif