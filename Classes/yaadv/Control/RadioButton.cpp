#include "RadioButton.h"

USING_NS_CC;
using namespace yaadv;
using namespace yaadv::rx;
namespace yaadv {
	namespace rx {

RadioButton::RadioButton()
    : _selectedNumber(0),
      _tmpSelectedNumber(0),
      _selectedButton(nullptr),
      _tmpSelectedButton(nullptr) {
  _radios = new std::vector<EasyButton *>;
  touchEvent = [=]() {};
  _eventTouch = EventListenerTouchOneByOne::create();
  _eventTouch->setSwallowTouches(true);
  _eventTouch->onTouchBegan = [=](Touch *t, Event *e) {
    _tmpSelectedNumber = 0;
    for (auto it = _radios->begin(); it != _radios->end(); it++) {
      if ((*it)->getNormal()->getBoundingBox().containsPoint(
              (*it)->convertTouchToNodeSpace(t))) {
        _tmpSelectedButton = (*it);
        if (_tmpSelectedButton == _selectedButton) return false;
        (*it)->onTouchBegan(t, e);
        return true;
      }
      _tmpSelectedNumber++;
    }
    _tmpSelectedButton = nullptr;
    return false;
  };

  _eventTouch->onTouchEnded = [=](Touch *t, Event *e) {

    if (_tmpSelectedButton->getNormal()->getBoundingBox().containsPoint(
            _tmpSelectedButton->convertTouchToNodeSpace(t))) {
      if (_selectedButton) _selectedButton->onTouchEnded(t, e, false);
      _selectedButton = _tmpSelectedButton;
      _selectedNumber = _tmpSelectedNumber;
      onTouchEnded(t, e, true);
    } else {
      _tmpSelectedButton->onTouchEnded(t, e, false);
    }

  };

  this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
      _eventTouch, this);
}

RadioButton::~RadioButton() {
  _radios->clear();
  CC_SAFE_DELETE(_radios);
}

RadioButton *RadioButton::createRadioButton(EasyButton *i, ...) {
  auto tmpRadioButton = new RadioButton();
  tmpRadioButton->autorelease();

  va_list argPtr;
  va_start(argPtr, i);
  EasyButton *p = i;
  while (p != nullptr) {
    p->setEventTouchEnabled(false);
    tmpRadioButton->_radios->push_back(p);
    tmpRadioButton->addChild(p);
    p = va_arg(argPtr, EasyButton *);
  }
  va_end(argPtr);

  return tmpRadioButton;
}

void RadioButton::onTouchBegan(Touch *t, Event *e) {}

void RadioButton::onTouchEnded(Touch *t, Event *e, bool flag) { touchEvent(); }

int RadioButton::getSelectedNumber() { return _selectedNumber; }

void RadioButton::setSelectedNumber(int selectedNumber) {
  if (selectedNumber > _radios->size() - 1 || selectedNumber < 0) {

    return;
  }
  if (_selectedButton) _selectedButton->onTouchEnded(nullptr, nullptr, false);

  _selectedButton = _radios->at(selectedNumber);
  _selectedButton->onTouchBegan(nullptr, nullptr);

  _selectedNumber = selectedNumber;
}


}  // namespace rx

}  // namespace yaadv
