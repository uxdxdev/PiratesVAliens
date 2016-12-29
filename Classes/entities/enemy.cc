/*

Copyright (c) 2016 David Morton

*/
#include "entities/enemy.h"
#include "manager/world_manager.h"
#include "audio/include/SimpleAudioEngine.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "SonarCocosHelperCPP/SonarFrameworks.h"
#endif
Enemy::Enemy(const std::string &filename, eEnemyType type)
{	
	m_pSprite = cocos2d::Sprite::create();
	m_pSprite->setAnchorPoint(cocos2d::Vec2::ZERO);

	auto spriteGraphic = cocos2d::Sprite::createWithSpriteFrameName(filename + "0.png");
	spriteGraphic->setAnchorPoint(cocos2d::Vec2::ZERO);
	
	WorldManager::getInstance()->GetGameBoard()->addChild(m_pSprite, 3);
	m_pSprite->setContentSize(spriteGraphic->getContentSize());
	m_pSprite->addChild(spriteGraphic);
	
	m_eState = ALIVE;
	m_eObjectType = ENEMY;		
	cocos2d::Vec2 gameWorldCell = WorldManager::getInstance()->PositionToTileCoordinates(WorldManager::getInstance()->GetEnemySpawnPosition()); // Start at the spawn point and go from there
	m_pTargetPosition = WorldManager::getInstance()->GetEnemyTargetSpritePosition(gameWorldCell);
	m_bIsAtDestination = true; // for path finding nodes
	m_bIsAtFinalTarget = false;
	m_iNodeIndexInPath = 0;
	m_iDifficultyMultiplier = WorldManager::getInstance()->GetDifficulty();
	m_Type = type;
	if (m_Type == ENEMY_BLUE)
	{
		m_iMaxHealth = ENEMY_BLUE_HEALTH * m_iDifficultyMultiplier;
		m_iHealth = m_iMaxHealth;
		m_fMovementSpeed = ENEMY_BLUE_SPEED;		
	}
	else if (m_Type == ENEMY_RED)
	{
		m_iMaxHealth = ENEMY_RED_HEALTH * m_iDifficultyMultiplier;
		m_iHealth = m_iMaxHealth;
		m_fMovementSpeed = ENEMY_RED_SPEED;				
	}
	else if (m_Type == ENEMY_GREEN)
	{
		m_iMaxHealth = ENEMY_GREEN_HEALTH * m_iDifficultyMultiplier;
		m_iHealth = m_iMaxHealth;
		m_fMovementSpeed = ENEMY_GREEN_SPEED;
	}
	else if (m_Type == ENEMY_YELLOW)
	{
		m_iMaxHealth = ENEMY_YELLOW_HEALTH * m_iDifficultyMultiplier;
		m_iHealth = m_iMaxHealth;
		m_fMovementSpeed = ENEMY_YELLOW_SPEED;
	}
	else if (m_Type == ENEMY_BOSS)
	{
		m_iMaxHealth = ENEMY_BOSS_HEALTH * m_iDifficultyMultiplier;
		m_iHealth = m_iMaxHealth;
		m_fMovementSpeed = ENEMY_BOSS_SPEED;

		// Particle effects for spawnpoint
		cocos2d::ParticleSystemQuad* particleSystem = cocos2d::ParticleSystemQuad::create("particle/BossParticle.plist");
		particleSystem->setAutoRemoveOnFinish(true);
		particleSystem->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));			
		particleSystem->setPosition(spriteGraphic->getContentSize().width / 2, spriteGraphic->getContentSize().height / 2);
		spriteGraphic->addChild(particleSystem, 1);

		// Google Analytics
#if (GOOGLE_ANALYTICS_ENABLED)
		SonarCocosHelper::GoogleAnalytics::sendEvent("Spawned", "Boss Spawned", "Alien boss has spawned in game", 0);
#endif
	}
	
	// load all the animation frames into an array
	const int kNumberOfFrames = 2;
	cocos2d::Vector<cocos2d::SpriteFrame*> frames;
	for (int i = 0; i < kNumberOfFrames; i++)
	{
		std::stringstream ss;
		ss << filename << i << ".png";
		frames.pushBack(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(ss.str()));
	}

	// play the animation
	cocos2d::Animation* anim = new cocos2d::Animation;
	anim->initWithSpriteFrames(frames, 0.25f);
	spriteGraphic->runAction(cocos2d::RepeatForever::create(cocos2d::Animate::create(anim)));

	auto healthBarSprite = cocos2d::Sprite::create("objects/healthBar.png");	
	m_HealthBar = cocos2d::ProgressTimer::create(healthBarSprite);
	m_HealthBar->setType(ProgressTimerType::BAR);
	m_HealthBar->setMidpoint(cocos2d::Vec2(0, 0.5));
	m_HealthBar->setPercentage((m_iHealth * 100) / m_iMaxHealth);
	m_HealthBar->setBarChangeRate(cocos2d::Vec2(1, 0));
	m_HealthBar->setAnchorPoint(cocos2d::Vec2::ZERO);
	m_HealthBar->setPositionY(spriteGraphic->getContentSize().height + 5);
	spriteGraphic->addChild(m_HealthBar, 1);

	auto healthBarBackgroundSprite = cocos2d::Sprite::create("objects/healthBarBackground.png");
	healthBarBackgroundSprite->setAnchorPoint(cocos2d::Vec2::ZERO);
	m_HealthBar->addChild(healthBarBackgroundSprite, -1);

	// Enemy movement
	auto moveUp = cocos2d::MoveBy::create(1.0f, cocos2d::Vec2(0, 10));
	auto moveDownToCenter = cocos2d::MoveBy::create(1.0f, cocos2d::Vec2(0, -10));
	auto moveDown = cocos2d::MoveBy::create(1.0f, cocos2d::Vec2(0, -10));
	auto moveUpToCenter = cocos2d::MoveBy::create(1.0f, cocos2d::Vec2(0, 10));
	auto wobble = cocos2d::Sequence::create(moveUp, moveDownToCenter, moveDown, moveUpToCenter, NULL);
	spriteGraphic->runAction(cocos2d::RepeatForever::create(wobble));
}

Enemy::~Enemy()
{
}

void Enemy::SetState(State state)
{
	m_eState = state;
	if (m_eState == DEAD)
	{		
		// If the enemy has died because it reached the final destination (village)
		// don't play the death animation, play the village being attacked
		if (m_bIsAtFinalTarget)
		{
			VillageAttack();
		}
		else
		{
			DeathAnimation();
			// If this enemy is a boss create a boss killed event
			if (m_Type == ENEMY_BOSS)
			{
				WorldManager::getInstance()->Notify(GameEvent::create(BOSS_DESTROYED));
			}
		}

		

		m_pSprite->setVisible(false);
		m_pSprite->getParent()->removeChild(m_pSprite);
		m_pSprite = nullptr;
	}
}

void Enemy::VillageAttack()
{
	// rand() % (max_number + 1 - minimum_number)) + minimum_number;
	int villagePanic = rand() % (2 + 1 - 0) + 0;
	std::string villagePanicSoundEffect;
	switch (villagePanic)
	{
	case 0:
		villagePanicSoundEffect = "audio/VillagePanic" + std::string(MUSIC_EXTENSION);
		break;
	case 1:
		villagePanicSoundEffect = "audio/VillagePanic01" + std::string(MUSIC_EXTENSION);
		break;
	case 2:
		villagePanicSoundEffect = "audio/VillagePanic02" + std::string(MUSIC_EXTENSION);
		break;
	default:
		villagePanicSoundEffect = "audio/VillagePanic" + std::string(MUSIC_EXTENSION);
		break;
	}	
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(villagePanicSoundEffect.c_str());

	// Google Analytics
#if (GOOGLE_ANALYTICS_ENABLED)
	SonarCocosHelper::GoogleAnalytics::sendEvent("Attack", "Enemy Attack", "An enemy has attacked the village", 0);
#endif
}

void Enemy::DeathAnimation()
{
	// rand() % (max_number + 1 - minimum_number)) + minimum_number;
	int deathSound = rand() % (2 + 1 - 0) + 0;
	std::string enemyDeathSoundEffect;
	switch (deathSound)
	{
	case 0:
		enemyDeathSoundEffect = "audio/AlienDeath" + std::string(MUSIC_EXTENSION);
		break;
	case 1:
		enemyDeathSoundEffect = "audio/AlienDeath01" + std::string(MUSIC_EXTENSION);
		break;
	case 2:
		enemyDeathSoundEffect = "audio/AlienDeath02" + std::string(MUSIC_EXTENSION);
		break;
	default:
		enemyDeathSoundEffect = "audio/AlienDeath" + std::string(MUSIC_EXTENSION);
		break;
	}
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(enemyDeathSoundEffect.c_str());

	cocos2d::ParticleSystemQuad* deathParticles;

	if (m_Type == ENEMY_BOSS)
	{
		deathParticles = cocos2d::ParticleSystemQuad::create("particle/BossDeathParticle.plist");
	}
	else
	{
		deathParticles = cocos2d::ParticleSystemQuad::create("particle/EnemyDeathParticle.plist");
	}

	// Particle effects	 
	deathParticles->setAutoRemoveOnFinish(true);
	deathParticles->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
	deathParticles->setPosition(WorldManager::getInstance()->GetCenter(m_pSprite));
	m_pSprite->getParent()->addChild(deathParticles, 10);
}

eEnemyType Enemy::GetEnemyType()
{
	return m_Type;
}

void Enemy::TakeDamage(int value)
{
	m_iHealth -= value;
	m_HealthBar->setPercentage((m_iHealth * 100) / m_iMaxHealth);
	if (m_iHealth <= 0 && m_eState == ALIVE)
	{
		SetState(DEAD);		
		WorldManager::getInstance()->Notify(GameEvent::create(ENEMY_DESTROYED));
	}
	//CCLOG("TakeDamage(): Remaining health: %d", m_iHealth);
}

void Enemy::MoveToNextNode()
{		
	//NextNodeInPath();


	if (m_bIsAtDestination)
	{
		NextNodeInPath();
				
		cocos2d::CallFunc *enemyCallback = cocos2d::CallFunc::create(CC_CALLBACK_0(Enemy::AtDestination, this));

		if (m_pSprite != nullptr && m_eState == ALIVE)
		{
			float distance = m_pSprite->getPosition().getDistance(m_pTargetPosition);
			float duration = distance / m_fMovementSpeed;

			cocos2d::MoveTo *moveAction = cocos2d::MoveTo::create(duration, m_pTargetPosition);
			m_pSprite->runAction(cocos2d::Sequence::create(moveAction, enemyCallback, NULL));
		}				
	}


	/*
	cocos2d::Point playerPos = this->GetSprite()->getPosition();
	cocos2d::TMXTiledMap *tileMap = WorldManager::getInstance()->GetGameBoard()->GetTiledMap();
	float tileWidth = tileMap->getTileSize().width;
	float tileHeight = tileMap->getTileSize().height;
	
	// Get the difference between the two points and determine if the move is left/right or up/down
	cocos2d::Point diff = m_pTarget->getPosition() - playerPos;

	if (abs(diff.x) > abs(diff.y)) {
		if (diff.x > 0) {
			//playerPos.x += tileWidth;
			playerPos.x += m_fMovementSpeed;
		}
		else {
			//playerPos.x -= tileWidth;
			playerPos.x -= m_fMovementSpeed;
		}
	}
	else {
		if (diff.y > 0) {
			//playerPos.y += tileHeight;
			playerPos.y += m_fMovementSpeed;
		}
		else {
			//playerPos.y -= tileHeight;
			playerPos.y -= m_fMovementSpeed;
		}
	}

	// safety check on the bounds of the map
	if (playerPos.x <= (tileMap->getMapSize().width * tileWidth) && playerPos.y <= (tileMap->getMapSize().height * tileHeight) &&
		playerPos.y >= 0 &&
		playerPos.x >= 0)
	{
		this->GetSprite()->setPosition(playerPos);		
	}
	*/
}

void Enemy::Update(float dt)
{	
	if (m_eState == ALIVE)
	{
		MoveToNextNode();
	}
}

void Enemy::NextNodeInPath()
{			
	// Find the next node in the path and set it as the target
	m_iNodeIndexInPath++;
	if (m_iNodeIndexInPath < (int)WorldManager::getInstance()->GetPathToTarget()->size())
	{
		// Get the sprite at the tile coordinate stored in the path vector
		m_bIsAtDestination = false;
		cocos2d::Vec2 nextNode;
		nextNode.x = WorldManager::getInstance()->GetPathToTarget()->at(m_iNodeIndexInPath)->m_iX;
		nextNode.y = WorldManager::getInstance()->GetPathToTarget()->at(m_iNodeIndexInPath)->m_iZ;
		m_pTargetPosition = WorldManager::getInstance()->GetEnemyTargetSpritePosition(nextNode);
	}	
	else
	{
		//CCLOG("At final destination, ATTACK!!");
		if (m_pSprite->getPosition() == m_pTargetPosition)
		{			
			m_bIsAtFinalTarget = true;
			TakeDamage(m_iHealth); // Kill off the enemy object	
			if (m_Type == ENEMY_BOSS)
			{
				WorldManager::getInstance()->Notify(GameEvent::create(BOSS_ATTACKED));
			}
			else
			{
				WorldManager::getInstance()->Notify(GameEvent::create(BASE_ATTACKED));
			}
		}
	}	
}

void Enemy::AtDestination()
{
	m_bIsAtDestination = true;
}
