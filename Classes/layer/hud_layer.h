/*

Copyright (c) 2016 David Morton

*/
#ifndef LAYER_HUD_LAYER_H
#define LAYER_HUD_LAYER_H

#include "base/game_defines.h"
#include "base/observer.h"

class HudLayer : public cocos2d::Layer, public Observer
{
public:
	virtual bool init();

	CREATE_FUNC(HudLayer);

	void update(float dt) override;	
	void OnNotify(std::shared_ptr<GameEvent> tEvent);
	void TogglePlatformBuilding(Ref* pSender);
	void PauseGame(Ref* pSender);
	void Surrender(Ref* pSender);
	void Resume(Ref* pSender);
	void StartWave(Ref* pSender);
	bool IsBuildingPlatforms();
	void DisableBuildMenuOptions();
	void GameOverAnimation();
	cocos2d::Label *GetPlatformLabel(){ return m_PlatformsLabel; }
private:
	cocos2d::Sprite *m_BuildMenuContainer;
	cocos2d::Sprite *m_BackgroundImage;
	cocos2d::MenuItemImage *m_WaveStart;
	cocos2d::MenuItemImage *m_BuildPlatformToggle;
	bool m_bIsBuildingPlatforms;
	cocos2d::Menu *m_Menu;
	cocos2d::Menu *m_BuildMenu;
	cocos2d::Menu *m_PauseMenu;
	cocos2d::MenuItemImage *m_PauseButton;

	cocos2d::Label *m_CoinsLabel;
	cocos2d::Label *m_TotalScoreLabel;
	cocos2d::Label *m_PlatformsLabel;
	cocos2d::Label *m_LivesLabel;
	cocos2d::Label *m_WavesLabel;

	float m_fScaleFactor;

	cocos2d::ParticleSystem *m_PlatformButtonParticles;

	bool m_bWaveActive;
};

#endif
