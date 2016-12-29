/*

Copyright (c) 2016 David Morton

*/
#include "scene/game_scene.h"
#include "manager/world_manager.h"
#include "audio/include/SimpleAudioEngine.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);
	
    // return the scene
    return scene;
}

GameScene::~GameScene()
{
	m_pBoardLayer = nullptr;
	m_pHudLayer = nullptr;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	std::string backgroundMusic = "audio/GameSceneBackgroundAudio" + std::string(MUSIC_EXTENSION);
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(backgroundMusic.c_str(), true);

	auto background = cocos2d::Sprite::create("background/GameBackground.png");	
	background->setScale(visibleSize.width / background->getContentSize().width);
	background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(background, 0);

	m_pBoardLayer = BoardLayer::create();			
	this->addChild(m_pBoardLayer);

	m_pHudLayer = HudLayer::create();	
	this->addChild(m_pHudLayer);

	m_bPaused = false;

	WorldManager::getInstance()->SetGameScene(this);
	this->scheduleUpdate();

	
	auto intro = cocos2d::Sprite::create("background/startgamescreen.png");
	intro->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	intro->setScale(visibleSize.width / background->getContentSize().width);
	this->addChild(intro, 10);

	auto delay = cocos2d::DelayTime::create(4.0f);
	auto actionFadeOut = cocos2d::FadeOut::create(1.0f);
	auto sequenceIntro = cocos2d::Sequence::create(delay, actionFadeOut, NULL);
	intro->runAction(sequenceIntro);

#if (GOOGLE_ANALYTICS_ENABLED)	
	// Google Analytics
	SonarCocosHelper::GoogleAnalytics::setScreenName("Game Scene");
	SonarCocosHelper::GoogleAnalytics::sendEvent("Game", "Game Started", "Player started a new game", 0);
#endif

#if (ADMOB_ENABLED)
	sdkbox::PluginAdMob::hide("bannerad");
#endif
	// AdMob show banner ad
	//SonarCocosHelper::AdMob::hideBannerAd(SonarCocosHelper::AdBannerPosition::eBottom);
    return true;
}

void GameScene::update(float dt)
{
	if (!m_bPaused)
	{
		m_pBoardLayer->update(dt);
	}	
	m_pHudLayer->update(dt);
}

bool GameScene::IsPaused()
{
	return m_bPaused;
}

bool GameScene::PauseGame()
{
	if (!m_bPaused)
	{
		m_bPaused = true;		
		CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();	
		CocosDenshion::SimpleAudioEngine::getInstance()->pauseAllEffects();
		Director::getInstance()->pause();
	}
	else
	{
		m_bPaused = false;
		CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
		CocosDenshion::SimpleAudioEngine::getInstance()->resumeAllEffects();
		Director::getInstance()->resume();
	}
	return m_bPaused;
}
