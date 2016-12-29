/*

Copyright (c) 2016 David Morton

*/
#ifndef OBSERVER_H
#define OBSERVER_H

#include <memory>

#include "base/game_defines.h"
#include "base/game_event.h"

class Observer
{
public:
	virtual ~Observer(){}	
	virtual void OnNotify(std::shared_ptr<GameEvent> tEvent) = 0;
};

#endif