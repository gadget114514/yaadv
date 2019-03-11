#ifndef EASYBUTTON_H
#define EASYBUTTON_H

#include "cocos2d.h"

USING_NS_CC;

namespace yaadv {

class EasyButton : public Node {
  Sprite *_normal;
  Sprite *_touch;
  Sprite *_selected;
  EventListenerTouchOneByOne *_eventTouch;

 public:
  EasyButton(Sprite *_normalSprite, Sprite *_touchSprite, Sprite *_selectedSprite);
  ~EasyButton();
  static EasyButton *createEasyButton(const std::string &_normalFile);
  static EasyButton *createEasyButton(const std::string &_normalFile, const std::string &_touchFile);
  static EasyButton *createEasyButton(const std::string &_normalFile, const std::string &_touchFile,
                                      const std::string &_selectedFile);

  void onTouchBegan(Touch *t, Event *e);
  void onTouchMoved(Touch *t, Event *e);
  void onTouchEnded(Touch *t, Event *e, bool flag);

  void setEventTouchEnabled(bool flag);
  Sprite *getNormal();

  std::function<void()> touchEvent;
};
};  // namespace yaadv

#endif