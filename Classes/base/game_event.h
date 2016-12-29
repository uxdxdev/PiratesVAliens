/*

Copyright (c) 2016 David Morton

*/
#ifndef GAMEEVENT_H_
#define GAMEEVENT_H_

#include <memory>

#include "base/game_defines.h"

class GameEvent 
{
public:
	static std::shared_ptr<GameEvent> create(eEventType type);
	static std::shared_ptr<GameEvent> create(eEventType type, cocos2d::Vec2 location);	
	static std::shared_ptr<GameEvent> create(eEventType type, cocos2d::Vec2 location, int value);
	virtual bool Init();
	eEventType GetEventType();	
	cocos2d::Vec2 GetLocation();
	int GetValue();
private:
	eEventType m_EventType;
	cocos2d::Vec2 m_Location;
	int m_iValue;
};

#endif

