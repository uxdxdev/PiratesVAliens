/*

Copyright (c) 2016 David Morton

*/
#include "entities/tower_platform.h"
#include "manager/world_manager.h"
#include "audio/include/SimpleAudioEngine.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "SonarCocosHelperCPP/SonarFrameworks.h"
#endif
TowerPlatform::TowerPlatform(const std::string &filename)
{
	m_pSprite = cocos2d::Sprite::create(filename);
	m_pSprite->setAnchorPoint(cocos2d::Vec2::ZERO);
	WorldManager::getInstance()->GetGameBoard()->addChild(m_pSprite, 1);
	m_eState = ALIVE;
	m_eObjectType = PLATFORM;
	m_bHasTower = false;	

	std::string constructionSound = "audio/Construction" + std::string(MUSIC_EXTENSION);	
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(constructionSound.c_str());

	// Google Analytics
#if (GOOGLE_ANALYTICS_ENABLED)
	SonarCocosHelper::GoogleAnalytics::sendEvent("Build", "Platform Built", "Player built a platform", 0);
#endif
}

TowerPlatform::~TowerPlatform()
{	
}

void TowerPlatform::SetState(State state)
{
	m_eState = state;
	if (m_eState == DEAD)
	{
		// Google Analytics
#if (GOOGLE_ANALYTICS_ENABLED)
		SonarCocosHelper::GoogleAnalytics::sendEvent("Removed", "Platform removed", "Player removed a platform", 0);
#endif
		m_pSprite->setVisible(false);
		m_pSprite->getParent()->removeChild(m_pSprite);
		m_pSprite = nullptr;
	}
}

void TowerPlatform::TowerOnArea(bool value)
{
	m_bHasTower = value;
}

bool TowerPlatform::HasTower()
{
	return m_bHasTower;
}

void TowerPlatform::Update(float dt)
{

}
