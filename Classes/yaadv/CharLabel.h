#pragma once
#ifndef __CHAR_LABEL_H__
#define __CHAR_LABEL_H__
#include "cocos2d.h"

USING_NS_CC;
#include "yaadv.h"

namespace yaadv {

class CharLabel : public cocos2d::Node {
  Label *_charLabel;
  int _pos;
  float _defaultDelayTime;
  float _fadeTime;
  int _defaultFontSize;
  std::string _text;

  bool _isRunning;

  Color3B _defaultFontColor;

  float _lineHeight;
  float _x;
  float _y;

  void showNextChar();

  std::function<void()> _showFinishCallback;

 public:
  CharLabel();
  ~CharLabel();

  static CharLabel *create(const std::string text, int fontSize = 24, std::function<void()> callback = nullptr);
  bool init(const std::string text, int fontSize);

  void setString(const std::string text);

  void finishShow();

  static std::string getNextChar(const std::string src, int pos);

  void setShowFinishCallback(std::function<void()> callback) { _showFinishCallback = callback; }

  void setColor(Color3B color) { _defaultFontColor = color; }

  bool isRunning() { return _isRunning; }
};
};  // namespace yaadv

#endif