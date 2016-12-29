/*

Copyright (c) 2016 David Morton

*/
#ifndef ENTITIES_TOWER_H
#define ENTITIES_TOWER_H

#include "entities/game_object.h"
#include "entities/enemy.h"
#include "base/observer.h"

class Tower : public GameObject, public Observer, public cocos2d::Object {
public:
	Tower(const std::string &filename, eObjectType type);
	~Tower();
	void Update(float dt) override;
	void SetState(State state) override;
	void FindTarget();
	void OnNotify(std::shared_ptr<GameEvent> tEvent);
	void ShootTimer(float dt);
	void BulletMovementEnd(cocos2d::Sprite *bullet);
	cocos2d::Vec2 GetTargetPosition();
	void Shoot();
	bool LevelUp() override;	
private:
	cocos2d::Sprite *m_pTarget;
	cocos2d::Sprite *m_TowerTop;

	std::vector<std::shared_ptr<Enemy>> *m_vpEnemyList;
	cocos2d::Vec2 m_TargetPosition;
	int m_iShootTime;
	cocos2d::Sprite *m_pLine;
	float m_fViewRadius;
	int m_iCost;
	std::string m_strTowerShotSound;
	unsigned int SoundId;
	int m_iLevelUpCost;
	int m_iFireRate;
};

#endif