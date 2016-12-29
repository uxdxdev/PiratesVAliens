/*

Copyright (c) 2016 David Morton

*/
#ifndef GAME_SCENE_H__
#define GAME_SCENE_H__

#include "cocos2d.h"
#include "layer/board_layer.h"
#include "layer/hud_layer.h"

class GameScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
	~GameScene();
	virtual bool init();
    
	void update(float dt) override;
    
	bool PauseGame();
	bool IsPaused();

    // implement the "static create()" method manually
	CREATE_FUNC(GameScene);
private:
	BoardLayer *m_pBoardLayer;
	HudLayer *m_pHudLayer;
	bool m_bPaused;

};

#endif // GAME_SCENE_H__
