#ifndef SLIDEBAR_H
#define SLIDEBAR_H

#include "cocos2d.h"

USING_NS_CC;
namespace yaadv {
namespace rx {
class Slidebar : public Node {
  Sprite *_background;
  Sprite *_point;
  float _maxValue;
  float _minValue;
  float *_targetValue;
  EventListenerTouchOneByOne *_controlEvent;
  float _pointPositionX;
  float _maxWidth;
  float _change;
  float _floatValue;

 public:
  Slidebar(Sprite *backgroundSprite, Sprite *pointSprite);
  ~Slidebar();
  static Slidebar *createSlidebar(std::string backgroundFile,
                                  std::string pointFile);
  void setLinkValue(int &value);
  void setMaxValue(float value);
  void setMinValue(float value);
  void valueChange();
  EventListenerTouchOneByOne *createControlEvent();
  void setMaxWidth(float value);
  void setTargetValue(float *value);
  void changeTargetValue(float change);
  float getChange();
  void setSlidebar();
  void setFloat(float value);
  float getFloat();

  std::function<void()> touchEvent;
  std::function<void()> moveEvent;
};
};  // namespace rx
};  // namespace yaadv
#endif