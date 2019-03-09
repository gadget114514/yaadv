#include "Slidebar.h"
using namespace yaadv;
using namespace yaadv::rx;


namespace yaadv {

	namespace rx {

Slidebar::Slidebar(Sprite *backgroundSprite, Sprite *pointSprite) {
  _background = backgroundSprite;
  this->addChild(_background);
  _point = pointSprite;
  this->addChild(_point);
  _pointPositionX = _point->getPositionX();
  _maxWidth =
      (_background->getContentSize().width - _point->getContentSize().width) /
      2;
  _maxValue = 1.0f;
  _minValue = 0.0f;
  _targetValue = nullptr;
  _floatValue = 0.5f;
  _controlEvent = createControlEvent();
  this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
      _controlEvent, this);
  touchEvent = [=]() {};
  moveEvent = [=]() {};
}

Slidebar::~Slidebar() {}

Slidebar *Slidebar::createSlidebar(std::string backgroundFile,
                                   std::string pointFile) {
  auto tmpBackgroundSprite = Sprite::create(backgroundFile);
  auto tmpPointSprite = Sprite::create(pointFile);
  if (tmpBackgroundSprite && tmpPointSprite) {
    auto tmpSlidebar = new Slidebar(tmpBackgroundSprite, tmpPointSprite);
    tmpSlidebar->autorelease();
    return tmpSlidebar;
  } else {
    if (!tmpBackgroundSprite)
      log("Sliderbar background file (%s) not found.", backgroundFile.c_str());
    if (!tmpPointSprite)
      log("Sliderbar point file (%s) not found.", pointFile.c_str());
    return nullptr;
  }
}

EventListenerTouchOneByOne *Slidebar::createControlEvent() {
  auto et = EventListenerTouchOneByOne::create();

  et->onTouchBegan = [=](Touch *t, Event *e) {
    if (_point->getBoundingBox().containsPoint(
            this->convertTouchToNodeSpace(t))) {
      return true;
    } else {
      if (_background->getBoundingBox().containsPoint(
              this->convertTouchToNodeSpace(t))) {
        float setP = this->convertTouchToNodeSpace(t).x;

        if (setP < _background->getPositionX() - _maxWidth)
          setP = _background->getPositionX() - _maxWidth;
        if (setP > _background->getPositionX() + _maxWidth)
          setP = _background->getPositionX() + _maxWidth;
        _point->setPositionX(setP);
        _pointPositionX = _point->getPositionX();
        return true;
      }
      return false;
    }
  };

  et->onTouchMoved = [=](Touch *t, Event *e) {
    float xOff = t->getLocation().x - t->getStartLocation().x;
    float LLimit = _background->getPositionX() - _pointPositionX - _maxWidth;
    float RLimit = _background->getPositionX() - _pointPositionX + _maxWidth;

    if (xOff < LLimit) xOff = LLimit;
    if (xOff > RLimit) xOff = RLimit;

    _point->setPositionX(_pointPositionX + xOff);

    _change =
        (_point->getPositionX() - (_background->getPositionX() - _maxWidth)) /
        (_maxWidth * 2);
    changeTargetValue(_change);
    _floatValue = (_maxValue - _minValue) * _change;

    moveEvent();
  };

  et->onTouchEnded = [=](Touch *t, Event *e) {

    _pointPositionX = _point->getPositionX();

    _change = (_pointPositionX - (_background->getPositionX() - _maxWidth)) /
              (_maxWidth * 2);
    changeTargetValue(_change);
    _floatValue = (_maxValue - _minValue) * _change;
    touchEvent();
  };

  return et;
}

void Slidebar::setMaxWidth(float value) { _maxWidth = value; }

void Slidebar::changeTargetValue(float change) {
  if (_targetValue) {
    *_targetValue = *_targetValue * change;
  }
}

void Slidebar::setMaxValue(float value) {
  _maxValue = value;
  setFloat(_floatValue);
}

void Slidebar::setMinValue(float value) {
  _minValue = value;
  setFloat(_floatValue);
}

float Slidebar::getChange() { return _change; }

void Slidebar::setSlidebar() {
  _point->setPosition(
      (_background->getPositionX() - _maxWidth) + _maxWidth * 2 * _change,
      _point->getPositionY());
  _pointPositionX = _point->getPositionX();
}

void Slidebar::setFloat(float value) {
  if (value > _maxValue || value < _minValue) {
    log("Bad value!");
    return;
  }
  _floatValue = value;
  _change = value / (_maxValue - _minValue);
  setSlidebar();
}

float Slidebar::getFloat() { return _floatValue; }

} 

}  // namespace yaadv