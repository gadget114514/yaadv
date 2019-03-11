#include "SaveScene.h"
#include <stdlib.h>
#include "Control/PopupLayer.hpp"
#include "GameSystem.h"
#include "yaadv.h"

USING_NS_CC;
using namespace yaadv;

namespace yaadv {
SaveScene::SaveScene() {
  _currentSelectButton = -1;
  _currentPage = 1;
  for (int i = 0; i < 4; i++) {
    eventTouch[i] = nullptr;
  }
}

SaveScene::~SaveScene() {}

Scene *SaveScene::createScene() {
  auto scene = Scene::create();

  auto layer = SaveScene::create();

  scene->addChild(layer);

  return scene;
}

bool SaveScene::init() {
  if (!Layer::init()) {
    return false;
  }

  Config &vars = *Config::getInstance();

  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto stageLayer = Layer::create();

  auto backgroundLayer = LayerColor::create(Color4B::BLACK);
  stageLayer->addChild(backgroundLayer);

  auto backgroundWindow = Sprite::create(vars["yaadv/ui/sl/bg_save.jpg"]);
  backgroundWindow->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
  stageLayer->addChild(backgroundWindow);

  auto titletxt = Sprite::create(vars["yaadv/ui/sl/txt_save.png"]);
  titletxt->setPosition(Vec2(
	  origin.x + 170,
	  visibleSize.height + origin.y - 90));
  stageLayer->addChild(titletxt);

  _savedataLayer = Node::create();
  stageLayer->addChild(_savedataLayer);

  loadList(_currentPage);

  _buttonNext =
      MenuItemImage::create(vars["yaadv/ui/sl/btn_sl_next_normal.png"], vars["yaadv/ui/sl/btn_sl_next_touch.png"],
                            CC_CALLBACK_0(SaveScene::nextPage, this));
  _buttonNext->setPosition(Vec2(1000, 70));
  _buttonFront =
      MenuItemImage::create(vars["yaadv/ui/sl/btn_sl_front_normal.png"], vars["yaadv/ui/sl/btn_sl_front_touch.png"],
                            CC_CALLBACK_0(SaveScene::frontPage, this));
  _buttonFront->setPosition(Vec2(1000, 170));
  auto pageMenu = Menu::create(_buttonNext, _buttonFront, NULL);
  pageMenu->setPosition(Vec2::ZERO);
  stageLayer->addChild(pageMenu);

  auto buttonBack = MenuItemImage::create(vars["yaadv/ui/button_return.png"], vars["yaadv/ui/button_return_down.png"],
                                          CC_CALLBACK_0(SaveScene::back, this));
  buttonBack->setPosition(Vec2(175, 90));
  auto menu = Menu::create(buttonBack, NULL);
  menu->setPosition(Vec2::ZERO);
  stageLayer->addChild(menu);

  this->addChild(stageLayer);
  return true;
}

void SaveScene::back() {
  GameSystem::getInstance()->initGameSavedataList();
  Director::getInstance()->popScene();
}

void SaveScene::popup() {
  Config &vars = *Config::getInstance();
  auto result = true;
  if (result) {
    PopupLayer *popupDialog = PopupLayer::create(vars["yaadv/ui/popupwindow/bg_popup.png"]);
    popupDialog->addLabelButton(vars["Yes"], CC_CALLBACK_0(SaveScene::apply, this));
    popupDialog->addLabelButton(vars["No"], CC_CALLBACK_0(SaveScene::cancel, this));

    char *text = new char[13];
    sprintf(text, "%d", _currentSelectButton + 1);
    std::string popupText = "Are you sure save in No." + (std::string)text + " file?";
    popupDialog->setString(popupText);
    this->addChild(popupDialog);

    CC_SAFE_DELETE_ARRAY(text);
  }
}

void SaveScene::apply() {
  if (_currentSelectButton >= 0) {
    GameSystem::getInstance()->saveGameSceneInfo(_currentSelectButton);
    GameSystem::getInstance()->updateGameSavedata(_currentSelectButton);
    dataButtons[_currentSelectButton % 4]->updataData(true);
  }
}

void SaveScene::cancel() { _currentSelectButton = -1; }

void SaveScene::loadList(int page) {
  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();
  _savedataLayer->removeAllChildren();
  for (int i = 0; i < 4; i++) {
    dataButtons[i] = SaveData::create(4 * (page - 1) + i);
    dataButtons[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    dataButtons[i]->setPosition(visibleSize.width / 2 + origin.x + 50, 530 - 140 * i);
    if (eventTouch[i] != nullptr) {
      this->getEventDispatcher()->removeEventListener(eventTouch[i]);
    }
    eventTouch[i] = EventListenerTouchOneByOne::create();
    eventTouch[i]->onTouchBegan = [=](Touch *t, Event *e) {
      if (dataButtons[i]->getStageLayer()->getBoundingBox().containsPoint(dataButtons[i]->convertTouchToNodeSpace(t))) {
        return true;
      }
      return false;
    };
    eventTouch[i]->onTouchEnded = [=](Touch *t, Event *e) {
      if (dataButtons[i]->getStageLayer()->getBoundingBox().containsPoint(dataButtons[i]->convertTouchToNodeSpace(t))) {
        _currentSelectButton = 4 * (page - 1) + i;
        popup();
      } else {
      }
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventTouch[i], this);
    _savedataLayer->addChild(dataButtons[i]);
  }
}

void SaveScene::nextPage() {
  if (_currentPage < MAX_PAGE) {
    _currentPage++;
    loadList(_currentPage);
  }
}

void SaveScene::frontPage() {
  if (_currentPage > 1) {
    _currentPage--;
    loadList(_currentPage);
  }
}

}  // namespace yaadv
