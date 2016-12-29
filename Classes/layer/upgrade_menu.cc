/*

Copyright (c) 2016 David Morton

*/
#include "layer/upgrade_menu.h"
#include "manager/world_manager.h"
#include "entities/tower_platform.h"
#include "audio/include/SimpleAudioEngine.h"

USING_NS_CC;

#define MENU_SIZE 128

bool UpgradeMenuLayer::init()
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
		
	// Upgrade
	m_UpgradeOption = MenuItemImage::create(
		"menu/upgradeOption.png",
		"menu/upgradeOption.png",
		CC_CALLBACK_0(UpgradeMenuLayer::Upgrade, this));
	m_UpgradeOption->setScale(m_fScaleFactor);
	m_UpgradeOption->setPosition(Vec2(background->getPositionX() - m_UpgradeOption->getContentSize().width / 2, background->getPositionY() - m_UpgradeOption->getContentSize().height / 2));
	m_UpgradeOption->setVisible(true);

	// Sell
	m_SellOption = MenuItemImage::create(
		"menu/sellOption.png",
		"menu/sellOption.png",
		CC_CALLBACK_0(UpgradeMenuLayer::Sell, this));
	m_SellOption->setScale(m_fScaleFactor);
	m_SellOption->setPosition(Vec2(background->getPositionX() + m_SellOption->getContentSize().width / 2, background->getPositionY() + m_SellOption->getContentSize().height / 2));
	m_SellOption->setVisible(true);
	
	m_Menu = Menu::create(m_UpgradeOption, m_SellOption, NULL);
	m_Menu->setPosition(Vec2::ZERO);	
	this->addChild(m_Menu);
	
	m_ObjectType = NONE; // initially set the build seleciton to none

	auto action = cocos2d::ScaleTo::create(1.0f, 0.0f);
	this->runAction(action);

	m_GameObject = nullptr;
	return true;
}

void UpgradeMenuLayer::update(float dt)
{
}



void UpgradeMenuLayer::Upgrade()
{
	if (m_GameObject != nullptr)
	{
		if (m_GameObject->LevelUp())
		{
			// Level up successful
		}		
	}
}

void UpgradeMenuLayer::Sell()
{
	if (m_ObjectType == CANNON)
	{
		WorldManager::getInstance()->Notify(GameEvent::create(CANNON_DESTROYED, m_Location));
		WorldManager::getInstance()->GetGameModel()->SetCoins(WorldManager::getInstance()->GetGameModel()->GetCoins() + (CANNON_COST / 2));
		WorldManager::getInstance()->Notify(GameEvent::create(UPDATE_STATS));		
	}
	else if (m_ObjectType == TOWER)
	{
		WorldManager::getInstance()->Notify(GameEvent::create(TOWER_DESTROYED, m_Location));
		WorldManager::getInstance()->GetGameModel()->SetCoins(WorldManager::getInstance()->GetGameModel()->GetCoins() + (TOWER_COST / 2));
		WorldManager::getInstance()->Notify(GameEvent::create(UPDATE_STATS));
	}
	CloseMenu();
}

void UpgradeMenuLayer::UpgradeSellObject(cocos2d::Vec2 location, eObjectType objectType, GameObject *gameObject)
{
	std::string constructionSound = "audio/MenuOpen" + std::string(MUSIC_EXTENSION);
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(constructionSound.c_str());

	CloseMenu();
	WorldManager::getInstance()->GetGameBoard()->GetBuildMenu()->CloseMenu();
	this->setVisible(true);
	auto action = cocos2d::ScaleTo::create(0.1f, 1.0f);
	this->runAction(action);
	this->setPosition(location);
	m_Location = location;
	m_ObjectType = objectType;
	m_GameObject = gameObject;
	if (m_GameObject != nullptr)
	{		
		m_GameObject->SetRadiusVisible(true);
	}
}

void UpgradeMenuLayer::CloseMenu()
{		
	this->setVisible(false);
	this->setScale(0.0f);	
	if (m_GameObject != nullptr)
	{			
		m_GameObject->SetRadiusVisible(false);
	}
	m_GameObject = nullptr;
}
