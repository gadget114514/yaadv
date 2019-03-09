#ifndef MAINMENUSCENE_H
#define MAINMENUSCENE_H

#include "cocos2d.h"
#include "yaadv.h"
namespace yaadv {

class MainMenu : public cocos2d::Layer {
  cocos2d::DrawNode *drawNode;

  void update(float dt);

  struct Rain {
    float x;
    float y;
    float landLen;
    float len;
    float alpha;
    bool isLand;
  };

  struct RainOver {
    float x;
    float y;
    float r;
    float alpha;
  };

  std::vector<Rain *> rains;
  std::vector<RainOver *> rainOvers;

  void appendRain();
  bool rain;

 public:
  MainMenu();
  ~MainMenu();

  static cocos2d::Scene *createScene();

  virtual bool init();

  CREATE_FUNC(MainMenu);

  void menuExit(Ref *pSender);

  void newgame();
  void config();
  void load();
};
};  // namespace yaadv

#endif
