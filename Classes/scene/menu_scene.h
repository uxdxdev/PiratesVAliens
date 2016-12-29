/*

Copyright (c) 2016 David Morton

*/
#ifndef MENU_SCENE_H_
#define MENU_SCENE_H_

#include "cocos2d.h"

class MenuScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
	// Start GameScene
	void StartGame(Ref* pSender);

	void GooglePlaySignIn(cocos2d::Ref *pSender);

	void OpenLeaderboard(cocos2d::Ref *pSender);

	void OpenAchievements(cocos2d::Ref *pSender);

    // implement the "static create()" method manually
	CREATE_FUNC(MenuScene);
};

#endif // GAME_SCENE_H__
