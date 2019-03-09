#pragma once

#include "cocos2d.h"

namespace yaadv {
enum class Easing : int {
  QuadIn = 11,
  QuadOut = 12,
  QuadInOut = 13,
  ElasticIn = 21,
  ElasticOut = 22,
  ElasticInOut = 23,
  ExpoIn = 31,
  ExpoOut = 32,
  ExpoInOut = 33,
  BackIn = 41,
  BackOut = 42,
  BackInOut = 43,
  BounceIn = 51,
  BounceOut = 52,
  BounceInOut = 53
};

class Between {
 private:
  const static int UNDEFINED_POSITION;
  const static int MAX_OPACITY;

  static cocos2d::ActionInterval* getEasing(cocos2d::ActionInterval* action,
                                            float duration, Easing easing);

 public:
  static cocos2d::Spawn* to(cocos2d::Node* node, std::string to, float duration,
                            Easing easing,
	  bool relative=false);
  static cocos2d::Spawn* from(cocos2d::Node* node, std::string from,
                              float duration, Easing easing, bool relative=false);
  static cocos2d::Spawn* tween(cocos2d::Node* node, std::string to,
                               std::string from, float duration, Easing easing, bool relative=false);
  static Easing easing(std::string&s);
};
};  // namespace yaadv