#pragma once
#include "SimpleAudioEngine.h"
#include "cocos2d.h"

USING_NS_CC;

class AudioHelper {
 public:
  static CocosDenshion::SimpleAudioEngine *getAudioEngine();
};
