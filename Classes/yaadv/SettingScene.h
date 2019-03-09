#ifndef SETTING_SCENE_H
#define SETTING_SCENE_H

#include "Control/RadioButton.h"
#include "Control/Slidebar.h"
#include "cocos2d.h"

USING_NS_CC;
namespace yaadv {
using namespace yaadv::rx;

class SettingScene : public Layer {
  RadioButton *_isWindow;
  RadioButton *_isSkipAll;

  Slidebar *_musicVolumeSlidebar;
  Slidebar *_soundVolumeSlidebar;
  Slidebar *_autoSpeedSlidebar;
  Slidebar *_skipSpeedSlidebar;
  Slidebar *_textSpeedSlb;

 public:
  SettingScene();
  ~SettingScene();

  static Scene *createScene();

  virtual bool init();

  CREATE_FUNC(SettingScene);

  void back();
  void test();
  void changeMusicVolume();
  void changeSoundVolume();
  void changeTextSpeed();
  void changeAutoSpeed();
  void changeIsSkipAll();
};
};

#endif
