#ifndef __CHARATOR_MANAGER_H__
#define __CHARATOR_MANAGER_H__

#include "cocos2d.h"

USING_NS_CC;

#define CM CharacterManager::getInstance()

namespace yaadv {

enum CMPositionType { EMPTY, LEFT, LEFT_CENTER, CENTER, RIGHT_CENTER, RIGHT };

class Bamiri {
 public:
  float x, y;
  float anchorx, anchory;
  float scalex, scaley;
  float angle;
  float fadetime;
  std::string action;
  Bamiri() {
    x = 0;
    y = 0;
    anchorx = 0.5;
    anchory = 0;
    scalex = 1.0;
    scaley = 1.0;
    angle = 0;
    fadetime = 0.5;
  }
  Bamiri(float x, float y, float anchorx, float anchory, float scalex, float scaley, float angle, float fadetime,
         std::string action = "") {
    this->x = x;
    this->y = y;
    this->scalex = scalex;
    this->scaley = scaley;
    this->anchorx = anchorx;
    this->anchory = anchory;
    this->angle = angle;
    this->fadetime = fadetime;
    this->action = action;
  }
};

static Bamiri _defaultbamiri;

struct Character {
  std::string key;
  std::string name;
  Color3B color;

  std::map<std::string, char *> *fgList;
  std::map<std::string, Bamiri *> *bamiriList;
  int favor;
  int hate;

  Sprite *_sprite;
  std::string currentFace;
  bool positionIsLeft;
  CMPositionType currentPosition;

  Character()
      : name("noname"), color(Color3B::BLACK), fgList(nullptr), favor(0), hate(0), _sprite(nullptr), currentFace("") {
    fgList = new std::map<std::string, char *>;
    bamiriList = new std::map<std::string, Bamiri *>;
  }

  ~Character() {
    if (fgList) {
      for (auto i = fgList->begin(); i != fgList->end(); i++) {
        auto value = i->second;
        CC_SAFE_DELETE_ARRAY(value);
      }

      fgList->clear();
      CC_SAFE_DELETE(fgList);
    }
    if (bamiriList) {
      bamiriList->clear();
      CC_SAFE_DELETE(bamiriList);
    }
  }

  char *getCharacterFace(std::string &face) {
    auto result = fgList->find(face);
    if (result != fgList->end()) {
      if (strlen(result->second) < 1) {
        return nullptr;
      }
      return result->second;
    } else {
      return nullptr;
    }
  }

  Bamiri *getBamiri(std::string &face) {
    auto result = bamiriList->find(face);
    if (result != bamiriList->end()) {
      return result->second;
    } else {
      return &_defaultbamiri;
    }
  }

  void moveTo(CMPositionType pt, bool immediately = false) {
    float width = Director::getInstance()->getRunningScene()->getContentSize().width;
    if (width == 0) {
      width = Director::getInstance()->getVisibleSize().width;
    }
    log(">>>> Screen Width: %d", (int)width);
    switch (pt) {
      case CMPositionType::LEFT: {
        setAction(width / 4, immediately);
        break;
      }
      case CMPositionType::LEFT_CENTER: {
        setAction(width / 3, immediately);
        break;
      }
      case CMPositionType::CENTER: {
        setAction(width / 2, immediately);
        break;
      }
      case CMPositionType::RIGHT_CENTER: {
        setAction(width * 2 / 3, immediately);
        break;
      }
      case CMPositionType::RIGHT: {
        setAction(width * 3 / 4, immediately);
        break;
      }
      default: { break; }
    }
    currentPosition = pt;
  }

  void setAction(int Distance, bool immediately) {
    if (!_sprite) {
      log("Character:%s _sprite is Null!!", name.c_str());
    } else {
      if (immediately) {
        _sprite->setPosition(Point(Distance, _sprite->getPositionY()));
      } else {
        _sprite->runAction(MoveTo::create(0.3f, Point(Distance, 0)));
      }
    }
  }

  void leave() {
    if (_sprite) {
      log("%s is leaving.", name.c_str());
      _sprite->removeFromParent();
      _sprite = nullptr;
      currentFace = "";
      currentPosition = CMPositionType::EMPTY;
    } else {
    }
  }
};

class CharacterManager {
  static CharacterManager *_instance;

  CharacterManager();
  ~CharacterManager();

  std::map<std::string, Character *> *_pool;

 public:
  static CharacterManager *getInstance();
  static void destoryInstance();

  Character *getCharacter(std::string &key);

  void addCharacter(std::string &key, Character *cha);
};

};  // namespace yaadv

#include "../../lwf/cplusplus/cocos2dx/lwf_cocos2dx.h"

namespace yaadv {

struct YaImg {
  std::string key;
  std::string name;
  Color3B color;

  std::map<std::string, char *> *fgList;
  std::map<std::string, Bamiri *> *bamiriList;
  int favor;
  int hate;

  Sprite *_sprite;
  std::string currentFace;
  bool positionIsLeft;
  CMPositionType currentPosition;

  YaImg()
      : name("NoName"), color(Color3B::BLACK), fgList(nullptr), favor(0), hate(0), _sprite(nullptr), currentFace("") {
    fgList = new std::map<std::string, char *>;
  }

  ~YaImg() {
    if (fgList) {
      for (auto i = fgList->begin(); i != fgList->end(); i++) {
        auto value = i->second;
        CC_SAFE_DELETE_ARRAY(value);
      }

      fgList->clear();
      CC_SAFE_DELETE(fgList);
    }
  }

  char *getFace(std::string &face) {
    auto result = fgList->find(face);
    if (result != fgList->end()) {
      if (strlen(result->second) < 1) {
        return nullptr;
      }
      return result->second;
    } else {
      return nullptr;
    }
  }

  void moveToDefaultBamiri(CMPositionType pt, bool immediately = false) {
    float width = Director::getInstance()->getRunningScene()->getContentSize().width;
    if (width == 0) {
      width = Director::getInstance()->getVisibleSize().width;
    }

    switch (pt) {
      case CMPositionType::LEFT: {
        setAction(width / 4, immediately);
        break;
      }
      case CMPositionType::LEFT_CENTER: {
        setAction(width / 3, immediately);
        break;
      }
      case CMPositionType::CENTER: {
        setAction(width / 2, immediately);
        break;
      }
      case CMPositionType::RIGHT_CENTER: {
        setAction(width * 2 / 3, immediately);
        break;
      }
      case CMPositionType::RIGHT: {
        setAction(width * 3 / 4, immediately);
        break;
      }
      default: { break; }
    }
    currentPosition = pt;
  }

  void moveTo(CMPositionType pt, bool immediately = false) {
    float width = Director::getInstance()->getRunningScene()->getContentSize().width;
    if (width == 0) {
      width = Director::getInstance()->getVisibleSize().width;
    }

    switch (pt) {
      case CMPositionType::LEFT: {
        setAction(width / 4, immediately);
        break;
      }
      case CMPositionType::LEFT_CENTER: {
        setAction(width / 3, immediately);
        break;
      }
      case CMPositionType::CENTER: {
        setAction(width / 2, immediately);
        break;
      }
      case CMPositionType::RIGHT_CENTER: {
        setAction(width * 2 / 3, immediately);
        break;
      }
      case CMPositionType::RIGHT: {
        setAction(width * 3 / 4, immediately);
        break;
      }
      default: { break; }
    }
    currentPosition = pt;
  }

  void setAction(int distance, bool immediately) {
    if (!_sprite) {
    } else {
      if (immediately) {
        _sprite->setPosition(Point(distance, _sprite->getPositionY()));
      } else {
        _sprite->runAction(MoveTo::create(0.3f, Point(distance, 0)));
      }
    }
  }

  void leave() {
    if (_sprite) {
      _sprite->removeFromParent();
      _sprite = nullptr;
      currentFace = "";
      currentPosition = CMPositionType::EMPTY;
    } else {
    }
  }
};

class YaImgManager {
  static YaImgManager *_instance;

  YaImgManager();
  ~YaImgManager();

  std::map<std::string, YaImg *> *_pool;

 public:
  static YaImgManager *getInstance();
  static void destoryInstance();

  YaImg *getYaImg(std::string &key);

  void addYaImg(std::string &key, YaImg *cha);
};

};  // namespace yaadv

#endif
