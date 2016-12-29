/*

Copyright (c) 2016 David Morton

*/
#include "entities/land.h"
#include "manager/world_manager.h"

Land::Land(const std::string &filename)
{
	m_pSprite = cocos2d::Sprite::create(filename);
	m_pSprite->setAnchorPoint(cocos2d::Vec2::ZERO);
	WorldManager::getInstance()->GetGameBoard()->addChild(m_pSprite, 0);
	m_eState = ALIVE;
	m_eObjectType = LAND;
	m_bHasTower = false;
}

Land::~Land()
{	
}

void Land::SetState(State state)
{
	m_eState = state;
	if (m_eState == DEAD)
	{
		m_pSprite->setVisible(false);
		m_pSprite->getParent()->removeChild(m_pSprite);
		m_pSprite = nullptr;
	}
}

void Land::TowerOnArea(bool value)
{
	m_bHasTower = value;
}

bool Land::HasTower()
{
	return m_bHasTower;
}
void Land::Update(float dt)
{

}
