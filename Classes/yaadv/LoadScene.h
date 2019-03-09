#ifndef LOADSCENE_H
#define LOADSCENE_H
#include "SaveData.h"
#include "cocos2d.h"

USING_NS_CC;

#define MAX_PAGE 5
namespace yaadv {
class LoadScene : public Layer {
  SaveData *dataButtons[20];
  EventListenerTouchOneByOne *eventTouch[20];
  int _currentSelectButton;
  Node *_savedataLayer;
  int _currentPage;
  MenuItemImage *_buttonNext;
  MenuItemImage *_buttonFront;

 public:
  LoadScene();
  ~LoadScene();

  static Scene *createScene();

  virtual bool init();

  CREATE_FUNC(LoadScene);

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
