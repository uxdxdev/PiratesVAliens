/*

Copyright (c) 2016 David Morton

*/
#ifndef ENEMY_H_
#define ENEMY_H_

#include "entities/game_object.h"

class Enemy : public GameObject, public cocos2d::Object {
public:
	enum eEnemyHealth{
		ENEMY_BLUE_HEALTH = 80,
		ENEMY_RED_HEALTH = 110,
		ENEMY_GREEN_HEALTH = 250,
		ENEMY_YELLOW_HEALTH = 400,
		ENEMY_BOSS_HEALTH = 1000
};

	enum eEnemySpeed{
		ENEMY_BLUE_SPEED = 120,
		ENEMY_RED_SPEED = 100,
		ENEMY_GREEN_SPEED = 80,
		ENEMY_YELLOW_SPEED = 60,
		ENEMY_BOSS_SPEED = 90
	};

	Enemy(const std::string &filename, eEnemyType type);
	~Enemy();
	void MoveToNextNode();
	void Update(float dt) override;
	void NextNodeInPath();	
	void AtDestination();
	cocos2d::Vec2 GetCenter();
	void TakeDamage(int value);
	eEnemyType GetEnemyType();
	void SetState(State state) override;
	void DeathAnimation();
	void VillageAttack();

private:
	cocos2d::Vec2 m_pTargetPosition;
	bool m_bIsAtDestination;
	bool m_bIsAtFinalTarget;
	int m_iNodeIndexInPath;
	int m_iHealth;
	int m_iMaxHealth;
	eEnemyType m_Type;
	float m_fMovementSpeed;
	float m_iDifficultyMultiplier;
	cocos2d::ProgressTimer *m_HealthBar;
};

#endif

