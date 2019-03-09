#include "CharLabel.h"

USING_NS_CC;

USING_NS_CC;
using namespace yaadv;
namespace yaadv {
CharLabel::CharLabel()
    : _pos(0),
      _defaultDelayTime(1.0f),
      _fadeTime(0.5f),
      _lineHeight(0),
      _charLabel(nullptr),
      _x(0),
      _y(0),
      _defaultFontSize(20),
      _showFinishCallback(nullptr),
      _defaultFontColor(Color3B::WHITE),
      _isRunning(false) {}

CharLabel::~CharLabel() {}

CharLabel *CharLabel::create(const std::string text, int fontSize,
                             std::function<void()> callback) {
  CharLabel *ret = new CharLabel();
  if (ret && ret->init(text, fontSize)) {
    ret->_showFinishCallback = callback;
    ret->autorelease();
    return ret;
  }
  CC_SAFE_DELETE(ret);
  return nullptr;
}

bool CharLabel::init(const std::string text, int fontSize) {
  _text = text;
  _defaultFontSize = fontSize;
  showNextChar();
  return true;
}

void CharLabel::setString(const std::string text) {
  Node::removeAllChildren();

  _charLabel = nullptr;
  _text = text;
  _pos = 0;
  _x = 0;
  _y = 0;
  _isRunning = true;

  showNextChar();
}

void CharLabel::finishShow() {
  removeAllChildren();
  _charLabel = Label::createWithSystemFont(_text, "arial", _defaultFontSize);
  _charLabel->setDimensions(this->getContentSize().width,
                            this->getContentSize().height);
  _charLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
  _charLabel->setColor(_defaultFontColor);

  _charLabel->enableShadow(Color4B(0, 0, 0, 200), Size(2, -2), 1);
  Node::addChild(_charLabel);

  _isRunning = false;

  if (_showFinishCallback) {
    _showFinishCallback();
  }
}

void CharLabel::showNextChar() {
  if ((_text.length() > 0 && _text[_text.length() - 1] == 13) ||
      _pos >= (int)_text.length()) {
    _isRunning = false;
    if (_showFinishCallback) {
      _showFinishCallback();
    }
    return;
  }

  auto charString = getNextChar(_text, _pos);

  if (_charLabel && _lineHeight < _charLabel->getContentSize().height) {
    _lineHeight = _charLabel->getContentSize().height;
  }

  while (charString.compare("\n") == 0) {
    _y -= _lineHeight;
    _x = 0;
    _pos += charString.length();
    charString = getNextChar(_text, _pos);
  }

  auto tmpLabel =
      Label::createWithSystemFont(charString, "arial", _defaultFontSize);
  tmpLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
  Node::addChild(tmpLabel);
  tmpLabel->setColor(_defaultFontColor);
  tmpLabel->enableShadow(Color4B(0, 0, 0, 200), Size(2, -2), 1);

  _pos += charString.length();

  tmpLabel->setPosition(_x, _y);
  if (_x + tmpLabel->getContentSize().width > this->getContentSize().width) {
    _y -= tmpLabel->getContentSize().height;
    _x = 0;
  } else {
    _x += tmpLabel->getContentSize().width;
  }

  tmpLabel->runAction(Sequence::createWithTwoActions(
      DelayTime::create(_defaultDelayTime),
      CallFunc::create(CC_CALLBACK_0(CharLabel::showNextChar, this))));
  _charLabel = tmpLabel;
}

std::string CharLabel::getNextChar(const std::string src, int pos) {
  if (src.length() < 1) {
    return std::string("");
  }

  int len = 0;
  char c = src[pos];

  if (c >= 0 && c <= 127) {
    len++;
  } else if (c < 0 && c >= -32) {
    len += 3;
  } else if (c < 0 && c >= -64) {
    len += 2;
  } else if (c < 0) {
    len += 1;
  }
  std::string subString = src.substr(pos, len);
  return subString;
}

}  // namespace yaadv