#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__
#include "cocos2d.h"

USING_NS_CC;

#define SM SoundManager::getInstance()
namespace yaadv {
class SoundManager {
  SoundManager();
  ~SoundManager();
  static SoundManager *_instance;

  std::map<std::string, std::string> *_pool;

  std::string defaultSound;

 public:
  static SoundManager *getInstance();
  static void destoryInstance();

  std::string getSound(std::string key);

  void addSound(std::string key, std::string sound);
};
};  // namespace yaadv

#endif