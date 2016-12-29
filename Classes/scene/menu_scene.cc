/*

Copyright (c) 2016 David Morton

*/
#include "scene/menu_scene.h"
#include "scene/game_scene.h"
#include "audio/include/SimpleAudioEngine.h"
#include "manager/world_manager.h"

USING_NS_CC;

Scene* MenuScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MenuScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	WorldManager::getInstance()->Restart();


	// android back press event
  /*
	auto touchListener = EventListenerKeyboard::create();
	touchListener->onKeyReleased = [](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
	{
	    if (keyCode == EventKeyboard::KeyCode::KEY_BACK)
	    {
	        Director::getInstance()->end();
	    }
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
   */

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
	
	std::string backgroundMusic = "audio/MainMenuBackgroundAudio" + std::string(MUSIC_EXTENSION);
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(backgroundMusic.c_str(), true);

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

	// Quit button
	auto closeLabel = Label::createWithTTF("quit", "fonts/Starjedi.ttf", LABEL_FONT_SIZE * 1.5);
	auto closeItem = MenuItemLabel::create(closeLabel, CC_CALLBACK_1(MenuScene::menuCloseCallback, this));
	//closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width / 2 - 20, origin.y + closeItem->getContentSize().height / 2));
	
	// Start button 
	auto googlePlayLoginLabel = Label::createWithTTF("Login", "fonts/Starjedi.ttf", LABEL_FONT_SIZE * 1.5);
	auto googlePlayLoginLabelButton = MenuItemLabel::create(googlePlayLoginLabel, CC_CALLBACK_1(MenuScene::GooglePlaySignIn, this));

	// Start button
	auto startLabel = Label::createWithTTF("start", "fonts/Starjedi.ttf", LABEL_FONT_SIZE * 1.5);
	auto startButton = MenuItemLabel::create(startLabel, CC_CALLBACK_1(MenuScene::StartGame, this));
	//startButton->setPosition(Vec2(origin.x + visibleSize.width - startButton->getContentSize().width / 2 - 20, origin.y + startButton->getContentSize().height / 2 + closeItem->getContentSize().height));

	// Leaderboard
	auto leaderboardLabel = Label::createWithTTF("open leaderboard", "fonts/Starjedi.ttf", LABEL_FONT_SIZE * 1.5f);
	auto leaderboardItem = MenuItemLabel::create(leaderboardLabel, CC_CALLBACK_1(MenuScene::OpenLeaderboard, this));

	auto achievementsLabel = Label::createWithTTF("achievements", "fonts/Starjedi.ttf", LABEL_FONT_SIZE * 1.5f);
	auto achievementsItem = MenuItemLabel::create(achievementsLabel, CC_CALLBACK_1(MenuScene::OpenAchievements, this));

    // create menu, it's an autorelease object
	auto menu = Menu::create(googlePlayLoginLabelButton, startButton, achievementsItem, leaderboardItem, closeItem, NULL);
	menu->alignItemsVerticallyWithPadding(5);
	menu->setPosition(cocos2d::Vec2(visibleSize.width * 0.75, visibleSize.height / 2));
    this->addChild(menu, 1);



    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    //auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);        
    //label->setPosition(Vec2(origin.x + visibleSize.width/2, origin.y + visibleSize.height - label->getContentSize().height));    
    //this->addChild(label, 1);

    // add "MenuScene" splash screen"
	auto background = Sprite::create("background/MenuBackground.png");

	background->setScale(visibleSize.width / background->getContentSize().width);

    // position the sprite on the center of the screen
	background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

    // add the sprite as a child to this layer
	this->addChild(background, 0);

#if (GOOGLE_ANALYTICS_ENABLED)
	// Google Analytics
	SonarCocosHelper::GoogleAnalytics::setScreenName("Main Menu");
#endif

#if (ADMOB_ENABLED)
	sdkbox::PluginAdMob::show("bannerad");
#endif

	// AdMob show banner ad
	//SonarCocosHelper::AdMob::showBannerAd(SonarCocosHelper::AdBannerPosition::eBottom);
    return true;
}

void MenuScene::GooglePlaySignIn(cocos2d::Ref *pSender)
{
#if (GOOGLE_PLAY_SERVICES_ENABLED)
	if(!SonarCocosHelper::GooglePlayServices::isSignedIn())
	{
		SonarCocosHelper::GooglePlayServices::signIn();
	}
#endif
}


void MenuScene::OpenLeaderboard(cocos2d::Ref *pSender)
{
	//cocos2d::Application::getInstance()->openURL("http://piratesvsaliens.azurewebsites.net/");
#if (GOOGLE_PLAY_SERVICES_ENABLED)
	SonarCocosHelper::GooglePlayServices::showLeaderboard(LEADERBOARD_ID);
#endif
}

void MenuScene::OpenAchievements(cocos2d::Ref *pSender)
{
#if (GOOGLE_PLAY_SERVICES_ENABLED)
	SonarCocosHelper::GooglePlayServices::showAchievements();
#endif
}

void MenuScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void MenuScene::StartGame(Ref* pSender)
{
	CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	Director::getInstance()->replaceScene(TransitionFade::create(1, GameScene::createScene()));
#if (CHARTBOOST_ENABLED)
	sdkbox::PluginChartboost::show("Level Started"); // Offer reward
#endif

}
