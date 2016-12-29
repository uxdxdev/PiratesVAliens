/*

Copyright (c) 2016 David Morton

*/
#include "AppDelegate.h"
#include "scene/menu_scene.h"
#include "base/game_defines.h"
#include "audio/include/AudioEngine.h"
#include "audio/include/SimpleAudioEngine.h"
#include "manager/world_manager.h"

USING_NS_CC;

#define SCREEN_WIDTH 1280			
#define SCREEN_HEIGHT 720

static cocos2d::Size designResolutionSize = cocos2d::Size(SCREEN_WIDTH, SCREEN_HEIGHT);
static cocos2d::Size resolutionSize = cocos2d::Size(SCREEN_WIDTH, SCREEN_HEIGHT);
static cocos2d::Size smallResolutionSize = cocos2d::Size(960, 640);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1280, 720);
static cocos2d::Size largeResolutionSize = cocos2d::Size(1920, 1080);

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {

#if (GOOGLE_PLAY_SERVICES_ENABLED)
	// Google play sign in
	if(!SonarCocosHelper::GooglePlayServices::isSignedIn())
	{
		SonarCocosHelper::GooglePlayServices::signIn();
	}
#endif

	// admob
#if (ADMOB_ENABLED)
	sdkbox::PluginAdMob::init();
	sdkbox::PluginAdMob::setListener(new IMListener());
	sdkbox::PluginAdMob::setTestDevices("EACFA4F5ABC61463121F49DD809E1A9D");
	sdkbox::PluginAdMob::cache("bannerad");
	sdkbox::PluginAdMob::cache("gameover");
#endif

#if (CHARTBOOST_ENABLED)
	// Charboost
  sdkbox::PluginChartboost::init();
  sdkbox::PluginChartboost::setListener(new CBListener());
  sdkbox::PluginChartboost::cache("Default"); // show interstitial on startup using callback
	sdkbox::PluginChartboost::cache("Level Started");
	sdkbox::PluginChartboost::cache("MoreApp");
#endif

    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
	if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("Shipwreck", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("Shipwreck");
#endif
        director->setOpenGLView(glview);
    }
		
	//FileUtils::getInstance()->addSearchPath("audio");

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60.0f);

    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_WIDTH);
    Size frameSize = glview->getFrameSize();
    // if the frame's height is larger than the height of medium size.
    if (frameSize.height > mediumResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is larger than the height of small size.
    else if (frameSize.height > smallResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is smaller than the height of medium size.
    else
    {        
        director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
    }

	// Added to  set content scale factor
	director->setContentScaleFactor(MIN(resolutionSize.height / designResolutionSize.height, resolutionSize.width / designResolutionSize.width));

	Director::getInstance()->setDisplayStats(false);

    register_all_packages();

    // create a scene. it's an autorelease object
	auto scene = MenuScene::createScene();	

    // run
    director->runWithScene(scene);

	// Show full screen ad on start up
	//SonarCocosHelper::AdMob::showFullscreenAd(); // Show full screen interstitial ad

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseAllEffects();    
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {

    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    CocosDenshion::SimpleAudioEngine::getInstance()->resumeAllEffects();	    
}
