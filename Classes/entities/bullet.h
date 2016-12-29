#ifndef ENTITIES_BULLET_H_
#define ENTITIES_BULLET_H_

#include "entities/game_object.h"

class Bullet : public GameObject {
public:	
	Bullet(const std::string &filename, eObjectType type)
	{
		m_pSprite = cocos2d::Sprite::create(filename);
		m_eObjectType = type;	
	}

	void Update(float dt)
	{

	}

	void SetState(State state)
	{

	}	
private:
	eObjectType m_eObjectType;
};

#endif