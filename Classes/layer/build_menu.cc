/*

Copyright (c) 2016 David Morton

*/
#include "layer/build_menu.h"
#include "manager/world_manager.h"
#include "entities/tower_platform.h"
#include "audio/include/SimpleAudioEngine.h"

USING_NS_CC;

#define MENU_SIZE 128

bool BuildMenuLayer::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{		
		return false;
	}	

	this->ignoreAnchorPointForPosition(false);

	// Screen dimensions	
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	m_fScaleFactor = WorldManager::getInstance()->GetGameScaleFactor();

	// Background
	auto background = Sprite::create("background/BuildMenuBackground.png");	
	background->setScale(m_fScaleFactor);
	background->setPosition(Vec2(this->getContentSize().width / 2 + origin.x, this->getContentSize().height / 2 + origin.y));
	this->addChild(background, 0);
		
	// Cannon
	m_CannonOption = MenuItemImage::create(
		"menu/cannonOption.png",
		"menu/cannonOption.png",
		CC_CALLBACK_0(BuildMenuLayer::Build, this, CANNON));
	m_CannonOption->setScale(m_fScaleFactor);
	m_CannonOption->setPosition(Vec2(background->getPositionX() - m_CannonOption->getContentSize().width / 2, background->getPositionY() - m_CannonOption->getContentSize().height / 2));
	m_CannonOption->setVisible(true);

	// Tower
	m_FireTower = MenuItemImage::create(
		"menu/towerOption.png",
		"menu/towerOption.png",
		CC_CALLBACK_0(BuildMenuLayer::Build, this, TOWER));
	m_FireTower->setScale(m_fScaleFactor);
	m_FireTower->setPosition(Vec2(background->getPositionX() - m_FireTower->getContentSize().width / 2, background->getPositionY() + m_FireTower->getContentSize().height / 2));
	m_FireTower->setVisible(true);
	
	m_Menu = Menu::create(m_CannonOption, m_FireTower, NULL);
	m_Menu->setPosition(Vec2::ZERO);	
	this->addChild(m_Menu);
	
	m_BuildSelection = NONE; // initially set the build seleciton to none

	auto action = cocos2d::ScaleTo::create(1.0f, 0.0f);
	this->runAction(action);

	return true;
}

void BuildMenuLayer::update(float dt)
{
}

eObjectType BuildMenuLayer::GetSelectedOption()
{
	return m_BuildSelection;
}

void BuildMenuLayer::Build(eObjectType type)
{
	if (type == CANNON)
	{
		m_BuildSelection = type;
		this->setVisible(false);

		if (WorldManager::getInstance()->GetGameModel()->GetCoins() >= CANNON_COST)
		{
			WorldManager::getInstance()->GetGameModel()->SetCoins(WorldManager::getInstance()->GetGameModel()->GetCoins() - CANNON_COST);
			WorldManager::getInstance()->Notify(GameEvent::create(UPDATE_STATS));

			// Platform exists in that position with no tower on it, open build tower menu		
			if (m_BaseType == PLATFORM)
			{
				TowerPlatform *platform = (TowerPlatform*)WorldManager::getInstance()->GetSelectedObjectByType(m_BuildLocation, PLATFORM);
				WorldManager::getInstance()->Notify(GameEvent::create(CANNON_BUILD, WorldManager::getInstance()->GetCenter(platform->GetSprite())));
				platform->TowerOnArea(true);
				CloseMenu();
			}
			else if (m_BaseType == LAND)
			{
				Land *land = (Land*)WorldManager::getInstance()->GetSelectedObjectByType(m_BuildLocation, LAND);
				WorldManager::getInstance()->Notify(GameEvent::create(CANNON_BUILD, WorldManager::getInstance()->GetCenter(land->GetSprite())));
				land->TowerOnArea(true);
				CloseMenu();
			}
		}
		else
		{
			WorldManager::getInstance()->Notify(GameEvent::create(RESOURCES_TOO_LOW));
		}		
	}
	else if (type == TOWER)
	{
		m_BuildSelection = type;
		this->setVisible(false);

		if (WorldManager::getInstance()->GetGameModel()->GetCoins() >= TOWER_COST)
		{
			WorldManager::getInstance()->GetGameModel()->SetCoins(WorldManager::getInstance()->GetGameModel()->GetCoins() - TOWER_COST);
			WorldManager::getInstance()->Notify(GameEvent::create(UPDATE_STATS));

			// Platform exists in that position with no tower on it, open build tower menu		
			if (m_BaseType == PLATFORM)
			{
				TowerPlatform *platform = (TowerPlatform*)WorldManager::getInstance()->GetSelectedObjectByType(m_BuildLocation, PLATFORM);
				WorldManager::getInstance()->Notify(GameEvent::create(TOWER_BUILD, WorldManager::getInstance()->GetCenter(platform->GetSprite())));
				platform->TowerOnArea(true);
				CloseMenu();
			}
			else if (m_BaseType == LAND)
			{
				Land *land = (Land*)WorldManager::getInstance()->GetSelectedObjectByType(m_BuildLocation, LAND);
				WorldManager::getInstance()->Notify(GameEvent::create(TOWER_BUILD, WorldManager::getInstance()->GetCenter(land->GetSprite())));
				land->TowerOnArea(true);
				CloseMenu();
			}
		}
		else
		{
			WorldManager::getInstance()->Notify(GameEvent::create(RESOURCES_TOO_LOW));
		}
	}

}

void BuildMenuLayer::BuildLocation(cocos2d::Vec2 buildLocation, eObjectType baseType)
{
	std::string constructionSound = "audio/MenuOpen" + std::string(MUSIC_EXTENSION);
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(constructionSound.c_str());

	CloseMenu();
	WorldManager::getInstance()->GetGameBoard()->GetUpgradeMenu()->CloseMenu();
	this->setVisible(true);
	auto action = cocos2d::ScaleTo::create(0.1f, 1.0f);
	this->runAction(action);
	this->setPosition(buildLocation);
	m_BuildLocation = buildLocation;
	m_BaseType = baseType;
}

void BuildMenuLayer::CloseMenu()
{		
	this->setVisible(false);
	this->setScale(0.0f);
}