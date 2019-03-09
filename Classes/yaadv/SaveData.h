#ifndef SAVEDATA_H
#define SAVEDATA_H
#include "GameSystem.h"
#include "cocos2d.h"

#define TEXT_MAX_LENGTH 500

USING_NS_CC;

namespace yaadv {
class SaveData : public Node {
  std::string _imageFile;
  int _number;
  std::string _text;
  std::string _other;
  std::string _date;

  Sprite *_dataImage;
  Label *_dataText;
  Label *_dataDate;

  Sprite *_stageLayer;

 public:
  SaveData(int number, std::string imageFile, std::string text,
           std::string date);
  SaveData(int number);
  ~SaveData();
  void setOther();
  static SaveData *create(int i);
  std::function<void(int)> onTouchEnded;
  void updataData(bool isNeedReloadImage);
  Sprite *getStageLayer();
};
};  // namespace yaadv

#endif
