#ifndef __BACKGROUND_MUSIC_MANAGER_H__
#define __BACKGROUND_MUSIC_MANAGER_H__

#include "cocos2d.h"

USING_NS_CC;

#define BMM BackgroundMusicManager::getInstance()

namespace yaadv {
class BackgroundMusicManager {
  BackgroundMusicManager();
  ~BackgroundMusicManager();
  static BackgroundMusicManager *_instance;

  std::map<std::string, std::string> *_pool;

  std::string defaultBackgroundMusic;

 public:
  static BackgroundMusicManager *getInstance();
  static void destoryInstance();

  std::string getBackgroundMusic(std::string key);

  void addBackgroundMusic(std::string key, std::string bgm);
};
};

#endif