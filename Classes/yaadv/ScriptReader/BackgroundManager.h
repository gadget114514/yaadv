#ifndef BACKGROUNDMANAGER_H
#define BACKGROUNDMANAGER_H

#include "cocos2d.h"

USING_NS_CC;

#define BM BackgroundManager::getInstance()

namespace yaadv {
class BackgroundManager {
  BackgroundManager();
  ~BackgroundManager();
  static BackgroundManager *_instance;

  std::map<std::string, std::string> *_pool;

  std::string defaultBackground;

 public:
  static BackgroundManager *getInstance();
  static void destoryInstance();

  std::string getBackground(std::string key);

  void addBackground(std::string key, std::string background);
};
};  // namespace adv

#endif
