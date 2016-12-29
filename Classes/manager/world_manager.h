/*

Copyright (c) 2016 David Morton

*/
#ifndef WORLDMANAGER_H_
#define WORLDMANAGER_H_

#include <memory>

#include "base/subject.h"
#include "entities/game_object.h"
#include "entities/enemy.h"
#include "entities/land.h"
#include "entities/bullet.h"
#include "base/vector3.h"
#include "layer/board_layer.h"
#include "layer/hud_layer.h"
#include "ai/pathfinding_astar.h"
#include "scene/game_scene.h"
#include "scene/game_over_scene.h"
#include "dao/data_access_object.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "SonarCocosHelperCPP/SonarFrameworks.h"
#endif

// Plugins
#if (CHARTBOOST_ENABLED)
#include "pluginchartboost/PluginChartboost.h"
#endif
#if (ADMOB_ENABLED)
#include "pluginadmob/PluginAdMob.h"
#endif


class WorldManager : public Subject
{
public:
	static WorldManager* getInstance();
	bool Init();
	void CleanUp();
	void Restart();
	void Update(float dt);	
	void CheckForCollisions(float dt);

	void SetGameScene(GameScene *gameScene);
	GameScene *GetGameScene();

	void ScaleSprite(cocos2d::Sprite *sprite);

	cocos2d::Vec2 GetCenter(cocos2d::Sprite *sprite);
	cocos2d::Vec2 GetGameArea();


	GameModel *GetGameModel()
	{ 
		if (m_GameModel == nullptr)
		{
			// Game model
			std::shared_ptr<GameModel> gameModel(new GameModel());
			m_GameModel = std::move(gameModel);
		}
		return m_GameModel.get(); 
	}

	GameObject *GetSelectedObjectByType(cocos2d::Vec2 location, eObjectType type);

	cocos2d::Vec2 ConvertTiledToScreenCoordinates(cocos2d::Vec2 position);
	cocos2d::Vec2 ConvertScreenToTiledCoordinates(cocos2d::Vec2 position);

	void RegisterWithWorldManger(std::shared_ptr<GameObject> pGameObj);
	
	//void AddBulletToList(cocos2d::Sprite *bullet);
	void AddBulletToList(std::shared_ptr<Bullet> bullet);

	std::vector<std::shared_ptr<GameObject>> *GetGameObjects();	
	std::vector<std::shared_ptr<Enemy>> *GetEnemyObjects();
	//std::vector<cocos2d::Sprite*> *GetBulletObjects();
	std::vector<std::shared_ptr<Bullet>> *GetBulletObjects();

	void SetGameBoard(BoardLayer *board);
	BoardLayer *GetGameBoard();

	void SetHudLayer(HudLayer *hudlayer);
	HudLayer *GetHudLayer();

	void SetGameOverScene(GameOver *gameOverScene){ m_GameOverScene = gameOverScene; }
	GameOver *GetGameOverScene(){ return m_GameOverScene; }

	void SetEnemyTargetPosition(cocos2d::Vec2 target);
	cocos2d::Vec2 GetEnemyTargetPosition();

	cocos2d::Vec2 GetEnemyTargetSpritePosition(cocos2d::Vec2 position);

	void SetEnemySpawnPosition(cocos2d::Vec2 position);
	cocos2d::Vec2 GetEnemySpawnPosition();

	void SetTiledMap(cocos2d::TMXTiledMap *tiledMap);

	void SetGameScaleFactor(float gameScaleFactor);
	float GetGameScaleFactor();

	cocos2d::TMXTiledMap *GetTiledMap(){ return m_TiledMap; }
	cocos2d::Vec2 PositionToTileCoordinates(cocos2d::Vec2 position);

	// Get sprite at tilemap coordinate position e.g (4, 6)
	cocos2d::Sprite *GetSpriteAtTilePosition(cocos2d::Vec2 position);

	void UpdatePath();
	std::vector<std::shared_ptr<base::Vector3> > *GetPathToTarget(){ return &m_vPathToGoal; }

	void SetGameWorldCellState(cocos2d::Vec2 location, base::GameWorld::CellState state);

	void SetPathfinder(ai::AStar *pathfinder);
	ai::AStar *GetPathfinder();

	void SetSpawnManager(SpawnManager *spawnManager);
	SpawnManager *GetSpawnManager();

	// Path is not blocked by platform build
	bool IsAllowedToBuild(){ return m_bIsAllowedToBuild; }

	void SetGameState(eEventType state){ m_eGameState = state; }
	eEventType GetGameState(){ return m_eGameState; }

	// Data access object
	void CreateDao();
	void UpdateDAO();
	// HTTP response callback function
	void onHttpRequestCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);

	void CheckUsername(std::string username);
	void QueryPlayerRank(std::string username);

	void SetAccessGranted(bool value);
	bool GetAccessGranted(){ return m_bAccessGranted; };
	void SetUsername(std::string username){ m_strInputUsername = username; };
	std::string GetUsername(){ return m_strInputUsername; }

	// Access to the game session time 
	int GetTimePlayedSeconds(){ return m_nTimePlayedSeconds; };
	void SetTimePlayedSeconds(int secondsPerGame){ m_nTimePlayedSeconds = secondsPerGame; };
			
	void SetDifficulty(float value){ m_fDifficultiyMultiplier = value; }
	float GetDifficulty(){ return m_fDifficultiyMultiplier; }

	void UnlockAchievement(const char *achievementId)
	{
#if (GOOGLE_PLAY_SERVICES_ENABLED)
		SonarCocosHelper::GooglePlayServices::unlockAchievement(achievementId);
#endif
	}

	void GoogleAnalyticsEvent(cocos2d::__String category, cocos2d::__String action, cocos2d::__String label, long value)
	{
#if (GOOGLE_ANALYTICS_ENABLED)
		SonarCocosHelper::GoogleAnalytics::sendEvent(category, action, label, value);
#endif
	}

	void WriteToLogFile(const char *message);
	void WriteToLogFile(std::stringstream &message);

private:
	WorldManager();
	~WorldManager();
	static WorldManager* m_Instance;	
	std::vector<std::shared_ptr<GameObject>> m_vGameOjects;
	std::vector<std::shared_ptr<Enemy>> m_vEnemyObjects;
	//std::vector<cocos2d::Sprite*> m_vBulletObjects;
	std::vector<std::shared_ptr<Bullet>> m_vBulletObjects;
	std::vector<std::shared_ptr<Land>> m_vLandObjects;
	
	std::shared_ptr<GameModel> m_GameModel;

	BoardLayer *m_pGameBoard;
	HudLayer *m_pHudLayer;
	GameScene *m_GameScene;
	GameOver *m_GameOverScene;

	cocos2d::Vec2 m_EnemyTargetPosition;
	cocos2d::Vec2 m_EnemySpawnPosition;
	cocos2d::Size m_MapSize;
	cocos2d::Size m_TileSize;

	cocos2d::TMXTiledMap *m_TiledMap;
	ai::AStar *m_pPathfinder;
	SpawnManager *m_pSpawnManager;

	std::vector<std::shared_ptr<base::Vector3> > m_vPathToGoal;

	cocos2d::TMXLayer *m_BackgroundLayer;	
	bool m_bIsAllowedToBuild;
	std::vector<cocos2d::Sprite*> m_vPathMarkers;
	cocos2d::Size m_VisibleSize;
	float m_fContentScaleFactor;
	eEventType m_eGameState;

	// Data access object
	std::shared_ptr<IDao> m_DataAccessObject;
	bool m_bAccessGranted;
	std::string m_strInputUsername;
	int m_nTimePlayedSeconds;
	float m_fDifficultiyMultiplier;
};

#if (CHARTBOOST_ENABLED)
class CBListener : public sdkbox::ChartboostListener{
public:
	void onChartboostCached(const std::string& name)
	{
		if(name == "Default")
		{
			//sdkbox::PluginChartboost::show("Default");
		}
	}
	bool onChartboostShouldDisplay(const std::string& name){}
	void onChartboostDisplay(const std::string& name){}
	void onChartboostDismiss(const std::string& name){}
	void onChartboostClose(const std::string& name){}
	void onChartboostClick(const std::string& name){}
	void onChartboostReward(const std::string& name, int reward)
	{
		if(name == sdkbox::CB_Location_LevelStart)
		{
			WorldManager::getInstance()->GetGameModel()->SetLives(WorldManager::getInstance()->GetGameModel()->GetLives() + 3);
			WorldManager::getInstance()->Notify(GameEvent::create(UPDATE_STATS));
		}
	}
	void onChartboostFailedToLoad(const std::string& name, sdkbox::CB_LoadError e){}
	void onChartboostFailToRecordClick(const std::string& name, sdkbox::CB_ClickError e){}
	void onChartboostConfirmation(){}
	void onChartboostCompleteStore(){}
};
#endif

#if (ADMOB_ENABLED)
class IMListener : public sdkbox::AdMobListener{
public:
    virtual void adViewDidReceiveAd(const std::string &name) {
    	if(name == "bannerad")
    	{
    		sdkbox::PluginAdMob::show(name);
    	}
    }
    virtual void adViewDidFailToReceiveAdWithError(const std::string &name, const std::string &msg) {
    }
    virtual void adViewWillPresentScreen(const std::string &name) {
    }
    virtual void adViewDidDismissScreen(const std::string &name) {
    }
    virtual void adViewWillDismissScreen(const std::string &name) {
    }
    virtual void adViewWillLeaveApplication(const std::string &name) {
    }
};
#endif
#endif
