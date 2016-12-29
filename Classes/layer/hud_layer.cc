/*

Copyright (c) 2016 David Morton

*/
#include "layer/hud_layer.h"
#include "manager/world_manager.h"
#include "audio/include/SimpleAudioEngine.h"
#include "scene/menu_scene.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "SonarCocosHelperCPP/SonarFrameworks.h"
#endif

#include <string>

USING_NS_CC;

bool HudLayer::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	WorldManager::getInstance()->SetHudLayer(this);
	WorldManager::getInstance()->AddObserver(this);
	// Screen dimensions
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// Background
	m_BackgroundImage = Sprite::create("background/HudBackground.png");
	m_BackgroundImage->setScale(visibleSize.width / m_BackgroundImage->getContentSize().width);
	m_BackgroundImage->setPosition(Vec2(this->getContentSize().width / 2 + origin.x, this->getContentSize().height / 2 + origin.y));
	m_BackgroundImage->setVisible(false);
	this->addChild(m_BackgroundImage, 0);

	

	m_fScaleFactor = WorldManager::getInstance()->GetGameScaleFactor();

	// Menu items
	m_PauseButton = MenuItemImage::create(
		"buttons/PauseNormal.png",
		"buttons/PauseSelected.png",
		CC_CALLBACK_1(HudLayer::PauseGame, this));
	m_PauseButton->setPosition(Vec2(origin.x + m_PauseButton->getContentSize().width / 2,
		origin.y + visibleSize.height - m_PauseButton->getContentSize().height / 2));
	m_PauseButton->setVisible(true);
	
	m_PauseButton->setScale(m_fScaleFactor);
	m_Menu = Menu::create(m_PauseButton, NULL);
	m_Menu->setPosition(Vec2::ZERO);
	this->addChild(m_Menu);


	// Build Menu
	m_WaveStart = MenuItemImage::create(
		"buttons/SkullNormal.png",
		"buttons/SkullSelected.png",
		CC_CALLBACK_1(HudLayer::StartWave, this));
	m_WaveStart->setPosition(Vec2(origin.x + visibleSize.width - m_WaveStart->getContentSize().width / 2,
		origin.y + visibleSize.height - (m_WaveStart->getContentSize().height / 2)));
	//m_WaveStart->setScale(m_fScaleFactor);

	m_bWaveActive = false;

	m_BuildPlatformToggle = MenuItemImage::create(
		"buttons/PlatformNormal.png",
		"buttons/PlatformSelected.png",
		CC_CALLBACK_1(HudLayer::TogglePlatformBuilding, this));
	m_BuildPlatformToggle->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
	m_BuildPlatformToggle->setPosition(Vec2(origin.x + visibleSize.width - (m_BuildPlatformToggle->getContentSize().width / 2), origin.y + (m_BuildPlatformToggle->getContentSize().height / 2)));
	//m_BuildPlatformToggle->setPosition(Vec2(origin.x + (m_BuildPlatformToggle->getContentSize().width / 2), origin.y + (m_BuildPlatformToggle->getContentSize().height / 2)));
	//m_BuildPlatformToggle->setScale(m_fScaleFactor);

	// Platform button particles
	m_PlatformButtonParticles = ParticleSystemQuad::create("particle/Flower.plist");
	m_PlatformButtonParticles->setPosition(Point(m_BuildPlatformToggle->getContentSize().width / 2, m_BuildPlatformToggle->getContentSize().height / 2));
	m_PlatformButtonParticles->setEmissionRate(200.00);
	m_PlatformButtonParticles->setTotalParticles(40);
	m_PlatformButtonParticles->setAutoRemoveOnFinish(true);
	m_BuildPlatformToggle->addChild(m_PlatformButtonParticles);


	m_BuildMenu = Menu::create(m_BuildPlatformToggle, m_WaveStart, NULL);
	m_BuildMenu->setPosition(Vec2::ZERO);
	this->addChild(m_BuildMenu);
		
	m_bIsBuildingPlatforms = false;


	// Platform and Gold stats
	m_LivesLabel = Label::createWithTTF("Lives " + to_string(WorldManager::getInstance()->GetGameModel()->GetLives()), "fonts/Starjedi.ttf", LABEL_FONT_SIZE * m_fScaleFactor * 2.0f);
	m_LivesLabel->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
	m_LivesLabel->enableOutline(Color4B(0, 0, 0, 255), 3);
	m_LivesLabel->setPosition(cocos2d::Vec2(origin.x + 20 + m_LivesLabel->getContentSize().width / 2, origin.y + m_LivesLabel->getContentSize().height / 2));
	m_LivesLabel->setTextColor(cocos2d::Color4B(255, 0, 114, 255));
	this->addChild(m_LivesLabel);

	m_WavesLabel = Label::createWithTTF("  Wave " +
		to_string(WorldManager::getInstance()->GetGameModel()->GetCurrentWave() + 1) +
		"/" +
		to_string(WorldManager::getInstance()->GetGameModel()->GetMaxWaves()), LABEL_FONT, LABEL_FONT_SIZE * m_fScaleFactor * 1.5f);
	//m_WavesLabel->setPosition(cocos2d::Vec2(m_LivesLabel->getPositionX() - (PADDING_LARGE + m_WavesLabel->getContentSize().width / 2), origin.y + visibleSize.height - (m_WavesLabel->getContentSize().height / 2)));
	m_WavesLabel->setTextColor(cocos2d::Color4B(0, 246, 255, 255));
	m_WavesLabel->enableOutline(Color4B(0, 0, 0, 255), 3);
	//this->addChild(m_WavesLabel);

	m_PlatformsLabel = Label::createWithTTF("  Platforms " +
		to_string(WorldManager::getInstance()->GetGameModel()->GetCurrentPlatforms()) +
		"/" +
		to_string(WorldManager::getInstance()->GetGameModel()->GetMaxPlatforms()), LABEL_FONT, LABEL_FONT_SIZE * m_fScaleFactor * 1.5f);
	//m_PlatformsLabel->setPosition(cocos2d::Vec2(m_WavesLabel->getPositionX() - (PADDING_LARGE + m_PlatformsLabel->getContentSize().width / 2), origin.y + visibleSize.height - (m_PlatformsLabel->getContentSize().height / 2)));
	m_PlatformsLabel->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
	m_PlatformsLabel->setTextColor(cocos2d::Color4B(184, 118, 52, 255));	 
	m_PlatformsLabel->enableOutline(Color4B(0, 0, 0, 255), 3);
	//this->addChild(m_PlatformsLabel);

	
	m_CoinsLabel = Label::createWithTTF("  Gold $" + to_string(WorldManager::getInstance()->GetGameModel()->GetCoins()), LABEL_FONT, LABEL_FONT_SIZE * m_fScaleFactor * 1.5f);
	//m_CoinsLabel->setPosition(cocos2d::Vec2(m_PlatformsLabel->getPositionX() - (PADDING_LARGE + m_CoinsLabel->getContentSize().width), origin.y + visibleSize.height - (m_CoinsLabel->getContentSize().height / 2)));
	m_CoinsLabel->enableOutline(Color4B(0, 0, 0, 255), 3);
	m_CoinsLabel->setTextColor(cocos2d::Color4B(232, 232, 0, 255));
	//this->addChild(m_CoinsLabel);

	m_TotalScoreLabel = Label::createWithTTF("   Score " + to_string(WorldManager::getInstance()->GetGameModel()->GetScore()), LABEL_FONT, LABEL_FONT_SIZE * m_fScaleFactor * 1.5f);
	//m_TotalScoreLabel->setPosition(cocos2d::Vec2(m_CoinsLabel->getPositionX() - (PADDING_SMALL + m_TotalScoreLabel->getContentSize().width), origin.y + visibleSize.height - (m_TotalScoreLabel->getContentSize().height / 2)));
	m_TotalScoreLabel->enableOutline(Color4B(0, 0, 0, 255), 3);
	//this->addChild(m_TotalScoreLabel);	

	// Menu items
	//auto livesItem = MenuItemLabel::create(m_LivesLabel);
	auto wavesItem = MenuItemLabel::create(m_WavesLabel);
	auto platformsItem = MenuItemLabel::create(m_PlatformsLabel);
	auto coinsItem = MenuItemLabel::create(m_CoinsLabel);
	//auto scoreItem = MenuItemLabel::create(m_TotalScoreLabel);

	// Hud menu
	auto hudMenuItems = Menu::create(wavesItem, platformsItem, coinsItem, NULL);
	hudMenuItems->alignItemsHorizontallyWithPadding(20);
	hudMenuItems->setPosition(cocos2d::Vec2(visibleSize.width / 2, visibleSize.height - wavesItem->getContentSize().height / 2));
	this->addChild(hudMenuItems);
	
	// Pause menu
	auto surrenderLabel = Label::createWithTTF("Surrender", "fonts/Starjedi.ttf", LABEL_FONT_SIZE * m_fScaleFactor * 3.0f);
	surrenderLabel->setTextColor(Color4B(35, 255, 50, 255));
	surrenderLabel->enableOutline(Color4B(0, 0, 0, 255), 3);
	auto surrenderItem = MenuItemLabel::create(surrenderLabel, CC_CALLBACK_1(HudLayer::Surrender, this));

	auto resumeLabel = Label::createWithTTF("Resume", "fonts/Starjedi.ttf", LABEL_FONT_SIZE * m_fScaleFactor * 3.0f);
	resumeLabel->setTextColor(Color4B(255, 76, 91, 255));
	resumeLabel->enableOutline(Color4B(0, 0, 0, 255), 3);
	auto resumeItem = MenuItemLabel::create(resumeLabel, CC_CALLBACK_1(HudLayer::Resume, this));

	m_PauseMenu = Menu::create(resumeItem, surrenderItem, NULL);
	m_PauseMenu->alignItemsVerticallyWithPadding(5);
	m_PauseMenu->setPosition(cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 2));
	m_PauseMenu->setVisible(false);
	this->addChild(m_PauseMenu);

	return true;
}

bool HudLayer::IsBuildingPlatforms()
{
	return m_bIsBuildingPlatforms;
}

void HudLayer::PauseGame(Ref* pSender)
{
	WorldManager::getInstance()->GetGameScene()->PauseGame();
	m_PauseMenu->setVisible(true);
	m_BackgroundImage->setVisible(true);
	m_Menu->setVisible(false);
	m_BuildMenu->setVisible(false);
}

void HudLayer::Surrender(Ref* pSender)
{
	// Return to menu scene	
	WorldManager::getInstance()->GetGameScene()->PauseGame();
	m_PauseMenu->setEnabled(false);
	//m_BackgroundImage->setVisible(false);
	CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
	//Director::getInstance()->replaceScene(TransitionFade::create(1, MenuScene::createScene()));
	WorldManager::getInstance()->GetGameModel()->SetLives(0);
	GameOverAnimation();
}

void HudLayer::Resume(Ref* pSender)
{
	// Call pause game to toggle
	WorldManager::getInstance()->GetGameScene()->PauseGame();
	m_PauseMenu->setVisible(false);
	m_BackgroundImage->setVisible(false);
	m_Menu->setVisible(true);

	if (m_bWaveActive == false)
	{		
		m_BuildMenu->setVisible(true);
	}	
}

void HudLayer::GameOverAnimation()
{
	WorldManager::getInstance()->SetGameState(GAME_OVER);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	cocos2d::Sprite *gameOverDisplay;

	if (WorldManager::getInstance()->GetGameModel()->GetLives() > 0)
	{
		gameOverDisplay = cocos2d::Sprite::create("background/youwonscreen.png");

		// Play village cheering music when the game is won
		std::string villageVictory = "audio/VictoryCheerLoop" + std::string(MUSIC_EXTENSION);
		CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(villageVictory.c_str(), true);

		// Google Analytics
#if (GOOGLE_ANALYTICS_ENABLED)
		SonarCocosHelper::GoogleAnalytics::sendEvent("Game", "Game Won", "Player has won the game", 0);
#endif
	}
	else
	{
		gameOverDisplay = cocos2d::Sprite::create("background/youdiedscreen.png");

		// Play village in total panic sound effect
		std::string villagePanic = "audio/Panic" + std::string(MUSIC_EXTENSION);
		CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(villagePanic.c_str(), true);

		// Google Analytics
#if (GOOGLE_ANALYTICS_ENABLED)
		SonarCocosHelper::GoogleAnalytics::sendEvent("Game", "Game Lose", "Player has lost the game", 0);
#endif
	}
	
	gameOverDisplay->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	gameOverDisplay->setScale(visibleSize.width / gameOverDisplay->getContentSize().width);

	this->addChild(gameOverDisplay, 10);
	gameOverDisplay->setOpacity(0);

	// Scale up to cover the screen
	auto action = cocos2d::FadeIn::create(3.0f);
	gameOverDisplay->runAction(action);

	m_BuildMenu->setVisible(false);
	//m_PauseButton->setVisible(false);
}

void HudLayer::update(float dt)
{
}

void HudLayer::OnNotify(std::shared_ptr<GameEvent> tEvent)
{	
	if (tEvent->GetEventType() == START_SPAWNING) // Turn off the build menu when the enemies start to spawn
	{
		m_BuildMenu->setVisible(false);
		//m_PauseButton->setVisible(true);
	}
	else if (tEvent->GetEventType() == WAVE_END) // Turn on the build menu when the last enemy has been destroyed, this is different to the STOP_SPAWNING event
	{
		m_bWaveActive = false;
		if (WorldManager::getInstance()->GetGameState() != GAME_OVER)
		{
			m_BuildMenu->setVisible(true);
			auto sequence = Sequence::create(ScaleTo::create(0.5f, 1.5f), ScaleTo::create(0.5f, 1.0f), nullptr);
			m_BuildPlatformToggle->runAction(sequence);
			//m_PauseButton->setVisible(false);
		}		
	}
	else if (tEvent->GetEventType() == UPDATE_STATS)
	{
		// Coins
		m_CoinsLabel->setString("  Gold $" + to_string(WorldManager::getInstance()->GetGameModel()->GetCoins()));

		// Score
		//m_TotalScoreLabel->setString("   Score " + to_string(WorldManager::getInstance()->GetGameModel()->GetScore()));
		
		// Platforms
		m_PlatformsLabel->setString("  Platforms " +
			to_string(WorldManager::getInstance()->GetGameModel()->GetCurrentPlatforms()) +
			"/" +
			to_string(WorldManager::getInstance()->GetGameModel()->GetMaxPlatforms()));

		// Set particles for platform build button
		if (WorldManager::getInstance()->GetGameModel()->GetCurrentPlatforms() < WorldManager::getInstance()->GetGameModel()->GetMaxPlatforms())
		{
			m_PlatformButtonParticles->setVisible(true);
		}
		else
		{
			m_PlatformButtonParticles->setVisible(false);			
		}

		// Waves
		m_WavesLabel->setString("  Wave " +
			to_string(WorldManager::getInstance()->GetGameModel()->GetCurrentWave() + 1) +
			"/" +
			to_string(WorldManager::getInstance()->GetGameModel()->GetMaxWaves()));
	
		// Lives
		m_LivesLabel->setString("Lives " + to_string(WorldManager::getInstance()->GetGameModel()->GetLives()));
	}
	else if (tEvent->GetEventType() == RESOURCES_TOO_LOW) // Turn on the build menu when the last enemy has been destroyed, this is different to the STOP_SPAWNING event
	{
		auto sequence = Sequence::create(ScaleTo::create(0.5f, 1.5f), ScaleTo::create(0.5f, 1.0f), nullptr);
		m_CoinsLabel->runAction(sequence);
	}
	else if (tEvent->GetEventType() == BASE_ATTACKED)
	{
		//auto sequence = cocos2d::Sequence::create(cocos2d::ScaleTo::create(0.5f, 1.5f), cocos2d::ScaleTo::create(0.5f, 1.0f), nullptr);
		//m_LivesLabel->runAction(sequence);
	}
}

void HudLayer::TogglePlatformBuilding(Ref* pSender)
{
	if (m_bIsBuildingPlatforms)
	{
		m_bIsBuildingPlatforms = false;
		m_BuildPlatformToggle->unselected();
	}
	else
	{
		m_bIsBuildingPlatforms = true;		
		m_BuildPlatformToggle->selected();
	}
}

void HudLayer::StartWave(Ref* pSender)
{
	WorldManager::getInstance()->Notify(GameEvent::create(START_SPAWNING, WorldManager::getInstance()->GetEnemySpawnPosition()));
	DisableBuildMenuOptions();

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto waveLabel = Label::createWithTTF("Wave " + to_string(WorldManager::getInstance()->GetGameModel()->GetCurrentWave() + 1), LABEL_FONT, LABEL_FONT_SIZE * m_fScaleFactor * 4.0f);
	waveLabel->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	waveLabel->enableOutline(Color4B(0, 0, 0, 255), 3);
	waveLabel->setOpacity(0);
	waveLabel->setTextColor(cocos2d::Color4B(255, 255, 255, 255));
	this->addChild(waveLabel, 5);
	
	auto delay = cocos2d::DelayTime::create(2.0f);
	auto actionFadeIn = cocos2d::FadeIn::create(0.5f);
	auto actionFadeOut = cocos2d::FadeOut::create(0.5f);
	auto sequenceIntro = cocos2d::Sequence::create(actionFadeIn, delay, actionFadeOut, NULL);
	waveLabel->runAction(sequenceIntro);

	m_bWaveActive = true;
}

void HudLayer::DisableBuildMenuOptions()
{
	// Turn off all platform build options
	m_bIsBuildingPlatforms = false;	
	m_BuildPlatformToggle->unselected();
}
