#/****************************************************************************
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
	${COCOS2DX_ROOT_PATH}/../yasl
)
# add cross-platforms source files and header files 
list(APPEND GAME_SOURCE
     Classes/AppDelegate.cpp
     Classes/HelloWorldScene.cpp
Classes/LoadScene.cpp
Classes/MainMenuScene.cpp
Classes/HelloWorldScene.cpp
Classes/AppDelegate.cpp
Classes/ScriptReader/BackgroundManager.cpp
Classes/ScriptReader/SCStopSound.cpp
Classes/ScriptReader/SCStopBGM.cpp
Classes/ScriptReader/ScriptReader.cpp
Classes/ScriptReader/SCCharacterSpeak.cpp
Classes/ScriptReader/BackgroundMusicManager.cpp
Classes/ScriptReader/SCIf.cpp
Classes/ScriptReader/SCLeave.cpp
Classes/ScriptReader/SCSelect.cpp
Classes/ScriptReader/SCSet.cpp
Classes/ScriptReader/SoundManager.cpp
Classes/ScriptReader/SCJump.cpp
Classes/ScriptReader/SCPlaySound.cpp
Classes/ScriptReader/SCPlayBGM.cpp
Classes/ScriptReader/CharacterManager.cpp
Classes/ScriptReader/SCShowFg.cpp
Classes/ScriptReader/SCBackground.cpp
Classes/ScriptReader/SCGameOver.cpp
Classes/ScriptReader/ScriptCommand.cpp
Classes/HistoryScene.cpp
Classes/SaveData.cpp
Classes/CharLabel.cpp
Classes/TestLayer.cpp
Classes/SaveScene.cpp
Classes/Control/RadioButton.cpp
Classes/Control/EasyButton.cpp
Classes/Control/Slidebar.cpp
Classes/Control/PopupLayer.cpp
Classes/TestFolder/TestInFolder.cpp
Classes/SettingScene.cpp
Classes/GameScene.cpp
Classes/AudioHelper.cpp
Classes/GameSystem.cpp
Classes/History.cpp
Classes/ini.c
     )
list(APPEND GAME_HEADER
     Classes/AppDelegate.h
     Classes/HelloWorldScene.h
Classes/MainMenuScene.h
Classes/GameScene.h
Classes/ScriptReader/SCLeave.h
Classes/ScriptReader/CharacterManager.h
Classes/ScriptReader/BackgroundManager.h
Classes/ScriptReader/ScriptReader.h
Classes/ScriptReader/SCPlaySound.h
Classes/ScriptReader/SCPlayBGM.h
Classes/ScriptReader/SCSet.h
Classes/ScriptReader/ScriptCommand.h
Classes/ScriptReader/SoundManager.h
Classes/ScriptReader/SCStopBGM.h
Classes/ScriptReader/SCStopSound.h
Classes/ScriptReader/BackgroundMusicManager.h
Classes/ScriptReader/SCBackground.h
Classes/ScriptReader/SCCharacterSpeak.h
Classes/ScriptReader/SCJump.h
Classes/ScriptReader/SCSelect.h
Classes/ScriptReader/SCIf.h
Classes/ScriptReader/SCGameOver.h
Classes/AudioHelper.h
Classes/GameSystem.h
Classes/HelloWorldScene.h
Classes/SettingScene.h
Classes/Control/RadioButton.h
Classes/Control/Slidebar.h
Classes/Control/EasyButton.h
Classes/TestFolder/TestInFolder.h
Classes/HistoryScene.h
Classes/TestLayer.h
Classes/SaveData.h
Classes/SaveScene.h
Classes/CharLabel.h
Classes/AppDelegate.h
Classes/LoadScene.h
Classes/ini.h
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

add_library(yasllib STATIC IMPORTED)
set_target_properties(yasllib PROPERTIES IMPORTED_LOCATION
  "${COCOS2DX_ROOT_PATH}/../yasl/build/libyaslapi.a")


set(APP_SRC
    ${GAME_SOURCE}
    ${GAME_HEADER}
    )
cocos_build_app(${APP_NAME}
                APP_SRC "${APP_SRC}"
                DEPEND_COMMON_LIBS "cocos2d" "yasllib"
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