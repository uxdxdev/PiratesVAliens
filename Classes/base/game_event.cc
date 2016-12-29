/*

Copyright (c) 2016 David Morton

*/
#include "base/game_event.h"

std::shared_ptr<GameEvent> GameEvent::create(eEventType type)
{
	std::shared_ptr<GameEvent> pRet(new GameEvent());
	pRet->m_EventType = type;
	if (pRet->Init())
	{
		return pRet;
	}
	return NULL;
}

std::shared_ptr<GameEvent> GameEvent::create(eEventType type, cocos2d::Vec2 location)
{
	std::shared_ptr<GameEvent> pRet(new GameEvent());
	pRet->m_EventType = type;
	pRet->m_Location = location;
	if (pRet->Init())
	{
		return pRet;
	}
	return NULL;
}

std::shared_ptr<GameEvent> GameEvent::create(eEventType type, cocos2d::Vec2 location, int value)
{
	std::shared_ptr<GameEvent> pRet(new GameEvent());
	pRet->m_EventType = type;
	pRet->m_Location = location;
	pRet->m_iValue = value;
	if (pRet->Init())
	{
		return pRet;
	}
	return NULL;
}

eEventType GameEvent::GetEventType()
{
	return m_EventType;
}

cocos2d::Vec2 GameEvent::GetLocation()
{
	return m_Location;
}

int GameEvent::GetValue()
{
	return m_iValue;
}

bool GameEvent::Init()
{	
	return true;
}

