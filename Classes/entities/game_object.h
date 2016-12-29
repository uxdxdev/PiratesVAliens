/*

Copyright (c) 2016 David Morton

*/
#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include "base/game_defines.h"

class GameObject{
public:	
	GameObject() : m_pSprite(nullptr), m_pRadius(nullptr){}
	virtual cocos2d::Sprite *GetSprite(){ return m_pSprite; };
	virtual cocos2d::Sprite *GetRadiusSprite(){ return m_pRadius; };
	virtual void Update(float dt) = 0;
	virtual State GetState(){ return m_eState; }
	virtual void SetState(State state) = 0;
	virtual void SetRadiusVisible(bool value){ 
		if (m_pRadius != nullptr && m_eState == ALIVE)
		{
			m_pRadius->setVisible(value);
		}	
	}

	virtual eObjectType GetType(){ return m_eObjectType; }
	
	virtual int GetLevel(){ return m_iLevel; }
	virtual void SetLevel(int level){ m_iLevel = level; }

	virtual bool LevelUp(){ return false; }
	
	virtual void SetAttackStrength(int attack){ m_iAttackStrength = attack; }
	virtual int GetAttackStrength(){ return m_iAttackStrength; }

protected:
	cocos2d::Sprite *m_pSprite;
	cocos2d::Sprite *m_pRadius;
	State m_eState;
	eObjectType m_eObjectType;
	int m_iLevel;
	int m_iAttackStrength;
};
#endif
