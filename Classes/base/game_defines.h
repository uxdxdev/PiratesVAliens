/*

Copyright (c) 2016 David Morton

*/
#ifndef GAME_DEFINES_H_
#define GAME_DEFINES_H_

#include "cocos2d.h"

// Contains private keys and other config detials for Google Play Services, etc.
// - Leaderboard ID
// - Achievement IDs
#include "PrivateConfigSettings.h"

// Labels
#define LABEL_FONT "fonts/rexlia.ttf"
#define LABEL_FONT_SIZE 32
#define PADDING_LARGE 100
#define PADDING_SMALL 50

#define CANNON_COST 100
#define TOWER_COST 100

#define PI 3.14159

// Plugins sdkbox
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define CHARTBOOST_ENABLED 1 // sdkbox
#define ADMOB_ENABLED 1 // sdkbox
#define GOOGLE_ANALYTICS_ENABLED 1 // Sonar
#define GOOGLE_PLAY_SERVICES_ENABLED 1 // Sonar
#endif

#define OUTPUT_LOGFILE 0

struct PlayerStats{
	std::string playerUsername;
	int playerRank;
	int playerScore;
};

enum eObjectType{
	ENEMY,
	CANNON,
	TOWER,
	PLATFORM,
	LAND,
	BULLET,
	NONE
};

enum eEnemyType{
	ENEMY_BLUE,
	ENEMY_RED,
	ENEMY_GREEN,
	ENEMY_YELLOW,
	ENEMY_BOSS
};

enum eEventType{
	PLATFORM_BUILD,
	PLATFORM_DESTROY,
	ENEMY_SPAWNED,
	ENEMY_DESTROYED,
	CANNON_BUILD,
	CANNON_DESTROYED,
	TOWER_BUILD,
	TOWER_DESTROYED,
	WAVE_START,
	WAVE_END,
	STOP_SPAWNING,
	START_SPAWNING,
	LAND_BUILD,
	TOUCH_ENDED,
	UPDATE_STATS,
	BASE_ATTACKED,
	BOSS_ATTACKED,
	BOSS_DESTROYED,
	GAME_START,
	GAME_OVER,
	RESOURCES_TOO_LOW
};

enum State{
	DEAD,
	ALIVE
};

#include <string>
#include <sstream>

// Added because std::to_string is not available on android
template <typename T>
std::string to_string(T value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#define MUSIC_EXTENSION	".wav"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
#define MUSIC_EXTENSION        ".wav"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_BLACKBERRY)
#define MUSIC_EXTENSION        ".ogg"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define MUSIC_EXTENSION        ".mp3"
#else
#define MUSIC_EXTENSION        ".mp3"
#endif // CC_PLATFOR_WIN32

#endif // Game Defines
