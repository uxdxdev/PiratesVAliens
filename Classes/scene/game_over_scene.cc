/*

Copyright (c) 2016 David Morton

*/
#include "scene/game_over_scene.h"
#include "audio/include/SimpleAudioEngine.h"
#include "scene/menu_scene.h"
#include "manager/world_manager.h"

USING_NS_CC;

Scene* GameOver::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameOver::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool GameOver::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}
	WorldManager::getInstance()->SetGameOverScene(this);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
		
	float scaleFactor = WorldManager::getInstance()->GetGameScaleFactor();

	std::string backgroundMusic = "audio/MainMenuBackgroundAudio" + std::string(MUSIC_EXTENSION);
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(backgroundMusic.c_str(), true);

	auto background = cocos2d::Sprite::create("background/GameOverBackground.png");
	background->setScale(visibleSize.width / background->getContentSize().width);
	background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(background, 0);
		
	
	// Restart button
	auto restartLabel = Label::createWithTTF("restart", "fonts/Starjedi.ttf", LABEL_FONT_SIZE * scaleFactor * 1.5f);
	auto restartItem = MenuItemLabel::create(restartLabel, CC_CALLBACK_1(GameOver::GameRestart, this));
	//restartItem->setPosition(Vec2(origin.x + visibleSize.width - restartItem->getContentSize().width / 2 - 20, origin.y + restartItem->getContentSize().height / 2));
		
	// Leaderboard
	auto leaderboardLabel = Label::createWithTTF("leaderboard", "fonts/Starjedi.ttf", LABEL_FONT_SIZE * scaleFactor * 1.5f);
	auto leaderboardItem = MenuItemLabel::create(leaderboardLabel, CC_CALLBACK_1(GameOver::OpenLeaderboard, this));

	createTF();

	// Open keyboard
	auto openKeyboardItem = MenuItemLabel::create(Label::create(), CC_CALLBACK_1(GameOver::OpenKeyboard, this));
	openKeyboardItem->setContentSize(m_pTextField->getContentSize());	
	auto keyboardMenu = Menu::create(openKeyboardItem, NULL);
	keyboardMenu->setPosition(m_pTextField->getPosition());
	//this->addChild(keyboardMenu);

	// Create the main menu items
	auto saveGameLabel = Label::createWithTTF("submit", "fonts/Starjedi.ttf", LABEL_FONT_SIZE * scaleFactor * 1.0f);
	m_pSaveButton = MenuItemLabel::create(saveGameLabel, CC_CALLBACK_1(GameOver::SaveGame, this));
	//m_pSaveButton->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + m_pTextField->getPositionY() - m_pTextField->getContentSize().height - m_pSaveButton->getContentSize().height / 2));
	m_pSaveButton->setVisible(false);

	auto shareGameLabel = Label::createWithTTF("share", "fonts/Starjedi.ttf", LABEL_FONT_SIZE * scaleFactor * 1.5f);
	m_pShareButton = MenuItemLabel::create(shareGameLabel, CC_CALLBACK_1(GameOver::ShareScore, this));
	//m_pShareButton->setVisible(false);

	// Create the main menu items
	auto checkUsernameLabel = Label::createWithTTF("check username", "fonts/Starjedi.ttf", LABEL_FONT_SIZE * scaleFactor * 1.0f);
	m_pCheckUsernameButton = MenuItemLabel::create(checkUsernameLabel, CC_CALLBACK_1(GameOver::checkUsername, this));
	//m_pCheckUsernameButton->setPosition(Vec2(origin.x + (m_pCheckUsernameButton->getContentSize().width / 2) + PADDING_SMALL, origin.y + restartItem->getContentSize().height / 2 + 20));
	m_pCheckUsernameButton->setVisible(true);
	
	m_LoginLabel = Label::createWithTTF("please wait...", LABEL_FONT, LABEL_FONT_SIZE * scaleFactor * 0.7);	
	m_LoginLabel->setPositionX(checkUsernameLabel->getContentSize().width / 2);
	m_LoginLabel->setColor(cocos2d::Color3B(0, 0, 0));
	m_LoginLabel->setVisible(false);
	checkUsernameLabel->addChild(m_LoginLabel);

	// create menu, it's an autorelease object
	//auto menu = Menu::create(m_pCheckUsernameButton, m_pSaveButton, m_pShareButton, leaderboardItem, restartItem, NULL);
	auto menu = Menu::create(m_pShareButton, leaderboardItem, restartItem, NULL);
	//menu->setPosition(Vec2::ZERO);
	menu->setPosition(cocos2d::Point(visibleSize.width / 2, visibleSize.height / 3));
	menu->alignItemsVerticallyWithPadding(2);
	this->addChild(menu, 1);
	
	m_ScoreLabel = Label::createWithTTF("Score " + to_string(WorldManager::getInstance()->GetGameModel()->GetScore()), LABEL_FONT, LABEL_FONT_SIZE * scaleFactor * 3.0);
	//m_ScoreLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, m_pTextField->getPositionY() + m_pTextField->getContentSize().height / 2 + m_ScoreLabel->getContentSize().height / 2));
	m_ScoreLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height * 0.6f));
	m_ScoreLabel->setTextColor(cocos2d::Color4B(255, 0, 114, 255));
	m_ScoreLabel->enableOutline(Color4B(0, 0, 0, 255), 5);
	this->addChild(m_ScoreLabel);
	
	// Submit score to Google Play Leaderboard
#if (GOOGLE_PLAY_SERVICES_ENABLED)
	SonarCocosHelper::GooglePlayServices::submitScore(LEADERBOARD_ID, WorldManager::getInstance()->GetGameModel()->GetScore());
#endif

	// Particles for Button
	addParticlesToButtons(m_pSaveButton); // Label	
	
	// Register Touch Event
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(GameOver::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(GameOver::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);	

	m_iUsernameCharCount = 0;
	this->scheduleUpdate();

#if (GOOGLE_ANALYTICS_ENABLED)	
	// Google Analytics
	SonarCocosHelper::GoogleAnalytics::setScreenName("Game Over Scene");
	SonarCocosHelper::GoogleAnalytics::sendEvent("Game", "Game Ended", "A game session has ended", 0);
#endif

#if (CHARTBOOST_ENABLED)
	// Show interstitial ad
	sdkbox::PluginChartboost::show("MoreApp");
#endif

#if (ADMOB_ENABLED)
	sdkbox::PluginAdMob::show("bannerad");
#endif
	// AdMob show banner ad
	//SonarCocosHelper::AdMob::showBannerAd(SonarCocosHelper::AdBannerPosition::eBottom);

	return true;
}

void GameOver::OpenLeaderboard(cocos2d::Ref *pSender)
{
	//cocos2d::Application::getInstance()->openURL("http://piratesvsaliens.azurewebsites.net/");
#if (GOOGLE_PLAY_SERVICES_ENABLED)
	SonarCocosHelper::GooglePlayServices::showLeaderboard(LEADERBOARD_ID);
#endif
}

void GameOver::UpdatePlayerStats(PlayerStats playerStats)
{
	m_PlayerStats = playerStats;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto scaleFactor = WorldManager::getInstance()->GetGameScaleFactor();

	// Labels
	auto username = Label::createWithTTF(to_string(playerStats.playerUsername), LABEL_FONT, LABEL_FONT_SIZE * scaleFactor * 2.0f);
	//username->setPosition(Vec2(origin.x + PADDING_SMALL + username->getContentSize().width / 2, visibleSize.height / 2));
	username->setTextColor(cocos2d::Color4B(232, 232, 0, 255));
	username->enableOutline(Color4B(0, 0, 0, 255), 3);
	//this->addChild(username);

	auto rank = Label::createWithTTF("Rank " + to_string(playerStats.playerRank), LABEL_FONT, LABEL_FONT_SIZE * scaleFactor);
	//rank->setPosition(Vec2(origin.x + PADDING_SMALL + rank->getContentSize().width / 2, username->getPositionY() - username->getContentSize().height / 2));
	rank->setTextColor(cocos2d::Color4B(232, 232, 0, 255));
	rank->enableOutline(Color4B(0, 0, 0, 255), 3);
	//this->addChild(rank);

	auto highscore = Label::createWithTTF("highscore " + to_string(playerStats.playerScore), LABEL_FONT, LABEL_FONT_SIZE * scaleFactor);
	//highscore->setPosition(Vec2(origin.x + PADDING_SMALL + highscore->getContentSize().width / 2, rank->getPositionY() - rank->getContentSize().height / 2));
	highscore->setTextColor(cocos2d::Color4B(255, 0, 114, 255));
	highscore->enableOutline(Color4B(0, 0, 0, 255), 3);
	//this->addChild(highscore);

	// Menu items with labels
	auto usernameItem = MenuItemLabel::create(username);
	auto rankItem = MenuItemLabel::create(rank);
	auto highscoreItem = MenuItemLabel::create(highscore);

	// Menu
	auto statsMenu = Menu::create(usernameItem, rankItem, highscoreItem, NULL);
	statsMenu->setPosition(cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 2));
	statsMenu->alignItemsVerticallyWithPadding(2);
	statsMenu->setEnabled(false);
	//this->addChild(statsMenu);

	m_pShareButton->setVisible(true);
}

void GameOver::OpenKeyboard(cocos2d::Ref *pSender)
{
	if (m_pTextField != nullptr)
	{
		m_pTextField->attachWithIME();
		//CCLOG("Keyboard opened");
	}	
}

// Call the Game Scene
void GameOver::SaveGame(cocos2d::Ref *pSender)
{
	if (WorldManager::getInstance()->GetAccessGranted())
	{		
		WorldManager::getInstance()->SetUsername(m_pTextField->getString());	
		WorldManager::getInstance()->UpdateDAO();
		WorldManager::getInstance()->QueryPlayerRank(m_pTextField->getString());
		m_pTextField->getParent()->removeChild(m_pTextField); // remove text field
		m_pTextField = nullptr;
		m_pCheckUsernameButton->setVisible(false);
		m_pSaveButton->setVisible(false);	
		m_LoginLabel->setVisible(false);
		m_ScoreLabel->setVisible(false);
	}
}

void GameOver::ShareScore(cocos2d::Ref *pSender)
{
#if (GOOGLE_ANALYTICS_ENABLED)	
	// Tweet score
	//std::string tweet = "I'm Rank " + to_string(m_PlayerStats.playerRank) + " on #PiratesVsAliens with a Highscore of " + to_string(m_PlayerStats.playerScore) + " ARRRR! Play Now! http://goo.gl/X5ckG8";
	std::string tweet = "I just scored " + to_string(WorldManager::getInstance()->GetGameModel()->GetScore()) + " on Pirates Vs Aliens ARRRR! Play Now! goo.gl/O66dXJ #PvA";
	SonarCocosHelper::Twitter::Tweet(tweet.c_str(), "Share my score", NULL);

	// Google Analytics
	SonarCocosHelper::GoogleAnalytics::sendEvent("Social", "Shared Game", "Player has shared the game to social media", 0);
#endif
}

void GameOver::update(float dt)
{	
}

void GameOver::GameRestart(cocos2d::Ref* pSender)
{
	CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	Director::getInstance()->replaceScene(TransitionFade::create(1, MenuScene::createScene()));	
}


// Adding effects to the Play Button 
void GameOver::addParticlesToButtons(MenuItemLabel* button)
{
	auto emitter = ParticleSystemQuad::create("particle/Flower.plist");
	emitter->setPosition(Point(button->getContentSize().width / 2, button->getContentSize().height / 2));
	emitter->setEmissionRate(200.00);
	emitter->setTotalParticles(40);
	emitter->setAutoRemoveOnFinish(true);
	button->addChild(emitter);
}

void GameOver::checkUsername(cocos2d::Ref *pSender)
{
	m_LoginLabel->setVisible(true);
	UpdateLoginLabel("please wait...");
	//m_LoginLabel->setColor(Color3B(200, 0, 0));
	WorldManager::getInstance()->CheckUsername(m_pTextField->getString());
}

void GameOver::TurnOnSaveButton()
{
	if (m_pTextField->getCharCount() > 2)
	{
		m_pSaveButton->setVisible(true);
	}	
}

void GameOver::TurnOffSaveButton()
{
	if (m_pTextField->getCharCount() > 2)
	{
		m_pSaveButton->setVisible(false);
	}
}

// TODO : Clean up function */
void GameOver::mainMenuCleanup()
{
	//CCLOG("Main menu cleanup");
}

// Getting the user name to save score
void GameOver::createTF()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	m_pTextField = TextFieldTTF::textFieldWithPlaceHolder("enter username", LABEL_FONT, LABEL_FONT_SIZE);
	m_pTextField->setColor(cocos2d::Color3B(232, 232, 0));
	m_pTextField->setColorSpaceHolder(cocos2d::Color3B(232, 232, 0));
	
	m_nCharLimit = 12;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)    
	// On android, TextFieldTTF cannot auto adjust its position when soft-keyboard pop up
	// So we had to set a higher position
	m_pTextField->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 50));
#else
	// Set positionY to 1/3 of screen after adding google play
	m_pTextField->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 3));
#endif	
	m_pTextField->setDelegate(this);
	//m_pTextField->attachWithIME(); // Turn off keyboard
	m_pTextField->setVisible(false);
	this->addChild(m_pTextField);
}

// Allow touches on screen
bool GameOver::onTouchBegan(Touch *pTouch, Event *pEvent)
{
	return true;
}

bool GameOver::onTextFieldAttachWithIME(TextFieldTTF * pSender)
{
	return false;
}

bool GameOver::onTextFieldDetachWithIME(TextFieldTTF * pSender)
{
	return false;
}

// Create a field to input user details
bool GameOver::onTextFieldInsertText(TextFieldTTF *pSender, const char *text, size_t nLen)
{
	//WorldManager::getInstance()->setAccessGranted(false);
	m_LoginLabel->setVisible(false);
	m_pSaveButton->setVisible(false);
	// Do not allow the user to enter incorrect characters into username 
	if (' ' == *text || '\n' == *text)
	{
		CCLOG("Incorrect character entered into username");
		return true;
	}

	// If the textfield's char count more than _charLimit, doesn't insert text anymore.
	if (pSender->getCharCount() >= m_nCharLimit)
	{
		//m_pSaveButton->setVisible(true);
		return true;
	}

	// Create a insert text sprite and do some action to add an effect
	auto label = Label::createWithSystemFont(text, LABEL_FONT, LABEL_FONT_SIZE);
	this->addChild(label);
	label->setColor(Color3B::YELLOW);

	// Move the sprite from top to position
	auto endPos = pSender->getPosition();
	if (pSender->getCharCount())
	{
		endPos.x += pSender->getContentSize().width / 2;
	}
	auto inputTextSize = label->getContentSize();
	Vec2 beginPos(endPos.x, Director::getInstance()->getWinSize().height - inputTextSize.height * 2);

	float duration = 0.5;
	label->setPosition(beginPos);
	label->setScale(8);

	auto seq = Sequence::create(
		Spawn::create(
		MoveTo::create(duration, endPos),
		ScaleTo::create(duration, 1),
		FadeOut::create(duration),
		nullptr),
		CallFuncN::create(CC_CALLBACK_1(GameOver::callbackRemoveNodeWhenDidAction, this)),
		nullptr);
	label->runAction(seq);
	m_iUsernameCharCount++;
	CCLOG("Username character count : %d", m_pTextField->getCharCount());
	// Check username
	if (m_iUsernameCharCount > 2)
	{
		//m_pCheckUsernameButton->setVisible(true);
		//m_pSaveButton->setVisible(true);
	}
	return false;
}

void GameOver::callbackRemoveNodeWhenDidAction(Node * node)
{
	this->removeChild(node, true);
}

bool GameOver::onTextFieldDeleteBackward(TextFieldTTF *pSender, const char *delText, size_t nLen)
{
	//WorldManager::getInstance()->setAccessGranted(false);
	m_LoginLabel->setVisible(false);
	m_pSaveButton->setVisible(false);
	// Create a delete text sprite and do some action
	auto label = Label::createWithSystemFont(delText, LABEL_FONT, LABEL_FONT_SIZE);
	this->addChild(label);

	// Move the sprite to fly out
	auto beginPos = pSender->getPosition();
	auto textfieldSize = pSender->getContentSize();
	auto labelSize = label->getContentSize();
	beginPos.x += (textfieldSize.width - labelSize.width) / 2.0f;

	auto winSize = Director::getInstance()->getWinSize();
	Vec2 endPos(-winSize.width / 4.0f, winSize.height * (0.5 + (float)rand() / (2.0f * RAND_MAX)));

	float duration = 1;
	float rotateDuration = 0.2f;
	int repeatTime = 5;
	label->setPosition(beginPos);

	auto seq = Sequence::create(
		Spawn::create(
		MoveTo::create(duration, endPos),
		Repeat::create(
		RotateBy::create(rotateDuration, (rand() % 2) ? 360 : -360),
		repeatTime),
		FadeOut::create(duration),
		nullptr),
		CallFuncN::create(CC_CALLBACK_1(GameOver::callbackRemoveNodeWhenDidAction, this)),
		nullptr);
	label->runAction(seq);
	m_iUsernameCharCount--;
	CCLOG("Username character count : %d", m_pTextField->getCharCount());
	if (m_iUsernameCharCount > 2)
	{
		//m_pCheckUsernameButton->setVisible(false);
		//m_pSaveButton->setVisible(true);
	}

	return false;
}

bool GameOver::onDraw(TextFieldTTF * pSender)
{
	return false;
}


static Rect getRect(Node * node)
{
	Rect rc;
	rc.origin = node->getPosition();
	rc.size = node->getContentSize();
	rc.origin.x -= rc.size.width / 2;
	rc.origin.y -= rc.size.height / 2;
	return rc;
}

void GameOver::keyboardWillShow(IMEKeyboardNotificationInfo &info)
{

}

void GameOver::keyboardWillHide(IMEKeyboardNotificationInfo &info)
{

}
