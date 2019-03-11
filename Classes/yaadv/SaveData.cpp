#include "SaveData.h"
#include "GameSystem.h"
#include "yaadv.h"

USING_NS_CC;
using namespace yaadv;
#define NO_DATA_IMAGE ""
#define MAX_TEXT_COUNT 30

namespace yaadv {

SaveData::SaveData(int number, std::string imageFile, std::string text, std::string date) {}

SaveData::SaveData(int i) : _dataImage(nullptr), _dataDate(nullptr), _dataText(nullptr) {
  Config &vars = *Config::getInstance();

  _stageLayer = Sprite::create(vars["yaadv/ui/sl/btn_button_normal.png"]);

  this->setContentSize(_stageLayer->getContentSize());

  _stageLayer->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
  _stageLayer->setPosition(Vec2(0, this->getContentSize().height));

  _number = i;

  updataData(false);

  onTouchEnded = [=](int i) {};

  auto touchImage = Sprite::create(vars["yaadv/ui/sl/btn_button_touch.png"]);
  touchImage->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
  touchImage->setVisible(false);
  _stageLayer->addChild(touchImage, 0);

  auto eventTouch = EventListenerTouchOneByOne::create();

  eventTouch->onTouchBegan = [=](Touch *t, Event *e) {
    if (_stageLayer->getBoundingBox().containsPoint(this->convertTouchToNodeSpace(t))) {
      touchImage->setVisible(true);
      return true;
    }
    return false;
  };

  eventTouch->onTouchEnded = [=](Touch *t, Event *e) {
    touchImage->setVisible(false);
    if (_stageLayer->getBoundingBox().containsPoint(this->convertTouchToNodeSpace(t))) {
      onTouchEnded(_number);
    } else {
    }
  };

  this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventTouch, this);

  this->addChild(_stageLayer);
}

SaveData::~SaveData() {}

SaveData *SaveData::create(int i) {
  auto tmpSavedata = new SaveData(i);
  tmpSavedata->autorelease();
  return tmpSavedata;
}

void SaveData::updataData(bool isNeedReloadImage) {
  Config &vars = *Config::getInstance();
  auto savedata = GameSystem::getInstance()->getGameSavedata(_number);

  if (_dataImage != nullptr) {
    _dataImage->removeFromParent();
    _dataImage = nullptr;
  }
  if (_dataDate != nullptr) {
    _dataDate->removeFromParent();
    _dataDate = nullptr;
  }
  if (_dataText != nullptr) {
    _dataText->removeFromParent();
    _dataText = nullptr;
  }

  if (savedata) {
    char numberInChar[3];
    sprintf(numberInChar, "%d", _number + 1);
    auto dataNumber = Label::createWithSystemFont((std::string)numberInChar, vars["arial"], 26);
    dataNumber->setPosition(Vec2(25, 105));
    _stageLayer->addChild(dataNumber);

    std::string imageFile = savedata->imageFile;

    _dataImage = Sprite::create(imageFile);

    if (isNeedReloadImage) {
      _dataImage->runAction(Sequence::createWithTwoActions(DelayTime::create(0.1f), CallFunc::create([=]() {
                                                             Director::getInstance()->getTextureCache()->reloadTexture(
                                                                 imageFile);
                                                             _dataImage->setTexture(imageFile);
                                                           })));
    }

    if (_dataImage) {
      _dataImage->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
      _dataImage->setPosition(70, _stageLayer->getContentSize().height / 2);
      _stageLayer->addChild(_dataImage);
    }

    std::string text = savedata->text;

    if (text.length() > MAX_TEXT_COUNT) {
      int len = 0;
      while (len < MAX_TEXT_COUNT - 2) {
        char c = text[len];

        if (c >= 0 && c <= 127) {
          len++;
        } else if (c < 0 && c >= -32) {
          len += 3;
        } else if (c < 0 && c >= -64) {
          len += 2;
        } else if (c < 0) {
          len += 1;
        }
      }
      text = text.substr(0, len);
      text += (std::string) "..";
    }
    _dataText = Label::createWithSystemFont(text, vars["arial"], 26);
    _dataText->setDimensions(TEXT_MAX_LENGTH, 24);
    _dataText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    _dataText->setPosition(255, 55);
    _stageLayer->addChild(_dataText);

    std::string date = savedata->date;

    _dataDate = Label::createWithSystemFont(date, vars["arial"], 26);
    _dataDate->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    _dataDate->setPosition(255, 95);
    _stageLayer->addChild(_dataDate);

  } else {
    char numberInChar[13];
    sprintf(numberInChar, "%d", _number + 1);
    auto dataNumber = Label::createWithSystemFont((std::string)numberInChar, vars["arial"], 26);
    dataNumber->setPosition(Vec2(25, 105));
    _stageLayer->addChild(dataNumber);

    _dataImage = Sprite::create(vars["yaadv/ui/saveload/pic_bg.png"]);
    _dataImage->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    _dataImage->setPosition(70, _stageLayer->getContentSize().height / 2);

    _stageLayer->addChild(_dataImage);

    _dataDate = Label::createWithSystemFont("---------- --:--:--", vars["arial"], 20);
    _dataDate->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    _dataDate->setPosition(255, 95);
    _stageLayer->addChild(_dataDate);
  }
}

Sprite *SaveData::getStageLayer() { return _stageLayer; }

}  // namespace yaadv
