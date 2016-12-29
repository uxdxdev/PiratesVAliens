/*

Copyright (c) 2016 David Morton

*/
#ifndef BASE_GAME_MODEL
#define BASE_GAME_MODEL

#include "base/observer.h"
#include "base/game_event.h"
#include <vector>

class GameModel : public Observer{
public:

	GameModel();	

	~GameModel()
	{

	}

	void OnNotify(std::shared_ptr<GameEvent> tEvent);
	
	int GetCurrentPlatforms(){ return m_iCurrentPlatforms; }
	void SetCurrentPlatforms(int platforms){ m_iCurrentPlatforms = platforms; }

	int GetMaxPlatforms(){ return m_iMaxPlatforms; }
	void SetMaxPlatforms(int platforms){ m_iMaxPlatforms = platforms; }

	int GetCoins(){ return m_iCoins; }
	void SetCoins(int coins){ m_iCoins = coins; }

	int GetCurrentWave(){ return m_iCurrentWave; }
	int GetWaveEnemyLimit(){ return m_iWaveEnemyLimit.at(m_iCurrentWave); }
	void SetCurrentWave(int wave){ m_iCurrentWave = wave; }

	int GetMaxWaves(){ return m_iMaxWaves; }
	void SetMaxWaves(int waves){ m_iMaxWaves = waves; }

	int GetLives(){ return m_iLives; }
	void SetLives(int lives){ m_iLives = lives; }

	int GetScore(){ return m_iScore; }
	void SetScore(int score){ m_iScore = score; }

	int GetBossKills(){ return m_iBossKills; }
	void SetBossKills(int value){ m_iBossKills = value; }

	int GetEnemiesKilled(){ return m_iEnemiesKilled; }
	void SetEnemiesKilled(int killed){ m_iEnemiesKilled = killed; }

	int GetItemsCollected(){ return m_iItemsCollected; }
	void SetItemsCollected(int items){ m_iItemsCollected = items; }

	int GetPlayerDeaths(){ return m_iPlayerDeaths; }

	void UpdateScore();

private:
	int m_iCurrentPlatforms;
	int m_iMaxPlatforms;
	int m_iCoins;
	int m_iCurrentWave;
	int m_iMaxWaves;
	int m_iLives;
	int m_iScore;
	int m_iBossKills;
	int m_iEnemiesKilled;
	int m_iItemsCollected;
	int m_iPlayerDeaths;
	bool m_HardToKill;

	std::vector<int> m_iWaveEnemyLimit;
};
#endif