#ifndef RADIO_BUTTON_H
#define RADIO_BUTTON_H

#include "EasyButton.h"
#include "cocos2d.h"

USING_NS_CC;
namespace yaadv {
namespace rx {
class RadioButton : public Node {
  std::vector<EasyButton *> *_radios;
  int _selectedNumber;
  int _tmpSelectedNumber;
  EasyButton *_selectedButton;
  EasyButton *_tmpSelectedButton;
  EventListenerTouchOneByOne *_eventTouch;

 public:
  RadioButton();
  ~RadioButton();
  static RadioButton *createRadioButton(EasyButton *i, ...);
  std::function<void()> touchEvent;

  void onTouchBegan(Touch *t, Event *e);
  void onTouchEnded(Touch *t, Event *e, bool flag);

  int getSelectedNumber();
  void setSelectedNumber(int selectedNumber);
};
};  // namespace rx
};  // namespace yaadv
#endif