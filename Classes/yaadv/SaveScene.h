#ifndef SAVESCENE_H
#define SAVESCENE_H
#include "SaveData.h"
#include "cocos2d.h"

USING_NS_CC;

#define MAX_PAGE 5

namespace yaadv {
class SaveScene : public Layer {
  SaveData *dataButtons[8];
  EventListenerTouchOneByOne *eventTouch[8];
  int _currentSelectButton;
  Node *_savedataLayer;
  int _currentPage;
  MenuItemImage *_buttonNext;
  MenuItemImage *_buttonFront;

 public:
  SaveScene();
  ~SaveScene();

  static Scene *createScene();

  virtual bool init();

  CREATE_FUNC(SaveScene);

  void back();

  void popup();

  void apply();

  void cancel();

  void loadList(int page);

  void nextPage();
  void frontPage();
};
};  // namespace yaadv

#endif
