# Pirates V Aliens

Cocos2dx game written in C++ for iOS and Android  

## Build instructions

- Create new cocos project  
`cocos new -l cpp -p com.domain.project Project`  
- Install sdkbox and import `Chartboost`, `Admob`  
```
cd Project
sdkbox import chartboost
sdkbox import admob
```  
- Update `sdkbox_config.json` for each sdkbox plugin  
- Install Cocos Helper Classes - Instructions http://sonarsystems.co.uk/cocoshelper.php  
- Configure `AndroidManifest.xml` for each Cocos Helper plugin  
- Clone this repo  
`git clone git@github.com:damorton/shipwreck.git`  
- Copy `Classes` and `Resources` directory to new `Project` directory and overwrite existing files  
- Update `ant.properties` file with `keystore` information  
- Configure `Google Play Console` with `Leaderboards`, `Achievements`  
- Configure `Google Analytics`
- Build  

### XCode development
- Add the `Classes` directory to build settings under `User Header Search Path`  
`$(PROJECT_DIR)/../Classes`  

## Dependencies

- Cocos2dx v3.10  
- NDK r10c http://pnsurez.blogspot.ie/2015/07/download-android-ndk-tools.html  
- Google Play Services r22 http://venomvendor.blogspot.ie/2012/03/android-sdk-extras-by-google-inc.html  
- sdkbox v 1.0.1.15  
- Sonar Cocos Helper v1.50 https://github.com/SonarSystems/Cocos-Helper  
