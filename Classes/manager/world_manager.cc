// Copyright (c) 2016 David Morton
#include "manager/world_manager.h"

WorldManager* WorldManager::m_Instance = nullptr;

WorldManager* WorldManager::getInstance()
{
	//Create the World Manager and return it
	if (m_Instance == nullptr)
		m_Instance = new WorldManager();
	return m_Instance;
}

WorldManager::WorldManager()
{
	if (!Init())
	{
		// Failed to initialize world manager
	}	
}

bool WorldManager::Init()
{
	CCLOG("World Manager Init()");	
	CreateDao();
	m_bAccessGranted = false;
	m_GameModel = nullptr;
	m_fDifficultiyMultiplier = 1.0f;

	return true;
}

void WorldManager::SetAccessGranted(bool value)
{
	m_bAccessGranted = value;
	if (value)
	{
		// Allow user to post highscore to remote DB
		m_GameOverScene->TurnOnSaveButton();
	}
	else if (!value)
	{		
		//m_GameOverScene->TurnOffSaveButton();
		m_GameOverScene->TurnOnSaveButton();
	}
}

void WorldManager::CreateDao()
{
	if (m_DataAccessObject == 0)
	{
		// MySQL Data Access Object
		m_DataAccessObject = std::shared_ptr<IDao>(new Dao());
	}
}

void WorldManager::UpdateDAO()
{
	// Update Remote MySQL database			
	/*
	cocos2d::network::HttpRequest* remoteRequest = new (std::nothrow) cocos2d::network::HttpRequest();
	remoteRequest->setUrl("http://grandtheftmuffins.esy.es/update_db.php/");
	remoteRequest->setRequestType(cocos2d::network::HttpRequest::Type::POST);
	remoteRequest->setResponseCallback(CC_CALLBACK_2(WorldManager::onHttpRequestCompleted, this));
	*/

	
	// Construct the request string to update the database
	cocos2d::String *remoteData = cocos2d::String::createWithFormat("playerUsername=%s&playerHighscore=%s&playerBossKills=%s&playerCoins=%s&enemiesKilled=%s&itemsCollected=%s&timePlayed=%s&numberOfGamesPlayed=1&numberOfDeaths=%s",
		WorldManager::getInstance()->GetUsername().c_str(),
		to_string(WorldManager::getInstance()->GetGameModel()->GetScore()).c_str(),
		to_string(WorldManager::getInstance()->GetGameModel()->GetBossKills()).c_str(),
		to_string(WorldManager::getInstance()->GetGameModel()->GetCoins()).c_str(),
		to_string(WorldManager::getInstance()->GetGameModel()->GetEnemiesKilled()).c_str(),
		to_string(WorldManager::getInstance()->GetGameModel()->GetItemsCollected()).c_str(),
		to_string(WorldManager::getInstance()->GetTimePlayedSeconds()).c_str(),
		to_string(WorldManager::getInstance()->GetGameModel()->GetPlayerDeaths()).c_str()
		);

	

	//CCLOG("%s", remoteData->getCString());

	/*
	remoteRequest->setRequestData(remoteData->getCString(), remoteData->length());
	cocos2d::network::HttpClient::getInstance()->send(remoteRequest);
	remoteRequest->release();
	*/

	// Send request string to the MySQL DAO	
	m_DataAccessObject->Update(remoteData->getCString());
}

void WorldManager::onHttpRequestCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
	/*
	//CCLOG("http request completed");
	if (response && response->getResponseCode() == 200 && response->getResponseData()) {
	std::vector<char> *data = response->getResponseData();
	std::string ret(&(data->front()), data->size());
	CCLOG("%s", ("Response message: " + ret).c_str());
	}
	else
	{
	CCLOG("%s", ("Error " + to_string(response->getResponseCode()) + " in request").c_str()); //error
	}
	*/
}

void WorldManager::CheckUsername(std::string username)
{
	// Check username using username_security script
	cocos2d::String *remoteData = cocos2d::String::createWithFormat("playerUsername=%s", username.c_str());
	m_DataAccessObject->Create(remoteData->getCString(), "http://piratesvsaliens.azurewebsites.net/username_security.php/");
}

void WorldManager::QueryPlayerRank(std::string username)
{
	// Check username using username_security script
	cocos2d::String *remoteData = cocos2d::String::createWithFormat("playerUsername=%s", username.c_str());
	m_DataAccessObject->Create(remoteData->getCString(), "http://piratesvsaliens.azurewebsites.net/player_rank.php/");
}

void WorldManager::Restart()
{	
	m_fDifficultiyMultiplier = 1.0f;
	m_pSpawnManager = nullptr;
	m_TiledMap = nullptr;
	m_pPathfinder = nullptr;
	m_GameModel = nullptr;
	m_vObservers.clear();
	m_vPathMarkers.clear();
	m_vBulletObjects.clear();
	m_vGameOjects.clear();
	m_vLandObjects.clear();
	m_vEnemyObjects.clear();
	m_pGameBoard = nullptr;
	m_pHudLayer = nullptr;
	m_GameScene = nullptr;
	m_BackgroundLayer = nullptr;
	SetGameState(GAME_START);
}

void WorldManager::SetGameScene(GameScene *gameScene)
{
	m_GameScene = gameScene;
}

GameScene *WorldManager::GetGameScene()
{
	return m_GameScene;
}

cocos2d::Vec2 WorldManager::GetGameArea()
{
	// Return the maximum point on the edge of the game area
	return MAX(m_MapSize.width * m_TileSize, m_MapSize.height * m_TileSize);
}

cocos2d::Vec2 WorldManager::GetCenter(cocos2d::Sprite *sprite)
{
	auto position = sprite->getPosition();
	auto contentSize = sprite->getContentSize() * m_fContentScaleFactor;
	return position + (contentSize / 2.0);
}


cocos2d::Vec2 WorldManager::ConvertTiledToScreenCoordinates(cocos2d::Vec2 position)
{
	if (m_TiledMap)
	{
		cocos2d::Vec2 convertedPosition = cocos2d::Vec2(position.x + m_TiledMap->getPositionX(), position.y + m_TiledMap->getPositionY());		
		convertedPosition.scale(m_fContentScaleFactor);
		return convertedPosition;
	}
	return cocos2d::Vec2::ZERO;
}

cocos2d::Vec2 WorldManager::ConvertScreenToTiledCoordinates(cocos2d::Vec2 position)
{
	if (m_TiledMap)
	{
		return cocos2d::Vec2(position.x - m_TiledMap->getPositionX(), position.y - m_TiledMap->getPositionY());
	}
	return cocos2d::Vec2::ZERO;
}

WorldManager::~WorldManager()
{
	//Clean up all resources created by the World Manager
	this->CleanUp();
}



void WorldManager::CleanUp()
{
	Restart();
	delete m_Instance;
	m_Instance = nullptr;
}

GameObject *WorldManager::GetSelectedObjectByType(cocos2d::Vec2 location, eObjectType type)
{
	if (type == LAND) // land
	{
		for (size_t i = 0; i < m_vLandObjects.size(); i++)
		{
			if (m_vLandObjects.at(i)->GetSprite()->getBoundingBox().containsPoint(location) && m_vLandObjects.at(i)->GetType() == type && m_vLandObjects.at(i)->GetState() == ALIVE)
			{
				return m_vLandObjects.at(i).get();
			}
		}
	}
	else if (type == CANNON || type == PLATFORM || type == TOWER)// platforms and cannons/towers
	{
		for (size_t i = 0; i < m_vGameOjects.size(); i++)
		{
			if (m_vGameOjects.at(i)->GetType() == type && m_vGameOjects.at(i)->GetState() == ALIVE)
			{
				if (m_vGameOjects.at(i)->GetSprite()->getBoundingBox().containsPoint(location))
				{
					return m_vGameOjects.at(i).get();
				}
			}			
		}
	}	
	return nullptr;
}

void WorldManager::SetPathfinder(ai::AStar *pathfinder)
{
	m_pPathfinder = pathfinder;
	m_BackgroundLayer = m_TiledMap->getLayer("Background");
	cocos2d::Size layerSize = m_BackgroundLayer->getLayerSize();	

	// Set up the game world initially based off hard coded obstacles set in tilemap
	for (int x = 0; x < layerSize.width; ++x)
	{
		for (int z = 0; z < layerSize.height; ++z)
		{
			int gid = m_BackgroundLayer->getTileGIDAt(cocos2d::Vec2(x, z));
			if (gid)
			{
				cocos2d::ValueMap properties = m_TiledMap->getPropertiesForGID(gid).asValueMap();
				if (!properties.empty()) {
					std::string blocked = properties["Blocked"].asString();
					if ("True" == blocked) {
						// Send blocked cell to RAIG
						m_pPathfinder->GetGameWorld()->SetCellState(x, 0, z, base::GameWorld::CELL_BLOCKED);

						auto position = WorldManager::getInstance()->GetSpriteAtTilePosition(cocos2d::Vec2(x, z))->getPosition();
						Notify(GameEvent::create(LAND_BUILD, position));
					}
				}
			}
		}
	}

	UpdatePath();
}

ai::AStar *WorldManager::GetPathfinder()
{
	return m_pPathfinder;
}


void WorldManager::SetSpawnManager(SpawnManager *spawnManager)
{
	m_pSpawnManager = spawnManager;
}

SpawnManager *WorldManager::GetSpawnManager()
{
	return m_pSpawnManager;
}

void WorldManager::Update(float dt)
{
	if (m_eGameState != GAME_OVER)
	{
		// Game Objects	
		for (size_t i = 0; i < m_vGameOjects.size(); i++)
		{
			m_vGameOjects.at(i)->Update(dt);			
		}


		// Enemies
		for (size_t i = 0; i < m_vEnemyObjects.size(); i++)
		{
			m_vEnemyObjects.at(i)->Update(dt);			
		}
		

		// Garbage collector
		// Use one for loop to remove all dead game objects in all vectors
		// Get the largest index and loop from 0 to largest		
		ssize_t largestIndex = m_vGameOjects.size();
		if (m_vEnemyObjects.size() > largestIndex)
		{
			largestIndex = m_vEnemyObjects.size();
		}
		if (m_vBulletObjects.size() > largestIndex)
		{
			largestIndex = m_vBulletObjects.size();
		}

		// One master loop
		for (ssize_t i = 0; i < largestIndex; i++)
		{
			if (i < m_vGameOjects.size())
			{
				if (m_vGameOjects.at(i)->GetState() == DEAD)
				{
					m_vGameOjects.erase(m_vGameOjects.begin() + i);
					//CCLOG("Number of game objects %d", m_vGameOjects.size());
				}
			}	

			if (i < m_vEnemyObjects.size())
			{
				if (m_vEnemyObjects.at(i)->GetState() == DEAD)
				{
					m_vEnemyObjects.erase(m_vEnemyObjects.begin() + i);
					//CCLOG("Number of enemy objects in game %d", m_vEnemyObjects.size());
				}
			}

			if (i < m_vBulletObjects.size())
			{
				if (m_vBulletObjects.at(i)->GetSprite()->isVisible() == false)
				{
					m_vBulletObjects.at(i)->GetSprite()->getParent()->removeChild(m_vBulletObjects.at(i)->GetSprite());
					m_vBulletObjects.erase(m_vBulletObjects.begin() + i);
					//CCLOG("Number of bullets in game %d", m_vBulletObjects.size());
				}
			}

			/*
			if (i < m_vBulletObjects.size())
			{
				if (m_vBulletObjects.at(i)->isVisible() == false)
				{
					m_vBulletObjects.at(i)->getParent()->removeChild(m_vBulletObjects.at(i));
					m_vBulletObjects.erase(m_vBulletObjects.begin() + i);
					//CCLOG("Number of bullets in game %d", m_vBulletObjects.size());
				}
			}
			*/
		}		

		// Collision detection
		CheckForCollisions(dt);
	}
}

void WorldManager::CheckForCollisions(float dt)
{
	for (size_t enemyIndex = 0; enemyIndex < m_vEnemyObjects.size(); enemyIndex++)
	{
		for (size_t bulletIndex = 0; bulletIndex < m_vBulletObjects.size(); bulletIndex++)
		{
			if (m_vEnemyObjects.at(enemyIndex)->GetState() == ALIVE)
			{
				if (m_vEnemyObjects.at(enemyIndex)->GetSprite()->getBoundingBox().intersectsRect(m_vBulletObjects.at(bulletIndex)->GetSprite()->getBoundingBox()))
				{
					//m_vEnemyObjects.at(enemyIndex)->SetState(DEAD);
					//WorldManager::getInstance()->Notify(GameEvent::create(ENEMY_DESTROYED, m_vEnemyObjects.at(enemyIndex)->GetSprite()->getPosition()));

					m_vEnemyObjects.at(enemyIndex)->TakeDamage(m_vBulletObjects.at(bulletIndex)->GetAttackStrength());
					//m_vBulletObjects.at(bulletIndex)->setVisible(false);
					m_vBulletObjects.at(bulletIndex)->GetSprite()->setVisible(false);

				}
			}					
		}
	}	
}

void WorldManager::ScaleSprite(cocos2d::Sprite *sprite)
{
	sprite->setScale(m_fContentScaleFactor);
}


void WorldManager::RegisterWithWorldManger(std::shared_ptr<GameObject> pGameObj)
{ 
	if (pGameObj->GetType() == ENEMY)
	{
		m_vEnemyObjects.push_back(std::dynamic_pointer_cast<Enemy>(pGameObj));
	}
	else if (pGameObj->GetType() == CANNON || pGameObj->GetType() == PLATFORM || pGameObj->GetType() == TOWER)
	{
		m_vGameOjects.push_back(pGameObj);
	}	
	else if(pGameObj->GetType() == LAND)
	{
		m_vLandObjects.push_back(std::dynamic_pointer_cast<Land>(pGameObj));
	}
}

void WorldManager::AddBulletToList(std::shared_ptr<Bullet> bullet)
{
	m_vBulletObjects.push_back(bullet);
}


/*
void WorldManager::AddBulletToList(cocos2d::Sprite *bullet)
{
	m_vBulletObjects.push_back(bullet);
}
*/
std::vector<std::shared_ptr<GameObject>> *WorldManager::GetGameObjects()
{ 
	return &m_vGameOjects;
}

std::vector<std::shared_ptr<Enemy>> *WorldManager::GetEnemyObjects()
{
	return &m_vEnemyObjects;
}

/*
std::vector<cocos2d::Sprite*> *WorldManager::GetBulletObjects()
{
	return &m_vBulletObjects;
}
*/

std::vector<std::shared_ptr<Bullet>> *WorldManager::GetBulletObjects()
{
	return &m_vBulletObjects;
}


// Target position
void WorldManager::SetEnemyTargetPosition(cocos2d::Vec2 target)
{	
	m_EnemyTargetPosition = target;
	m_EnemyTargetPosition.scale(m_fContentScaleFactor);
}

cocos2d::Vec2 WorldManager::GetEnemyTargetPosition()
{ 
	return m_EnemyTargetPosition;
}

// Converts the target sprites coordinates to game coordinates using tile coordinates e.g (4, 6)
cocos2d::Vec2 WorldManager::GetEnemyTargetSpritePosition(cocos2d::Vec2 position)
{
	auto enemyTarget = WorldManager::getInstance()->GetSpriteAtTilePosition(position);

	// Convert target sprites position
	auto enemyTargetPosition = enemyTarget->getPosition();
	enemyTargetPosition = ConvertTiledToScreenCoordinates(enemyTargetPosition);	
	return enemyTargetPosition;
}

cocos2d::Sprite *WorldManager::GetSpriteAtTilePosition(cocos2d::Vec2 position)
{
	return m_BackgroundLayer->getTileAt(position);
}

// Spawn position
void WorldManager::SetEnemySpawnPosition(cocos2d::Vec2 position)
{	
	m_EnemySpawnPosition = position;
	m_EnemySpawnPosition.scale(m_fContentScaleFactor);
}

cocos2d::Vec2 WorldManager::GetEnemySpawnPosition()
{
	return m_EnemySpawnPosition;
}

// Game board
void WorldManager::SetGameBoard(BoardLayer *board)
{
	m_pGameBoard = board;
}

BoardLayer *WorldManager::GetGameBoard()
{
	return m_pGameBoard;
}

// HUD
void WorldManager::SetHudLayer(HudLayer *hudlayer)
{
	m_pHudLayer = hudlayer;
}

HudLayer *WorldManager::GetHudLayer()
{
	return m_pHudLayer;
}
void WorldManager::SetTiledMap(cocos2d::TMXTiledMap *tiledMap)
{
	m_TiledMap = tiledMap;
	m_TileSize = m_TiledMap->getTileSize();
	m_MapSize = m_TiledMap->getMapSize();
	// Set content scale factor
	m_VisibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	SetGameScaleFactor(m_VisibleSize.height / m_TiledMap->getContentSize().height);	
}

void WorldManager::SetGameScaleFactor(float gameScaleFactor)
{
	m_fContentScaleFactor = gameScaleFactor;
}

float WorldManager::GetGameScaleFactor()
{
	return m_fContentScaleFactor;
}

cocos2d::Vec2 WorldManager::PositionToTileCoordinates(cocos2d::Vec2 position)
{	
	int x = position.x / m_TileSize.width;
	int y = ((m_MapSize.height * m_TileSize.height) - position.y) / m_TileSize.height;
	return cocos2d::Vec2(x, y);
}

void WorldManager::UpdatePath()
{
	// Convert to tile coordinates
	cocos2d::Vec2 spawnPos = PositionToTileCoordinates(m_EnemySpawnPosition);
	cocos2d::Vec2 targetPos = PositionToTileCoordinates(m_EnemyTargetPosition);

	// Create shared pointers to vectors
	std::shared_ptr<base::Vector3> spawnPosition(new base::Vector3(spawnPos.x, 0, spawnPos.y));
	std::shared_ptr<base::Vector3> targetPosition(new base::Vector3(targetPos.x, 0, targetPos.y));
		
	// Calculate path
	m_pPathfinder->FindPath(spawnPosition, targetPosition);	

	if (m_pPathfinder->GetState() == ai::AStar::REQUEST_COMPLETE)
	{		
		m_vPathToGoal = m_pPathfinder->CopyPathToGoal();
		std::reverse(m_vPathToGoal.begin(), m_vPathToGoal.end());
		
		// Once RAIG has returned a path loop the vector and color the cells in the path
		//int pathColorR = rand() % (255 + 1 - 0) + 0;
		//int pathColorG = rand() % (255 + 1 - 0) + 0;
		//int pathColorB = rand() % (255 + 1 - 0) + 0;
		
		for (auto node = m_vPathMarkers.begin(); node != m_vPathMarkers.end(); ++node)
		{
			(*node)->getParent()->removeChild((*node));
		}
		m_vPathMarkers.clear();

		for (auto node = m_vPathToGoal.begin(); node != m_vPathToGoal.end(); ++node)
		{
			int x = (*node)->m_iX;
			int z = (*node)->m_iZ;
			//auto tile = m_BackgroundLayer->getTileAt(cocos2d::Vec2(x, z));
			//tile->setColor(cocos2d::Color3B(pathColorR, pathColorG, pathColorB));

			cocos2d::Sprite *nodeMarker = cocos2d::Sprite::create("objects/pathMarker.png");
			nodeMarker->setAnchorPoint(cocos2d::Vec2::ZERO);
			auto fadeInOut = cocos2d::Sequence::create(cocos2d::FadeOut::create(0.5f), cocos2d::FadeIn::create(0.5f), NULL);
			nodeMarker->runAction(cocos2d::RepeatForever::create(fadeInOut));
			auto position = WorldManager::getInstance()->GetSpriteAtTilePosition(cocos2d::Vec2(x, z))->getPosition();			
			nodeMarker->setPosition(ConvertTiledToScreenCoordinates(position));
			ScaleSprite(nodeMarker);
			m_pGameBoard->addChild(nodeMarker);
			m_vPathMarkers.push_back(nodeMarker);
		}
		m_bIsAllowedToBuild = true;
	}
	else
	{
		CCLOG("Could not create new path! Dont allow platform to be built there");
		m_bIsAllowedToBuild = false;
		m_pPathfinder->ResetPath();
	}
}

void WorldManager::WriteToLogFile(const char *message)
{
#if (_WIN32 && OUTPUT_LOGFILE)
	FILE *logfile = fopen("logfile.txt", "a");
	fprintf(logfile, "%s", message);
	fclose(logfile);
#endif
}

void WorldManager::WriteToLogFile(std::stringstream &message)
{
#if (_WIN32 && OUTPUT_LOGFILE)
	FILE *logfile = fopen("logfile.txt", "a");
	fprintf(logfile, "%s", message.str().c_str());
	message.str("");
	fclose(logfile);
#endif
}
