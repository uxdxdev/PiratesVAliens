/*

Copyright (c) 2016 David Morton

*/
#ifndef _BOARDLAYER_H_
#define _BOARDLAYER_H_

#include <memory>

#include "manager/spawn_manager.h"
#include "ai/pathfinding_astar.h"
#include "base/game_defines.h"
#include "base/observer.h"
#include "base/game_model.h"
#include "layer/build_menu.h"
#include "layer/upgrade_menu.h"

class BoardLayer : public cocos2d::Layer, public Observer
{
public:
	virtual bool init();
	~BoardLayer();
	CREATE_FUNC(BoardLayer);
	void OnNotify(std::shared_ptr<GameEvent> tEvent);
	void StartWave(Ref* pSender);
	void WaveTimer(float dt);
	void update(float dt) override;
	virtual bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
	virtual void onTouchEnded(cocos2d::Touch*, cocos2d::Event*);
	cocos2d::TMXTiledMap *GetTiledMap(){ return m_TiledMap; }
	bool IsInsideGameArea(cocos2d::Vec2 touchLocation);
	BuildMenuLayer *GetBuildMenu(){ return m_BuildMenu; }
	UpgradeMenuLayer *GetUpgradeMenu(){ return m_UpgradeMenu; }
	void UpdateTimer(float dt);

private:
	cocos2d::Size m_MapSize;
	cocos2d::Size m_TileSize;

	std::shared_ptr<SpawnManager> m_pSpawnManager;
	std::shared_ptr<ai::AStar> m_pPathfinder;
	cocos2d::TMXTiledMap *m_TiledMap;
	GameModel *m_GameModel;
	BuildMenuLayer *m_BuildMenu;
	UpgradeMenuLayer *m_UpgradeMenu;

	cocos2d::Sprite *m_TouchSprite;
	cocos2d::Vec2 m_GameArea;	
	int m_AlienWaveAudionId;
	std::string m_WaveEndBell;
	std::string m_WaveEndComms;
	int m_iGameTime;
};

#endif