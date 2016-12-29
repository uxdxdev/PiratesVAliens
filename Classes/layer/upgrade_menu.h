/*

Copyright (c) 2016 David Morton

*/
#ifndef LAYER_UPGRADE_MENU_H
#define LAYER_UPGRADE_MENU_H	

#include "base/game_defines.h"
#include "entities/game_object.h"

class UpgradeMenuLayer : public cocos2d::Layer
{
public:
	virtual bool init();

	CREATE_FUNC(UpgradeMenuLayer);

	void update(float dt) override;
	void Upgrade();	
	void Sell();	
	void CloseMenu();
	void UpgradeSellObject(cocos2d::Vec2 location, eObjectType objectType, GameObject *gameObject);


private:
	float m_fScaleFactor;
	cocos2d::Menu *m_Menu;
	cocos2d::MenuItemImage *m_UpgradeOption;
	cocos2d::MenuItemImage *m_SellOption;
	cocos2d::MenuItemImage *m_CloseBuildMenu;

	eObjectType m_ObjectType;
	cocos2d::Vec2 m_Location;
	GameObject *m_GameObject;
};

#endif