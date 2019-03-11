#include "LoadScene.h"
#include "Control/PopupLayer.hpp"
#include "GameScene.h"
#include "GameSystem.h"

USING_NS_CC;
using namespace yaadv;
namespace yaadv {
LoadScene::LoadScene() {
  _currentSelectButton = -1;
  _currentPage = 1;
  for (int i = 0; i < 4; i++) {
    eventTouch[i] = nullptr;
  }
}

LoadScene::~LoadScene() {}

Scene *LoadScene::createScene() {
  auto scene = Scene::create();

  auto layer = LoadScene::create();

  scene->addChild(layer);

  return scene;
}

bool LoadScene::init() {
  if (!Layer::init()) {
    return false;
  }

  Config &vars = *Config::getInstance();
  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto stageLayer = Layer::create();

  auto backgroundLayer = LayerColor::create(Color4B::BLACK);
  stageLayer->addChild(backgroundLayer);

  auto backgroundWindow = Sprite::create(vars["yaadv/ui/sl/bg_load.jpg"]);
  backgroundWindow->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
  stageLayer->addChild(backgroundWindow);

  auto titletxt = Sprite::create(vars["yaadv/ui/sl/txt_load.png"]);
  titletxt->setPosition(Vec2(
	  origin.x + 170,
	  visibleSize.height + origin.y - 90));
  stageLayer->addChild(titletxt);

  _savedataLayer = Node::create();
  stageLayer->addChild(_savedataLayer);

  loadList(_currentPage);

  _buttonNext =
      MenuItemImage::create(vars["yaadv/ui/sl/btn_sl_next_normal.png"], vars["yaadv/ui/sl/btn_sl_next_touch.png"],
                            CC_CALLBACK_0(LoadScene::nextPage, this));
  _buttonNext->setPosition(Vec2(1000, 70));
  _buttonFront =
      MenuItemImage::create(vars["yaadv/ui/sl/btn_sl_front_normal.png"], vars["yaadv/ui/sl/btn_sl_front_touch.png"],
                            CC_CALLBACK_0(LoadScene::frontPage, this));
  _buttonFront->setPosition(Vec2(1000, 170));
  auto pageMenu = Menu::create(_buttonNext, _buttonFront, NULL);
  pageMenu->setPosition(Vec2::ZERO);
  stageLayer->addChild(pageMenu);

  auto buttonBack = MenuItemImage::create(vars["yaadv/ui/button_return.png"], vars["yaadv/ui/button_return_down.png"],
                                          CC_CALLBACK_0(LoadScene::back, this));
  buttonBack->setPosition(Vec2(175, 90));
  auto menu = Menu::create(buttonBack, NULL);
  menu->setPosition(Vec2::ZERO);
  stageLayer->addChild(menu);

  this->addChild(stageLayer);
  return true;
}

void LoadScene::back() { Director::getInstance()->popScene(); }

void LoadScene::popup() {
  Config &vars = *Config::getInstance();
  auto result = GameSystem::getInstance()->loadGameSceneInfo(_currentSelectButton);
  if (result) {
    PopupLayer *popupDialog = PopupLayer::create(vars["yaadv/ui/popupwindow/bg_popup.png"]);
    popupDialog->addLabelButton(vars["Yes"], CC_CALLBACK_0(LoadScene::apply, this));
    popupDialog->addLabelButton(vars["No"], CC_CALLBACK_0(LoadScene::cancel, this));
    popupDialog->setString(vars["Are you sure load this file?"]);
    this->addChild(popupDialog);
  }
}

void LoadScene::apply() {
  if (_currentSelectButton >= 0) {
    GameSystem::getInstance()->setIsLoadSuccess(true);
    if (GameSystem::getInstance()->getGameScene()) {
      Director::getInstance()->popScene();
    }
    GameSystem::getInstance()->setGameScene(GameScene::createScene());
    auto scene = GameSystem::getInstance()->getGameScene();
    Director::getInstance()->replaceScene(scene);
  }
}

void LoadScene::cancel() { _currentSelectButton = -1; }

void LoadScene::loadList(int page) {
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

void LoadScene::nextPage() {
  if (_currentPage < MAX_PAGE) {
    _currentPage++;
    loadList(_currentPage);
  }
}

void LoadScene::frontPage() {
  if (_currentPage > 1) {
    _currentPage--;
    loadList(_currentPage);
  }
}

}  // namespace yaadv
