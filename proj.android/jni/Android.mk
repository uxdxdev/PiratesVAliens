LOCAL_PATH := $(call \
my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH))

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
../../Classes/AppDelegate.cpp \
../../Classes/ai/pathfinding_astar.cc \
../../Classes/base/game_event.cc \
../../Classes/base/game_world.cc \
../../Classes/base/vector3.cc \
../../Classes/base/game_model.cc \
../../Classes/entities/enemy.cc \
../../Classes/entities/land.cc \
../../Classes/entities/tower.cc \
../../Classes/entities/cannon.cc \
../../Classes/entities/tower_platform.cc \
../../Classes/layer/board_layer.cc \
../../Classes/layer/hud_layer.cc \
../../Classes/layer/build_menu.cc \
../../Classes/layer/upgrade_menu.cc \
../../Classes/manager/spawn_manager.cc \
../../Classes/manager/world_manager.cc \
../../Classes/scene/game_scene.cc \
../../Classes/scene/game_over_scene.cc \
../../Classes/scene/menu_scene.cc \
../../Classes/dao/data_access_object.cc \
../../Classes/JNIHelpers.cpp \
../../Classes/JNIResults.cpp \
../../Classes/SonarCocosHelperCPP/SonarFrameworks.cpp

LOCAL_CPPFLAGS := -DSDKBOX_ENABLED
LOCAL_LDLIBS := -landroid \
-llog
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes
LOCAL_WHOLE_STATIC_LIBRARIES := PluginAdMob \
sdkbox \
PluginChartboost

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module, ./sdkbox)
$(call import-module, ./pluginadmob)
$(call import-module, ./pluginchartboost)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
