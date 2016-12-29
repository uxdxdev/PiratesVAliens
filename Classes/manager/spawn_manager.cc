/*

Copyright (c) 2016 David Morton

*/
#include "manager/spawn_manager.h"

#include "manager/world_manager.h"
#include "entities/tower_platform.h"
#include "entities/enemy.h"
#include "entities/land.h"
#include "entities/cannon.h"
#include "entities/tower.h"
#include "base/game_event.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "SonarCocosHelperCPP/SonarFrameworks.h"
#endif
SpawnManager::SpawnManager()
{
	WorldManager::getInstance()->AddObserver(this);
	m_iEnemiesSpawned = 0;
	m_iEnemiesDestroyed = 0;
	m_VisibleSize = cocos2d::Director::getInstance()->getVisibleSize();	
}

void SpawnManager::BuildPlatform(cocos2d::Vec2 position)
{	
	std::shared_ptr<TowerPlatform> platform(new TowerPlatform("objects/platform.png"));
	platform->GetSprite()->setPosition(WorldManager::getInstance()->ConvertTiledToScreenCoordinates(position));
	WorldManager::getInstance()->RegisterWithWorldManger(platform);		

	// scale sprite image
	WorldManager::getInstance()->ScaleSprite(platform->GetSprite());

	WorldManager::getInstance()->GetGameModel()->SetCurrentPlatforms(WorldManager::getInstance()->GetGameModel()->GetCurrentPlatforms() + 1);
	WorldManager::getInstance()->Notify(GameEvent::create(UPDATE_STATS));
}

void SpawnManager::SpawnEnemy(cocos2d::Vec2 position, eEnemyType type)
{
	std::string enemyFilename;
	if (type == ENEMY_BLUE)
	{
		//enemyFilename = "enemies/enemyBlue";
		enemyFilename = "enemyBlue";
	}
	else if (type == ENEMY_RED)
	{
		enemyFilename = "enemyRed";
	}
	else if (type == ENEMY_GREEN)
	{
		enemyFilename = "enemyGreen";
	}
	else if (type == ENEMY_YELLOW)
	{
		enemyFilename = "enemyYellow";
	}
	else if (type == ENEMY_BOSS)
	{
		enemyFilename = "enemyBoss";
	}

	std::shared_ptr<Enemy> enemy(new Enemy(enemyFilename, type));
	// scale sprite image
	WorldManager::getInstance()->ScaleSprite(enemy->GetSprite());

	enemy->GetSprite()->setPosition(WorldManager::getInstance()->ConvertTiledToScreenCoordinates(position));
	WorldManager::getInstance()->RegisterWithWorldManger(enemy);
}

void SpawnManager::CreateLand(cocos2d::Vec2 position)
{
	std::shared_ptr<Land> landMass(new Land("objects/land.png"));
	// scale sprite image
	WorldManager::getInstance()->ScaleSprite(landMass->GetSprite());

	// Convert tiled coordinates to screen coordinates
	landMass->GetSprite()->setPosition(WorldManager::getInstance()->ConvertTiledToScreenCoordinates(position));
	WorldManager::getInstance()->RegisterWithWorldManger(landMass);	
}

void SpawnManager::BuildCannon(cocos2d::Vec2 position)
{
	std::shared_ptr<Cannon> cannon(new Cannon("objects/cannonTop.png"));
	// scale sprite image
	WorldManager::getInstance()->ScaleSprite(cannon->GetSprite());

	// Convert tiled coordinates to screen coordinates	
	cannon->GetSprite()->setPosition(position);	
	WorldManager::getInstance()->RegisterWithWorldManger(cannon);	
}

void SpawnManager::BuildTower(cocos2d::Vec2 position)
{
	std::shared_ptr<Tower> tower(new Tower("objects/towerTop.png", TOWER));
	// scale sprite image
	WorldManager::getInstance()->ScaleSprite(tower->GetSprite());

	// Convert tiled coordinates to screen coordinates	
	tower->GetSprite()->setPosition(position);
	WorldManager::getInstance()->RegisterWithWorldManger(tower);
}

int SpawnManager::GetNumberOfSpawnedEnemies()
{	
	return m_iEnemiesSpawned;
}

void SpawnManager::OnNotify(std::shared_ptr<GameEvent> tEvent)
{
	eEventType eventType = tEvent->GetEventType();

	if (eventType == PLATFORM_BUILD)
	{
		// Convert to tile coordinates
		cocos2d::Vec2 gameWorldCell = WorldManager::getInstance()->PositionToTileCoordinates(tEvent->GetLocation());
		WorldManager::getInstance()->GetPathfinder()->GetGameWorld()->SetCellState(gameWorldCell.x, 0, gameWorldCell.y, base::GameWorld::CELL_BLOCKED);
		WorldManager::getInstance()->UpdatePath();

		if (WorldManager::getInstance()->IsAllowedToBuild())
		{
			auto position = WorldManager::getInstance()->GetSpriteAtTilePosition(gameWorldCell)->getPosition();
			BuildPlatform(position);
		}
		else
		{
			WorldManager::getInstance()->GetPathfinder()->GetGameWorld()->SetCellState(gameWorldCell.x, 0, gameWorldCell.y, base::GameWorld::CELL_OPEN);
		}
	}
	else if (eventType == PLATFORM_DESTROY)
	{
		// Check for platforms at location
		TowerPlatform *platform = (TowerPlatform *)WorldManager::getInstance()->GetSelectedObjectByType(tEvent->GetLocation(), PLATFORM); // find the destroyed game object
		if (platform != nullptr)
		{
			if (platform->GetType() == PLATFORM)
			{
				platform->SetState(DEAD); // remove from game

				// Convert to tilemap coordinates
				cocos2d::Vec2 gameWorldCell = WorldManager::getInstance()->PositionToTileCoordinates(tEvent->GetLocation());
				WorldManager::getInstance()->GetPathfinder()->GetGameWorld()->SetCellState(gameWorldCell.x, 0, gameWorldCell.y, base::GameWorld::CELL_OPEN); // open cell in path finder
				WorldManager::getInstance()->UpdatePath(); // get a new path

				// Update game model
				WorldManager::getInstance()->GetGameModel()->SetCurrentPlatforms(WorldManager::getInstance()->GetGameModel()->GetCurrentPlatforms() - 1);
				WorldManager::getInstance()->Notify(GameEvent::create(UPDATE_STATS));
			}
		}			
	}
	else if (eventType == LAND_BUILD)
	{
		CreateLand(tEvent->GetLocation());
	}
	else if (eventType == ENEMY_SPAWNED)
	{			
		cocos2d::Vec2 gameWorldCell = WorldManager::getInstance()->PositionToTileCoordinates(tEvent->GetLocation());
		auto position = WorldManager::getInstance()->GetSpriteAtTilePosition(gameWorldCell)->getPosition();
		SpawnEnemy(position, (eEnemyType)tEvent->GetValue());
		m_iEnemiesSpawned++;		
		if (m_iEnemiesSpawned == WorldManager::getInstance()->GetGameModel()->GetWaveEnemyLimit())
		{
			WorldManager::getInstance()->Notify(GameEvent::create(STOP_SPAWNING, WorldManager::getInstance()->GetEnemySpawnPosition()));
			m_iEnemiesSpawned = 0;
		}
	}
	else if (eventType == ENEMY_DESTROYED)
	{
		// Play particle effect at location?
		m_iEnemiesDestroyed++;
		if (m_iEnemiesDestroyed == WorldManager::getInstance()->GetGameModel()->GetWaveEnemyLimit())
		{
			WorldManager::getInstance()->Notify(GameEvent::create(WAVE_END, WorldManager::getInstance()->GetEnemySpawnPosition()));
			m_iEnemiesDestroyed = 0;
		}
	}
	else if (eventType == CANNON_BUILD)
	{
		BuildCannon(tEvent->GetLocation());
	}	
	else if (eventType == CANNON_DESTROYED)
	{
		// Check for cannons at location
		Cannon *cannon = (Cannon *)WorldManager::getInstance()->GetSelectedObjectByType(tEvent->GetLocation(), CANNON); // find the destroyed game object
		if (cannon != nullptr)
		{
			if (cannon->GetType() == CANNON)
			{
				cannon->SetState(DEAD); // remove tower

				// Find platform under tower
				TowerPlatform *platform = (TowerPlatform*)WorldManager::getInstance()->GetSelectedObjectByType(tEvent->GetLocation(), PLATFORM);
				if (platform)
				{
					platform->TowerOnArea(false);
				}

				// Or find land under tower
				Land *land = (Land*)WorldManager::getInstance()->GetSelectedObjectByType(tEvent->GetLocation(), LAND);
				if (land)
				{
					land->TowerOnArea(false);
				}
			}
		}
	}
	else if (eventType == TOWER_BUILD)
	{
		BuildTower(tEvent->GetLocation());
	}
	else if (eventType == TOWER_DESTROYED)
	{
		// Check for cannons at location
		Tower *tower = (Tower *)WorldManager::getInstance()->GetSelectedObjectByType(tEvent->GetLocation(), TOWER); // find the destroyed game object
		if (tower != nullptr)
		{
			if (tower->GetType() == TOWER)
			{
				tower->SetState(DEAD); // remove tower

				// Find platform under tower
				TowerPlatform *platform = (TowerPlatform*)WorldManager::getInstance()->GetSelectedObjectByType(tEvent->GetLocation(), PLATFORM);
				if (platform)
				{
					platform->TowerOnArea(false);
				}

				// Or find land under tower
				Land *land = (Land*)WorldManager::getInstance()->GetSelectedObjectByType(tEvent->GetLocation(), LAND);
				if (land)
				{
					land->TowerOnArea(false);
				}
			}
		}
	}
}

