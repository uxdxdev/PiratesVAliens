// Copyright (c) 2016 David Morton
#include "base/game_model.h"
#include "manager/world_manager.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "SonarCocosHelperCPP/SonarFrameworks.h"
#endif

#define WAVE_ONE 5
#define WAVE_TWO 8
#define WAVE_THREE 15
#define WAVE_FOUR 20
#define WAVE_FIVE 25
#define WAVE_SIX 30
#define WAVE_SEVEN 35
#define WAVE_EIGHT 40
#define WAVE_NINE 45
#define WAVE_TEN 50
#define WAVE_BOSS 60

#define MAX_PLATFORMS 5
#define MAX_LIVES 9
#define STARTING_COINS 500
#define CURRENT_WAVE 0

GameModel::GameModel()
{
	m_iCurrentPlatforms = 0;
	m_iMaxPlatforms = MAX_PLATFORMS;
	m_iCoins = STARTING_COINS;
	m_iCurrentWave = CURRENT_WAVE;
	m_iLives = MAX_LIVES;
	m_HardToKill = true;
	m_iScore = 0;
	m_iBossKills = 0;
	m_iEnemiesKilled = 0;
	m_iItemsCollected = 0;
	m_iPlayerDeaths = 0;
	WorldManager::getInstance()->AddObserver(this);
	
	m_iWaveEnemyLimit.push_back(WAVE_ONE);	
	m_iWaveEnemyLimit.push_back(WAVE_TWO);
	m_iWaveEnemyLimit.push_back(WAVE_THREE);
	m_iWaveEnemyLimit.push_back(WAVE_FOUR);
	m_iWaveEnemyLimit.push_back(WAVE_FIVE);
	m_iWaveEnemyLimit.push_back(WAVE_SIX);
	m_iWaveEnemyLimit.push_back(WAVE_SEVEN);
	m_iWaveEnemyLimit.push_back(WAVE_EIGHT);
	m_iWaveEnemyLimit.push_back(WAVE_NINE);
	m_iWaveEnemyLimit.push_back(WAVE_TEN);		
	m_iWaveEnemyLimit.push_back(WAVE_BOSS);	

	m_iCurrentWave = 0;

	m_iMaxWaves = m_iWaveEnemyLimit.size();
}

void GameModel::OnNotify(std::shared_ptr<GameEvent> tEvent)
{
	if (tEvent->GetEventType() == ENEMY_DESTROYED)
	{
		m_iEnemiesKilled++;
		m_iCoins += 20;
		m_iScore += 10;
		WorldManager::getInstance()->Notify(GameEvent::create(UPDATE_STATS));
	}	
	else if (tEvent->GetEventType() == WAVE_END) // Increment the wave counter when a wave ends
	{
		// Only increment to the size of the enemy wave limit vector
		if (m_iCurrentWave < (int)(m_iWaveEnemyLimit.size() - 1))
		{
			UpdateScore();
			m_iCurrentWave++;
			m_iMaxPlatforms += 2;
			WorldManager::getInstance()->SetDifficulty(WorldManager::getInstance()->GetDifficulty() + 0.1);			
		}		
		else
		{
			//WorldManager::getInstance()->Notify(GameEvent::create(GAME_OVER));
		}
	}
	else if (tEvent->GetEventType() == BASE_ATTACKED)
	{		
		m_iLives--;
		m_HardToKill = false; // Player lost a life
		WorldManager::getInstance()->Notify(GameEvent::create(UPDATE_STATS));
		if (m_iLives <= 0)
		{
			m_iPlayerDeaths = 1;
			UpdateScore();			
			WorldManager::getInstance()->Notify(GameEvent::create(GAME_OVER));
		}
	}
	else if (tEvent->GetEventType() == BOSS_ATTACKED)
	{
		m_iLives = 0;
		WorldManager::getInstance()->Notify(GameEvent::create(UPDATE_STATS));
		if (m_iLives <= 0)
		{
			m_iPlayerDeaths++;
			UpdateScore();
			WorldManager::getInstance()->Notify(GameEvent::create(GAME_OVER));
		}
	}
	else if (tEvent->GetEventType() == BOSS_DESTROYED)
	{
		// Player killed the boss
		m_iBossKills++;
		WorldManager::getInstance()->GoogleAnalyticsEvent("Entities", "Boss Killed", "Player has killed the enemy boss", 0);
		WorldManager::getInstance()->UnlockAchievement(ACHIEVEMENT_BLACK_BEARD_SIX);
		m_iScore += 1000; // Boss kill bonus

		UpdateScore();

		// Player didnt lose any lives
		if (m_HardToKill)
		{
			WorldManager::getInstance()->UnlockAchievement(ACHIEVEMENT_SEVEN_HARD_TO_KILL);
			WorldManager::getInstance()->GoogleAnalyticsEvent("Achievement", "Achievement 7 Unlocked", "Player has finished the game without losing a life", 0);
		}

		// Game over
		WorldManager::getInstance()->Notify(GameEvent::create(GAME_OVER));
	}

}

void GameModel::UpdateScore()
{
	m_iScore += m_iCoins + m_iLives + m_iEnemiesKilled + m_iCurrentWave; // Very complex scoring system...
	WorldManager::getInstance()->Notify(GameEvent::create(UPDATE_STATS));

	// Achievements
	if (m_iScore >= ACHIEVEMENT_PROPER_PIRATE_FIVE_VALUE)
	{
		WorldManager::getInstance()->UnlockAchievement(ACHIEVEMENT_PROPER_PIRATE_FIVE);
		WorldManager::getInstance()->GoogleAnalyticsEvent("Achievement", "Achievement 5 Unlocked", "Player has unlocked achievement 5", 0);
	}
	else if (m_iScore >= ACHIEVEMENT_PROPER_PIRATE_FOUR_VALUE)
	{
		WorldManager::getInstance()->UnlockAchievement(ACHIEVEMENT_PROPER_PIRATE_FOUR);
		WorldManager::getInstance()->GoogleAnalyticsEvent("Achievement", "Achievement 4 Unlocked", "Player has unlocked achievement 4", 0);
	}
	else if (m_iScore >= ACHIEVEMENT_PROPER_PIRATE_THREE_VALUE)
	{
		WorldManager::getInstance()->UnlockAchievement(ACHIEVEMENT_PROPER_PIRATE_THREE);
		WorldManager::getInstance()->GoogleAnalyticsEvent("Achievement", "Achievement 3 Unlocked", "Player has unlocked achievement 3", 0);
	}
	else if (m_iScore >= ACHIEVEMENT_PROPER_PIRATE_TWO_VALUE)
	{
		WorldManager::getInstance()->UnlockAchievement(ACHIEVEMENT_PROPER_PIRATE_TWO);
		WorldManager::getInstance()->GoogleAnalyticsEvent("Achievement", "Achievement 2 Unlocked", "Player has unlocked achievement 2", 0);
	}
	else if (m_iScore >= ACHIEVEMENT_PROPER_PIRATE_ONE_VALUE)
	{
		WorldManager::getInstance()->UnlockAchievement(ACHIEVEMENT_PROPER_PIRATE_ONE);
		WorldManager::getInstance()->GoogleAnalyticsEvent("Achievement", "Achievement 1 Unlocked", "Player has unlocked achievement 1", 0);
	}

}
