
if(WINDOWS AND BUILD_SHARED_LIBS)
  ADD_DEFINITIONS (-D_USEGUIDLL)
endif()

if(WINDOWS)
  set(COCOS_UI_SPECIFIC_SRC
      ui/UIEditBox/UIEditBoxImpl-win32.cpp
      )
elseif(MACOSX)
  set(COCOS_UI_SPECIFIC_SRC
      ui/UIEditBox/UIEditBoxImpl-mac.mm
      )
elseif(LINUX)
  set(COCOS_UI_SPECIFIC_SRC
      ui/UIEditBox/UIEditBoxImpl-common.cpp
      ui/UIEditBox/UIEditBoxImpl-linux.cpp
      )
endif()

#todo: android UIWebViewImpl and UIVideoPlayer
# will add when doing android support

set(COCOS_UI_SRC

  ui/CocosGUI.cpp
  ui/UIButton.cpp
  ui/UIAbstractCheckButton.cpp
  ui/UICheckBox.cpp
  ui/UIRadioButton.cpp
  ui/UIDeprecated.cpp
  ui/UIHBox.cpp
  ui/UIHelper.cpp
  ui/UIImageView.cpp
  ui/UILayout.cpp
  ui/UILayoutManager.cpp
  ui/UILayoutParameter.cpp
  ui/UIListView.cpp
  ui/UILoadingBar.cpp
  ui/UIPageView.cpp
  ui/UIPageViewIndicator.cpp
  ui/UIRelativeBox.cpp
  ui/UIRichText.cpp
  ui/UIScale9Sprite.cpp
  ui/UIScrollView.cpp
  ui/UIScrollViewBar.cpp
  ui/UISlider.cpp
  ui/UIText.cpp
  ui/UITextAtlas.cpp
  ui/UITextBMFont.cpp
  ui/UITextField.cpp
  ui/UIVBox.cpp
  ui/UIWebView.cpp
  ui/UIWidget.cpp
  ui/UIEditBox/UIEditBox.cpp
  ui/UILayoutComponent.cpp
  ${COCOS_UI_SPECIFIC_SRC}

)
