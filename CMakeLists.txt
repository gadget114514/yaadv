# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2015-2017 Chukong Technologies Inc.
#
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

cmake_minimum_required(VERSION 3.1)

set(APP_NAME TemplateCpp)

project(${APP_NAME})

set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cocos2d)
set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

include(CocosBuildSet)
if(NOT USE_COCOS_PREBUILT)
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)
  endif()

  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/wren")

# record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)

set(GAME_RES_FOLDER
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    )
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

include_directories(
        Classes
        ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
	${COCOS2DX_ROOT_PATH}/../lwf/cplusplus/core
	${COCOS2DX_ROOT_PATH}/../wren/src/include

)
# add cross-platforms source files and header files 
list(APPEND GAME_SOURCE
     Classes/AppDelegate.cpp
     Classes/HelloWorldScene.cpp
Classes/AudioHelper.cpp     
Classes/yaadv/LoadScene.cpp
Classes/yaadv/MainMenuScene.cpp
Classes/yaadv/ScriptReader/BackgroundManager.cpp
Classes/yaadv/ScriptReader/SCX.cpp
Classes/yaadv/ScriptReader/ScriptReader.cpp
Classes/yaadv/ScriptReader/BackgroundMusicManager.cpp
Classes/yaadv/ScriptReader/SoundManager.cpp
Classes/yaadv/ScriptReader/CharacterManager.cpp
Classes/yaadv/ScriptReader/ScriptCommand.cpp
Classes/yaadv/HistoryScene.cpp
Classes/yaadv/SaveData.cpp
Classes/yaadv/CharLabel.cpp
Classes/yaadv/SaveScene.cpp
Classes/yaadv/Control/RadioButton.cpp
Classes/yaadv/Control/EasyButton.cpp
Classes/yaadv/Control/Slidebar.cpp
Classes/yaadv/Control/PopupLayer.cpp
Classes/yaadv/SettingScene.cpp
Classes/yaadv/GameScene.cpp
Classes/yaadv/GameSystem.cpp
Classes/yaadv/History.cpp
Classes/yaadv/ScriptReader/ini.c
Classes/yaadv/ScriptReader/yaadvConfig.cpp
Classes/yaadv/ScriptReader/ScriptReaderWREN.cpp
Classes/yaadv/Control/Between.cpp
Classes/yaadv/Control/StringUtil.cpp
     )
list(APPEND GAME_HEADER
     Classes/AppDelegate.h
     Classes/HelloWorldScene.h
Classes/yaadv/MainMenuScene.h
Classes/yaadv/GameScene.h
Classes/yaadv/ScriptReader/CharacterManager.h
Classes/yaadv/ScriptReader/BackgroundManager.h
Classes/yaadv/ScriptReader/ScriptReader.h
Classes/yaadv/ScriptReader/SCX.h
Classes/yaadv/ScriptReader/ScriptCommand.h
Classes/yaadv/ScriptReader/SoundManager.h
Classes/yaadv/ScriptReader/BackgroundMusicManager.h
Classes/AudioHelper.h
Classes/yaadv/GameSystem.h
Classes/HelloWorldScene.h
Classes/yaadv/SettingScene.h
Classes/yaadv/Control/RadioButton.h
Classes/yaadv/Control/Slidebar.h
Classes/yaadv/Control/EasyButton.h
Classes/yaadv/Control/Between.h
Classes/yaadv/HistoryScene.h
Classes/yaadv/SaveData.h
Classes/yaadv/SaveScene.h
Classes/yaadv/CharLabel.h
Classes/AppDelegate.h
Classes/yaadv/LoadScene.h
Classes/yaadv/ScriptReader/ini.h
Classes/yaadv/ScriptReader/yaadvConfig.h
Classes/yaadv/ScriptReader/ScriptReaderWREN.h
)

if(ANDROID)
    # change APP_NAME to the share library name for Android, it's value depend on AndroidManifest.xml
    set(APP_NAME MyGame)
    list(APPEND GAME_SOURCE
         proj.android/app/jni/hellocpp/main.cpp
         )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         proj.linux/main.cpp
         )
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         proj.win32/resource.h
         )
    list(APPEND GAME_SOURCE
         proj.win32/main.cpp
         proj.win32/game.rc
         ${common_res_files}
         )
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/RootViewController.h
             )
        set(APP_UI_RES
            proj.ios_mac/ios/LaunchScreen.storyboard
            proj.ios_mac/ios/LaunchScreenBackground.png
            proj.ios_mac/ios/Images.xcassets
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             proj.ios_mac/ios/Prefix.pch
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(APP_UI_RES
            proj.ios_mac/mac/Icon.icns
            proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_res_files})
endif()

# mark app complie info and libs info


#set_target_properties(libwren PROPERTIES IMPORTED_LOCATION
#  "${COCOS2DX_ROOT_PATH}/../wren/build/libwren.a")


set(APP_SRC
    ${GAME_SOURCE}
    ${GAME_HEADER}
    )
cocos_build_app(${APP_NAME}
                APP_SRC "${APP_SRC}"
                DEPEND_COMMON_LIBS "cocos2d" "libwren"
                DEPEND_ANDROID_LIBS "cocos2d_android"
                )

if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES
                              MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/Info.plist"
                              )
    elseif(IOS)
        cocos_pak_xcode(${APP_NAME} INFO_PLIST "iOSBundleInfo.plist.in")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
    endif()
elseif(WINDOWS)
    cocos_copy_target_dll(${APP_NAME} COPY_TO ${APP_BIN_DIR})
endif()

if(LINUX OR WINDOWS)
    cocos_copy_target_res(${APP_NAME} COPY_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()
