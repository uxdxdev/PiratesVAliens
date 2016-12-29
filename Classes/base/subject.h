/*

Copyright (c) 2016 David Morton

*/
#ifndef SUBJECT_H
#define SUBJECT_H

#define MAX_OBSERVERS 5

#include <vector>

#include "base/observer.h"
#include "base/game_event.h"

class Subject
{
protected:
	// Vector of registered observers
	std::vector<Observer*> m_vObservers;

public:

	void Notify(std::shared_ptr<GameEvent> tEvent)
	{
		// Walk the array and notify each observer
		for (size_t i = 0; i < m_vObservers.size(); i++)
		{
			m_vObservers[i]->OnNotify(tEvent);
		}
	}

	// Register an observer
	void AddObserver(Observer* observer)
	{
		m_vObservers.push_back(observer);
	}

	// Remove an observer
	void RemoveObserver(Observer* observer)
	{
		for (size_t i = 0; i < m_vObservers.size(); i++)
		{
			if (m_vObservers[i] == observer)
			{
				m_vObservers.erase(m_vObservers.begin() + i);
			}
		} 
	}

};

#endif