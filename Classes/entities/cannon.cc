/*

Copyright (c) 2016 David Morton

*/
#include "entities/cannon.h"
#include "entities/bullet.h"
#include "manager/world_manager.h"
#include "audio/include/SimpleAudioEngine.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "SonarCocosHelperCPP/SonarFrameworks.h"
#endif
#define CANNON_BALL_SPEED 50.0f
#define CANNON_VIEW_RADIUS 200.0f
#define CANNON_FIRE_RATE 60 // 60 = second (FPS)
#define CANNON_ATTACK_STREGTH 30
#define MAX_LEVEL 3
#define	LEVEL_UP_COST 50

Cannon::Cannon(const std::string &filename)
{
	//m_pSprite = cocos2d::Sprite::create(filename);
	m_pSprite = cocos2d::Sprite::createWithSpriteFrameName("towerBaseLevel1.png");
	WorldManager::getInstance()->GetGameBoard()->addChild(m_pSprite, 2);
	WorldManager::getInstance()->AddObserver(this);

	
	// Tower Stats
	m_iLevel = 1; // Level 1 tower
	m_iAttackStrength = CANNON_ATTACK_STREGTH;
	m_fViewRadius = CANNON_VIEW_RADIUS * WorldManager::getInstance()->GetGameScaleFactor();
	m_iLevelUpCost = LEVEL_UP_COST;
	m_iFireRate = CANNON_FIRE_RATE;
	
	
	// Place tower top
	cocos2d::Vec2 topTowerPosition;
	topTowerPosition.x = m_pSprite->getPositionX() + (m_pSprite->getContentSize().width / 2);
	topTowerPosition.y = m_pSprite->getPositionY() + (m_pSprite->getContentSize().height / 1.5);
	m_TowerTop = cocos2d::Sprite::create(filename);	
	m_TowerTop->setPosition(topTowerPosition);
	WorldManager::getInstance()->ScaleSprite(m_TowerTop);
	m_pSprite->addChild(m_TowerTop);

	m_pLine = cocos2d::Sprite::create("objects/line.png");
	m_pLine->setAnchorPoint(cocos2d::Vec2::ZERO);
	
	// Center of tower
	cocos2d::Vec2 thisPosition;
	thisPosition.x = m_pSprite->getPositionX() + (m_pSprite->getContentSize().width / 2);
	thisPosition.y = m_pSprite->getPositionY() + (m_pSprite->getContentSize().height / 2);

	m_pLine->setPosition(thisPosition);
	m_pLine->setScaleY(m_fViewRadius / m_pLine->getContentSize().height);
	//m_pSprite->addChild(m_pLine);

	m_pRadius = cocos2d::Sprite::create("objects/radius.png");
	float radiusScale = m_fViewRadius / m_pRadius->getContentSize().width * 2.0f;
	m_pRadius->setPosition(thisPosition);
	m_pRadius->setScaleX(radiusScale);
	m_pRadius->setScaleY(radiusScale);
	m_pRadius->setVisible(false);	
	auto fadeInOut = cocos2d::Sequence::create(cocos2d::FadeOut::create(0.5f), cocos2d::FadeIn::create(0.5f), NULL);
	m_pRadius->runAction(cocos2d::RepeatForever::create(fadeInOut));
	m_pSprite->addChild(m_pRadius, -1);
	
	
	m_eState = ALIVE;
	m_eObjectType = CANNON;
	m_pTarget = nullptr;
	m_iShootTime = 0;
	m_iCost = CANNON_COST;

	SoundId = 0;
		
	std::string constructionSound = "audio/Construction" + std::string(MUSIC_EXTENSION);
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(constructionSound.c_str());	

	// Google Analytics
#if (GOOGLE_ANALYTICS_ENABLED)
	SonarCocosHelper::GoogleAnalytics::sendEvent("Build", "Cannon Built", "Player built a cannon", 0);
#endif
}

Cannon::~Cannon()
{
	WorldManager::getInstance()->RemoveObserver(this);
	m_vpEnemyList = nullptr;
}

bool Cannon::LevelUp()
{
	if (m_iLevel < MAX_LEVEL)
	{
		if (WorldManager::getInstance()->GetGameModel()->GetCoins() >= m_iLevelUpCost)
		{
			std::string constructionSound = "audio/PowerUp" + std::string(MUSIC_EXTENSION);
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(constructionSound.c_str());

			// Deduct level up cost from coins
			WorldManager::getInstance()->GetGameModel()->SetCoins(WorldManager::getInstance()->GetGameModel()->GetCoins() - m_iLevelUpCost);
			WorldManager::getInstance()->Notify(GameEvent::create(UPDATE_STATS));

			m_iLevel++; // Level up
			m_iAttackStrength += 10; // Increase attack
			m_fViewRadius += 100 * WorldManager::getInstance()->GetGameScaleFactor(); // Increase radius and scale
			//m_iLevelUpCost += 25; // Increase level up cost
			m_iFireRate -= 5; // Reduce fire rate cooldown

			// Resize the radius sprite
			float radiusScale = m_fViewRadius / m_pRadius->getContentSize().width * 2.0f;
			m_pRadius->setScaleX(radiusScale);
			m_pRadius->setScaleY(radiusScale);

			// Update tower sprite to the next level
			std::stringstream ss;
			ss << "towerBaseLevel" << m_iLevel << ".png";
			m_pSprite->setSpriteFrame(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(ss.str()));

			// Level up particle effect
			cocos2d::ParticleSystemQuad* levelUpParticle = cocos2d::ParticleSystemQuad::create("particle/LevelUpParticles.plist");
			levelUpParticle->setAutoRemoveOnFinish(true);
			levelUpParticle->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
			levelUpParticle->setPosition(cocos2d::Vec2(m_pSprite->getContentSize().width / 2, m_pSprite->getContentSize().height));
			m_pSprite->addChild(levelUpParticle);

			// Google Analytics
#if (GOOGLE_ANALYTICS_ENABLED)
			SonarCocosHelper::GoogleAnalytics::sendEvent("Upgrade", "Cannon Upgrade", "Player has upgraded a cannon", 0);
#endif

			return true;
		}
		else
		{
			WorldManager::getInstance()->Notify(GameEvent::create(RESOURCES_TOO_LOW));
			return false;
		}
	}
	else
	{
		// Cannot level up max level reached
		return false;
	}
}

void Cannon::SetState(State state)
{
	m_eState = state;
	if (m_eState == DEAD)
	{
		m_pSprite->setVisible(false);
		m_pSprite->getParent()->removeChild(m_pSprite);
		m_pSprite = nullptr;
	}
}

void Cannon::Update(float dt)
{	
	if (m_pTarget != nullptr && m_pSprite != nullptr)
	{
		cocos2d::Vec2 thisPosition;
		thisPosition = m_pSprite->getPosition();

		m_TargetPosition = WorldManager::getInstance()->GetCenter(m_pTarget);

		float angle = atan2(thisPosition.x - m_TargetPosition.x, thisPosition.y - m_TargetPosition.y);
		angle = angle * (180.0f / PI);

		if (angle < 0)
		{
			angle += 360.0f;
		}

		m_TowerTop->setRotation(angle + 180.0f);
		//m_pSprite->setRotation(angle + 180.0f);

		float distance = m_pSprite->getPosition().getDistance(WorldManager::getInstance()->GetCenter(m_pTarget));
		//m_pLine->setScaleY(distance / m_pLine->getContentSize().height);
	}	

	ShootTimer(dt);

}

void Cannon::OnNotify(std::shared_ptr<GameEvent> tEvent)
{
	if (tEvent->GetEventType() == ENEMY_SPAWNED)
	{
		if (m_pTarget == nullptr)
		{
			FindTarget();
		}
	}	
	else if (tEvent->GetEventType() == ENEMY_DESTROYED)
	{			
		FindTarget();		
	}
}


void Cannon::FindTarget()
{	
	m_pTarget = nullptr;

	float smallestDistance = 99999;
	m_vpEnemyList = WorldManager::getInstance()->GetEnemyObjects();

	if (m_pSprite != nullptr)
	{
		for (size_t i = 0; i < m_vpEnemyList->size(); i++)
		{
			if (m_vpEnemyList->at(i)->GetState() == ALIVE && m_vpEnemyList->at(i)->GetType() == ENEMY)
			{
				float distance = m_pSprite->getPosition().getDistance(WorldManager::getInstance()->GetCenter(m_vpEnemyList->at(i)->GetSprite()));
				
				if (distance < smallestDistance && distance < m_fViewRadius)
				{
					m_pTarget = m_vpEnemyList->at(i)->GetSprite();
					smallestDistance = distance;
				}
			}
			else
			{
				m_pTarget = nullptr;
			}
		}
	}	
}

void Cannon::ShootTimer(float dt)
{	
	FindTarget();

	// If there is a target
	if (m_pTarget != nullptr)
	{
		// Increament shoot timer
		m_iShootTime++;
		if (m_iShootTime > CANNON_FIRE_RATE)
		{
			Shoot();
			m_iShootTime = 0;
		}
	}
}

void Cannon::Shoot()
{
	std::shared_ptr<Bullet> bulletObj(new Bullet("objects/cannonBall.png", BULLET));
	bulletObj->GetSprite()->setPosition(m_pSprite->getPosition());
	bulletObj->SetAttackStrength(m_iAttackStrength); // Set the bullet damage based on the towers attack strength
	WorldManager::getInstance()->AddBulletToList(bulletObj);
	WorldManager::getInstance()->GetGameBoard()->addChild(bulletObj->GetSprite(), 2);
	
	// Particle effects
	cocos2d::ParticleSystemQuad* particleSystem = cocos2d::ParticleSystemQuad::create("particle/CannonShot.plist");
	particleSystem->setAutoRemoveOnFinish(true);
	particleSystem->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));	
	particleSystem->setPosition(cocos2d::Vec2(m_pSprite->getContentSize().width / 2, m_pSprite->getContentSize().height));
	m_pSprite->addChild(particleSystem);

	cocos2d::CallFunc *bulletCallback = cocos2d::CallFunc::create(CC_CALLBACK_0(Cannon::BulletMovementEnd, this, bulletObj->GetSprite()));

	cocos2d::Vec2 targetPosition = m_TargetPosition; // Grab a copy of the targets position when firing

	// Find duration based on distance and movement speed
	float distance = bulletObj->GetSprite()->getPosition().getDistance(m_TargetPosition);
	float duration = distance / CANNON_BALL_SPEED;

	cocos2d::Vec2 direction = cocos2d::Vec2(bulletObj->GetSprite()->getPosition(), targetPosition);

	direction.scale(8);
	cocos2d::MoveBy *bulletAction = cocos2d::MoveBy::create(duration, direction);
	
	bulletObj->GetSprite()->runAction(cocos2d::Sequence::create(bulletAction, bulletCallback, NULL));
		
	// rand() % (max_number + 1 - minimum_number)) + minimum_number;
	int shotSound = rand() % (2 + 1 - 0) + 0;
	std::string m_strCannonShotSound;
	switch (shotSound)
	{
	case 0:
		m_strCannonShotSound = "audio/CannonShot" + std::string(MUSIC_EXTENSION);
		break;
	case 1:
		m_strCannonShotSound = "audio/CannonShot01" + std::string(MUSIC_EXTENSION);
		break;
	case 2:
		m_strCannonShotSound = "audio/CannonShot02" + std::string(MUSIC_EXTENSION);
		break;
	default:
		m_strCannonShotSound = "audio/CannonShot" + std::string(MUSIC_EXTENSION);
		break;
	}
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(m_strCannonShotSound.c_str());	
	
	
}

void Cannon::BulletMovementEnd(cocos2d::Sprite *bullet)
{
	//CCLOG("Bullet destroyed");
	bullet->setVisible(false);
}
