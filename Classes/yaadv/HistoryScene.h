#ifndef HISTORYSCENE_H
#define HISTORYSCENE_H
#include "History.h"
#include "cocos2d.h"
USING_NS_CC;

namespace yaadv {
class HistoryMessage : public Node {
  Label *_nameLabel;
  Label *_textLabel;
  Sprite *_voice;
  float _height;

 public:
  HistoryMessage(std::string text, std::string name, Color4B color);
  ~HistoryMessage();
  static HistoryMessage *create(record *record);
  float getHeight();
};

class HistoryScene : public Layer {
  int pointPositionY;

 public:
  HistoryScene();
  ~HistoryScene();

  static Scene *createScene();

  virtual bool init();

  CREATE_FUNC(HistoryScene);

  void back();
};

}  // namespace yaadv

#endif
