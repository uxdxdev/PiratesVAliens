/*

Copyright (c) 2016 David Morton

*/
#include "layer/board_layer.h"
#include "manager/world_manager.h"
#include "entities/enemy.h"
#include "entities/tower_platform.h"
#include "entities/cannon.h"
#include "entities/tower.h"
#include "entities/land.h"
#include "audio/include/SimpleAudioEngine.h"
#include "scene/game_over_scene.h"
#include "scene/menu_scene.h"

USING_NS_CC;

BoardLayer::~BoardLayer()
{	
	m_TiledMap = nullptr;
	m_pSpawnManager = nullptr;
	m_pPathfinder = nullptr;
	m_GameModel = nullptr;
}

bool BoardLayer::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}
		
	m_WaveEndBell = "audio/WaveEndBell" + std::string(MUSIC_EXTENSION);
	m_WaveEndComms = "audio/AlienWaveAudio" + std::string(MUSIC_EXTENSION);
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(m_WaveEndBell.c_str());
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(m_WaveEndComms.c_str());

	// Register with world manager
	WorldManager::getInstance()->SetGameBoard(this);
	WorldManager::getInstance()->AddObserver(this);	

	// Register for touch events
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(BoardLayer::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(BoardLayer::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	// Screen dimensions
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
			
	
	// Spawn manager
	std::shared_ptr<SpawnManager> spawnManager(new SpawnManager());
	m_pSpawnManager = std::move(spawnManager);
	WorldManager::getInstance()->SetSpawnManager(m_pSpawnManager.get());

	// Tilemap	
	m_TiledMap = TMXTiledMap::create("tilemap/tilemap.tmx");	
	
	// Scale tilemap content (visible width / tilemap content width), this is effected by the design resolution
	float tilemapContentScaleFactor = visibleSize.height / m_TiledMap->getContentSize().height;
	m_TiledMap->setScale(tilemapContentScaleFactor);

	// Scale the tilemap dimensions (visible width / (tile width * number of tiles))
	float tilemapScaleFactor = visibleSize.height / (m_TiledMap->getTileSize().height * m_TiledMap->getMapSize().height);
	m_TiledMap->setTileSize(m_TiledMap->getTileSize() * tilemapScaleFactor);

	m_TileSize = m_TiledMap->getTileSize();
	m_MapSize = m_TiledMap->getMapSize();
	this->addChild(m_TiledMap, 1);	

	WorldManager::getInstance()->SetTiledMap(m_TiledMap);
	
	// Tilemap objects
	TMXObjectGroup *objectGroup = m_TiledMap->getObjectGroup("Objects");
	if (objectGroup == NULL){
		CCLOG("tile map has no objects object layer");
		return false;
	}

	// Set spawn point
	auto spawnPoint = objectGroup->getObject("SpawnPoint");
	int x = spawnPoint["x"].asInt();
	int y = spawnPoint["y"].asInt();
	WorldManager::getInstance()->SetEnemySpawnPosition(cocos2d::Vec2(x, y));
	
	// Set target position
	auto targetPoint = objectGroup->getObject("EnemyTarget");
	x = targetPoint["x"].asInt();
	y = targetPoint["y"].asInt();
	WorldManager::getInstance()->SetEnemyTargetPosition(cocos2d::Vec2(x, y));
	
	// Pathfinder
	std::shared_ptr<ai::AStar> pathFinder(new ai::AStar(m_MapSize.width, m_MapSize.height));
	m_pPathfinder = std::move(pathFinder);
	WorldManager::getInstance()->SetPathfinder(m_pPathfinder.get());
		
	// Particle effects for spawnpoint
	cocos2d::ParticleSystemQuad* particleSystem = cocos2d::ParticleSystemQuad::create("particle/EnemyPortal.plist");
	particleSystem->setAutoRemoveOnFinish(true);
	particleSystem->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));

	// Position touch sprite in game
	cocos2d::Vec2 gameWorldCell = WorldManager::getInstance()->PositionToTileCoordinates(WorldManager::getInstance()->GetEnemySpawnPosition());
	auto position = WorldManager::getInstance()->GetSpriteAtTilePosition(gameWorldCell)->getPosition();
	auto convertedPosition = WorldManager::getInstance()->ConvertTiledToScreenCoordinates(position); // At (0,0 of sprite)
	particleSystem->setPosition(convertedPosition.x + m_TileSize.width / 2, convertedPosition.y + m_TileSize.height / 2);
	
	this->addChild(particleSystem, 2);

	// Get outer most point of game area for touch location check
	m_GameArea = WorldManager::getInstance()->GetGameArea();

	// Game model
	m_GameModel = WorldManager::getInstance()->GetGameModel();

	m_TouchSprite = cocos2d::Sprite::create("objects/touch.png");
	m_TouchSprite->setAnchorPoint(cocos2d::Vec2::ZERO);
	WorldManager::getInstance()->ScaleSprite(m_TouchSprite);	
	this->addChild(m_TouchSprite, 5);

	// Build Menu
	m_BuildMenu = BuildMenuLayer::create();
	m_BuildMenu->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
	m_BuildMenu->setVisible(false);	
	this->addChild(m_BuildMenu, 10); // needs to be displayed above everything else.

	// Upgrade Menu
	m_UpgradeMenu = UpgradeMenuLayer::create();
	m_UpgradeMenu->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
	m_UpgradeMenu->setVisible(false);
	this->addChild(m_UpgradeMenu, 10); // needs to be displayed above everything else.
	
		// load and cache the texture and sprite frames
	auto cacher = cocos2d::SpriteFrameCache::getInstance();
	cacher->addSpriteFramesWithFile("spritesheet/spritesheet.plist");

	// Game session timer
	m_iGameTime = 0;
	WorldManager::getInstance()->SetTimePlayedSeconds(m_iGameTime);
	this->schedule(schedule_selector(BoardLayer::UpdateTimer), 1.0f);

#if (GOOGLE_ANALYTICS_ENABLED)
	SonarCocosHelper::GoogleAnalytics::setScreenName("Board Layer");
	SonarCocosHelper::GoogleAnalytics::sendEvent("Game", "Board View", "Board layer initialized", 0);
#endif
		
	return true;
}

// Updates Game Timer
void BoardLayer::UpdateTimer(float dt)
{
	if (WorldManager::getInstance()->GetGameState() != GAME_OVER)
	{
		m_iGameTime++;
		//CCLOG("Seconds in game %d", m_iGameTime);
	}
	//CCLOG("Updating game timer : %d seconds", m_nGameTime);
}

bool BoardLayer::onTouchBegan(Touch* touch, Event* event)
{	
	return true;
}

bool BoardLayer::IsInsideGameArea(cocos2d::Vec2 touchLocation)
{
	if (touchLocation.x < m_GameArea.x && touchLocation.y < m_GameArea.y)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void BoardLayer::onTouchEnded(Touch* touch, Event* event)
{	
	m_UpgradeMenu->CloseMenu();
	m_BuildMenu->CloseMenu();

	if (WorldManager::getInstance()->GetGameState() == GAME_OVER)
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
		Director::getInstance()->replaceScene(TransitionFade::create(1, GameOver::createScene()));
	}

	if (WorldManager::getInstance()->GetGameScene()->IsPaused() == false)
	{
		if (IsInsideGameArea(touch->getLocation()))
		{
			cocos2d::Vec2 touchCoordinates = touch->getLocation();
			
			// Position touch sprite in game
			cocos2d::Vec2 gameWorldCell = WorldManager::getInstance()->PositionToTileCoordinates(touchCoordinates);
			auto position = WorldManager::getInstance()->GetSpriteAtTilePosition(gameWorldCell)->getPosition();
			m_TouchSprite->setPosition(WorldManager::getInstance()->ConvertTiledToScreenCoordinates(position));

			//CCLOG("Touch (%f, %f)", touchCoordinates.x, touchCoordinates.y);
			// In build mode
			if (WorldManager::getInstance()->GetHudLayer()->IsBuildingPlatforms()) // BUILDING PLATFORMS
			{
				// Check for land
				Land *land = (Land*)WorldManager::getInstance()->GetSelectedObjectByType(touchCoordinates, LAND);
				if (land != nullptr)
				{
					// Check for land type
					if (land->GetType() == LAND)
					{
						// land here, cant build here
						return;
					}
				}
				else
				{
					// no land check for platforms
					TowerPlatform *platform = (TowerPlatform*)WorldManager::getInstance()->GetSelectedObjectByType(touchCoordinates, PLATFORM);
					if (platform != nullptr)
					{
						// Check for platform type
						if (platform->GetType() == PLATFORM)
						{
							// Platform exists in that position, remove it
							if (platform->HasTower() == false)
							{
								
								WorldManager::getInstance()->Notify(GameEvent::create(PLATFORM_DESTROY, touchCoordinates));																
								return;
							}
						}
					}
					else
					{
						// Check game model for number of current platforms
						if (m_GameModel->GetCurrentPlatforms() < m_GameModel->GetMaxPlatforms())
						{
							// No platforms at touch location				
							WorldManager::getInstance()->Notify(GameEvent::create(PLATFORM_BUILD, touchCoordinates));
						}
						else
						{
							auto sequence = Sequence::create(ScaleTo::create(0.5f, 1.5f), ScaleTo::create(0.5f, 1.0f), nullptr);
							WorldManager::getInstance()->GetHudLayer()->GetPlatformLabel()->runAction(sequence);
						}
					}
				}
			}
			else if (!WorldManager::getInstance()->GetHudLayer()->IsBuildingPlatforms()) // BUILDING TOWERS
			{
				// Check for towers/cannons
				Cannon *cannon = (Cannon*)WorldManager::getInstance()->GetSelectedObjectByType(touchCoordinates, CANNON);
				if (cannon != nullptr)
				{
					// check for types of cannons/towers
					if (cannon->GetType() == CANNON)
					{
						// open upgrade/sell menu for tower
						m_UpgradeMenu->UpgradeSellObject(touchCoordinates, CANNON, cannon);

						/*
						// for now just destroy it
						WorldManager::getInstance()->Notify(GameEvent::create(CANNON_DESTROYED, touchCoordinates));
						m_GameModel->SetCoins(m_GameModel->GetCoins() + (CANNON_COST / 2));
						WorldManager::getInstance()->Notify(GameEvent::create(UPDATE_STATS));
						*/
						return;
					}
				}

				// Towers
				Tower *tower = (Tower*)WorldManager::getInstance()->GetSelectedObjectByType(touchCoordinates, TOWER);
				if (tower != nullptr)
				{
					// check for types of cannons/towers
					if (tower->GetType() == TOWER)
					{
						// open upgrade/sell menu for tower
						m_UpgradeMenu->UpgradeSellObject(touchCoordinates, TOWER, tower);

						/*
						// for now just destroy it
						WorldManager::getInstance()->Notify(GameEvent::create(CANNON_DESTROYED, touchCoordinates));
						m_GameModel->SetCoins(m_GameModel->GetCoins() + (CANNON_COST / 2));
						WorldManager::getInstance()->Notify(GameEvent::create(UPDATE_STATS));
						*/
						return;
					}
				}				



				// No cannons, check for platforms
				TowerPlatform *platform = (TowerPlatform*)WorldManager::getInstance()->GetSelectedObjectByType(touchCoordinates, PLATFORM);
				if (platform != nullptr)
				{
					if (platform->GetType() == PLATFORM)
					{
						if (platform->HasTower() == false)
						{
							// Display build menu							
							m_BuildMenu->BuildLocation(touchCoordinates, PLATFORM);

							/*
							if (m_GameModel->GetCoins() >= CANNON_COST)
							{
								m_GameModel->SetCoins(m_GameModel->GetCoins() - CANNON_COST);
								WorldManager::getInstance()->Notify(GameEvent::create(UPDATE_STATS));

								// Platform exists in that position with no tower on it, open build tower menu								
								WorldManager::getInstance()->Notify(GameEvent::create(CANNON_BUILD, WorldManager::getInstance()->GetCenter(platform->GetSprite())));
								platform->TowerOnArea(true);
								return;
							}
							else
							{
								WorldManager::getInstance()->Notify(GameEvent::create(RESOURCES_TOO_LOW));
							}
							*/
						}
					}
				}

				// No platforms check for land
				Land *land = (Land*)WorldManager::getInstance()->GetSelectedObjectByType(touchCoordinates, LAND);
				if (land != nullptr)
				{
					if (land->GetType() == LAND)
					{
						if (land->HasTower() == false)
						{
							// Display build menu							
							m_BuildMenu->BuildLocation(touchCoordinates, LAND);

							/*
							if (m_GameModel->GetCoins() >= CANNON_COST)
							{
								m_GameModel->SetCoins(m_GameModel->GetCoins() - CANNON_COST);
								WorldManager::getInstance()->Notify(GameEvent::create(UPDATE_STATS));

								// Platform exists in that position with no tower on it, open build tower menu								
								WorldManager::getInstance()->Notify(GameEvent::create(CANNON_BUILD, WorldManager::getInstance()->GetCenter(land->GetSprite())));
								land->TowerOnArea(true);
								return;
							}
							else
							{
								WorldManager::getInstance()->Notify(GameEvent::create(RESOURCES_TOO_LOW));
							}
							*/
							return;
						}
					}
				}
			}
		}
	}
}

void BoardLayer::OnNotify(std::shared_ptr<GameEvent> tEvent)
{			
	if (tEvent->GetEventType() == START_SPAWNING)
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(m_WaveEndBell.c_str());
		m_AlienWaveAudionId = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(m_WaveEndComms.c_str());
		this->schedule(schedule_selector(BoardLayer::WaveTimer), 0.5f);
	}
	else if (tEvent->GetEventType() == STOP_SPAWNING)
	{		
		this->unschedule(schedule_selector(BoardLayer::WaveTimer));			
	}	
	else if (tEvent->GetEventType() == WAVE_END)
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(m_WaveEndBell.c_str());
		CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(m_AlienWaveAudionId);
	}
	else if (tEvent->GetEventType() == GAME_OVER)
	{		
		// Time played in game		
		WorldManager::getInstance()->SetTimePlayedSeconds(m_iGameTime);
		CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(m_AlienWaveAudionId);		
		WorldManager::getInstance()->GetHudLayer()->GameOverAnimation();
	}	
}

void BoardLayer::update(float dt)
{	
	WorldManager::getInstance()->Update(dt);		
}

void BoardLayer::WaveTimer(float dt)
{	
	// Create a random enemy, 4 enemies

	if (m_GameModel->GetCurrentWave() < (m_GameModel->GetMaxWaves() - 1))
	{
		int maxEnemyIndex = WorldManager::getInstance()->GetGameModel()->GetCurrentWave();
		if (maxEnemyIndex >= 3)
		{
			maxEnemyIndex = 3;
		}

		// rand() % (max_number + 1 - minimum_number)) + minimum_number;
		int enemyType = rand() % (maxEnemyIndex + 1 - 0) + 0;
		WorldManager::getInstance()->Notify(GameEvent::create(ENEMY_SPAWNED, WorldManager::getInstance()->GetEnemySpawnPosition(), enemyType));
	}
	else
	{
		int maxEnemyIndex = WorldManager::getInstance()->GetGameModel()->GetCurrentWave();
		if (maxEnemyIndex >= 3)
		{
			maxEnemyIndex = 3;
		}

		// rand() % (max_number + 1 - minimum_number)) + minimum_number;
		int enemyType = rand() % (maxEnemyIndex + 1 - 0) + 0;
		WorldManager::getInstance()->Notify(GameEvent::create(ENEMY_SPAWNED, WorldManager::getInstance()->GetEnemySpawnPosition(), enemyType));
				
		if (WorldManager::getInstance()->GetSpawnManager()->GetNumberOfSpawnedEnemies() == WorldManager::getInstance()->GetGameModel()->GetWaveEnemyLimit() - 1)
		{
			// Spawn the boss
			WorldManager::getInstance()->Notify(GameEvent::create(ENEMY_SPAWNED, WorldManager::getInstance()->GetEnemySpawnPosition(), ENEMY_BOSS));
		}
		
	}
}

