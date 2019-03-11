

#include "PopupLayer.hpp"
#include "../ScriptReader/yaadvConfig.h"

USING_NS_CC;
using namespace yaadv;

namespace yaadv {
PopupLayer::PopupLayer(const std::string &backgroundImage) {
  Config &vars = *Config::getInstance();
  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  _backgroundLayer = LayerColor::create(Color4B(0, 0, 0, 64));
  this->addChild(_backgroundLayer);

  _backgroundSprite = ui::Scale9Sprite::create(backgroundImage);
  _backgroundSprite->cocos2d::Node::setAnchorPoint(Vec2::ANCHOR_MIDDLE);
  _backgroundSprite->setPosition(visibleSize.width / 2 + origin.x,
                                 visibleSize.height / 2 + origin.y);
  this->addChild(_backgroundSprite);

  _buttonCounter = 0;

  _buttonList = new std::vector<PopupButton *>;

  _buttonPositionY = _backgroundSprite->getContentSize().height / 2 -
                     _backgroundSprite->getContentSize().height / 3;

  _buttonMenu = Menu::create();
  _buttonMenu->setPosition(0, _buttonPositionY);
  this->addChild(_buttonMenu);

  _popupTextFont = DEFAULT_TEXT_FONT;
  _popupTextSize = DEFAULT_TEXT_SIZE;
  _popupTextPositionX = _backgroundSprite->getContentSize().width / 2;
  _popupTextPositionY = (_backgroundSprite->getContentSize().height / 3) * 2;
  _popupText = Label::createWithSystemFont("Are you sure?", _popupTextFont,
                                           _popupTextSize);
  _popupText->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
  _popupText->setPosition(_popupTextPositionX, _popupTextPositionY);
  _backgroundSprite->addChild(_popupText);

  auto *backgroundTouch = EventListenerTouchOneByOne::create();
  backgroundTouch->onTouchBegan = [=](Touch *t, Event *e) { return true; };
  backgroundTouch->onTouchEnded = [=](Touch *t, Event *e) {
    this->runAction(Sequence::create(
        CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, this)), NULL));
  };
  backgroundTouch->setSwallowTouches(true);
  _backgroundLayer->getEventDispatcher()
      ->addEventListenerWithSceneGraphPriority(backgroundTouch,
                                               _backgroundLayer);

  auto *backgroundSpriteTouch = EventListenerTouchOneByOne::create();
  backgroundSpriteTouch->onTouchBegan = [=](Touch *t, Event *e) {
    if (_backgroundSprite->getBoundingBox().containsPoint(
            this->convertTouchToNodeSpace(t))) {
      return true;
    }
    return false;
  };
  backgroundSpriteTouch->setSwallowTouches(true);
  _backgroundSprite->getEventDispatcher()
      ->addEventListenerWithSceneGraphPriority(backgroundSpriteTouch,
                                               _backgroundSprite);
}

PopupLayer::~PopupLayer() {
  if (_buttonList) {
    for (auto i = _buttonList->begin(); i != _buttonList->end(); i++) {
      auto btn = *i;
      CC_SAFE_DELETE(btn);
    }
    _buttonList->clear();
    CC_SAFE_DELETE(_buttonList);
  }
}

PopupLayer *PopupLayer::create(const std::string &backgroundImage) {
  PopupLayer *popupLayer = new PopupLayer(backgroundImage);
  popupLayer->autorelease();
  return popupLayer;
}

void PopupLayer::addButton(const std::string &normalImage,
                           std::function<void(Ref *)> &callback) {
  addButton(normalImage, normalImage, normalImage, callback);
}

void PopupLayer::addButton(const std::string &normalImage,
                           const std::string &selectedImage,
                           std::function<void(Ref *)> &callback) {
  addButton(normalImage, selectedImage, selectedImage, callback);
}

void PopupLayer::addButton(const std::string &normalImage,
                           const std::string &touchImage,
                           const std::string &selectedImage,
                           std::function<void(Ref *)> &callback) {}

void PopupLayer::addLabelButton(const std::string &buttonText,
                                const ccMenuCallback &callback) {
  auto button = MenuItemFont::create(buttonText, callback);

  auto *buttonTouch = EventListenerTouchOneByOne::create();

  buttonTouch->onTouchBegan = [=](Touch *t, Event *e) {

    if (button->getBoundingBox().containsPoint(
            _backgroundSprite->convertTouchToNodeSpace(t))) {
      return true;
    }
    return false;
  };

  buttonTouch->onTouchEnded = [=](Touch *t, Event *e) {

    if (button->getBoundingBox().containsPoint(
            _backgroundSprite->convertTouchToNodeSpace(t))) {
      this->runAction(Sequence::create(
          CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, this)), NULL));
    }
  };

  button->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
      buttonTouch, button);

  auto popupButton = new PopupButton;
  popupButton->button = button;
  popupButton->buttonType = PopupButton::ButtonType::FONT;

  _buttonCounter++;
  _buttonList->push_back(popupButton);

  arrange();
}

void PopupLayer::arrange() {
  auto tempX = _backgroundSprite->getContentSize().width / (_buttonCounter + 1);
  _buttonMenu->removeFromParent();
  _buttonMenu->removeAllChildren();

  Vector<MenuItem *> buttonVector;
  for (int i = 0; i < _buttonCounter; i++) {
    _buttonList->at(i)->button->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _buttonList->at(i)->button->setPosition(tempX * (i + 1), _buttonPositionY);

    buttonVector.pushBack(_buttonList->at(i)->button);
  }
  _buttonMenu = Menu::createWithArray(buttonVector);
  _buttonMenu->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
  _buttonMenu->setPosition(0, 0);
  _backgroundSprite->addChild(_buttonMenu);
}

void PopupLayer::setString(const std::string &text) {
  _popupText->setString(text);
}

} // namespace yaadv
