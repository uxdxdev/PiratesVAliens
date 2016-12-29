/*

Copyright (c) 2016 David Morton

*/
#ifndef GAME_OVER_SCENE_H__
#define GAME_OVER_SCENE_H__

#include "base/game_defines.h"

USING_NS_CC;

class GameOver : public cocos2d::Layer, public TextFieldDelegate
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	// a selector callback
	void GameRestart(cocos2d::Ref* pSender);

	void update(float dt) override;
	
	// implement the "static create()" method manually
	CREATE_FUNC(GameOver);
		
	// Add effects to the Play button
	void addParticlesToButtons(MenuItemLabel* button);

	// TODO : Implement clean up function 
	void mainMenuCleanup();

	// User enter his name to save the score 
	void createTF();

	// Event listener, listen to touches on screen 
	virtual bool onTouchBegan(Touch *pTouch, Event *pEvent);

	// TODO : Implement keyboard on the screen to user enter details (mobile feature) 
	void keyboardWillShow(IMEKeyboardNotificationInfo &info);
	void keyboardWillHide(IMEKeyboardNotificationInfo &info);

	// Text field delegate 
	virtual bool onTextFieldAttachWithIME(TextFieldTTF *pSender) override;
	virtual bool onTextFieldDetachWithIME(TextFieldTTF *pSender) override;
	virtual bool onTextFieldInsertText(TextFieldTTF *pSender, const char *text, size_t nLen) override;
	virtual bool onTextFieldDeleteBackward(TextFieldTTF *pSender, const char *delText, size_t nLen) override;
	virtual bool onDraw(TextFieldTTF * pSender);

	// Remove seq node everytime a new character was input 
	void callbackRemoveNodeWhenDidAction(Node * node);

	void checkUsername(cocos2d::Ref *pSender);

	void TurnOnSaveButton();
	void TurnOffSaveButton();
		
	void SaveGame(Ref *pSender);
	void ShareScore(cocos2d::Ref *pSender);

	void UpdatePlayerStats(PlayerStats playerStats);

	void UpdateLoginLabel(std::string update)
	{
		m_LoginLabel->setString(update.c_str());
	}

	void OpenKeyboard(cocos2d::Ref *pSender);

	void OpenLeaderboard(cocos2d::Ref *pSender);


private:
	// Play button, starts Game Scene 
	MenuItemLabel* m_pCheckUsernameButton;
	MenuItemLabel* m_pSaveButton;
	MenuItemLabel* m_pShareButton;

	// Pointer to the user's enter details Text Field TTF 
	TextFieldTTF *m_pTextField;

	// Max amount of characters allowed 
	int m_nCharLimit;

	Label *m_LoginLabel;
	Label *m_ScoreLabel;

	int m_iUsernameCharCount;

	PlayerStats m_PlayerStats;
};

#endif // GAME_OVER_SCENE_H__
