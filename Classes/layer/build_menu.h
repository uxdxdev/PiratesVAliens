/*

Copyright (c) 2016 David Morton

*/
#ifndef LAYER_BUILD_MENU_H
#define LAYER_BUILD_MENU_H

#include "base/game_defines.h"

class BuildMenuLayer : public cocos2d::Layer
{
public:
	virtual bool init();

	CREATE_FUNC(BuildMenuLayer);

	void update(float dt) override;
	void Build(eObjectType type);
	eObjectType GetSelectedOption();
	void CloseMenu();
	void BuildLocation(cocos2d::Vec2 buildLocation, eObjectType baseType);

private:
	float m_fScaleFactor;
	cocos2d::Menu *m_Menu;
	cocos2d::MenuItemImage *m_CannonOption;
	cocos2d::MenuItemImage *m_FireTower;
	cocos2d::MenuItemImage *m_CloseBuildMenu;

	eObjectType m_BuildSelection;
	eObjectType m_BaseType;
	cocos2d::Vec2 m_BuildLocation;

};

#endif