#include "HistoryScene.h"
#include "ui/CocosGUI.h"
#include "yaadv.h"

USING_NS_CC;
using namespace yaadv;

namespace yaadv {
HistoryMessage::HistoryMessage(std::string text, std::string name, Color4B color) {
  float nameLabelHeight = 0;
  float textLabelHeight = 0;

  if (name.compare("")) {
    _nameLabel = Label::createWithSystemFont(name, "arial", 26);
    _nameLabel->setColor(Color3B::WHITE);
    _nameLabel->enableOutline(Color4B::BLACK, 10);
    _nameLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    _nameLabel->setPosition(Vec2(0, 0));
    this->addChild(_nameLabel);
    nameLabelHeight = _nameLabel->getContentSize().height;
  }

  _textLabel = Label::createWithSystemFont(text, "arial", 26);
  _textLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
  _textLabel->setPosition(Vec2(0, -nameLabelHeight));
  _textLabel->setDimensions(500, 0);
  this->addChild(_textLabel);
  textLabelHeight = _textLabel->getContentSize().height;

  this->_height = nameLabelHeight + textLabelHeight;
}

HistoryMessage::~HistoryMessage() {}

HistoryMessage *HistoryMessage::create(record *record) {
  auto text = record->text;
  auto name = record->name;
  auto color = record->color;
  auto tmpHM = new HistoryMessage(text, name, color);
  tmpHM->autorelease();
  return tmpHM;
}

float HistoryMessage::getHeight() { return this->_height; }

HistoryScene::HistoryScene() {}

HistoryScene::~HistoryScene() {}

Scene *HistoryScene::createScene() {
  auto scene = Scene::create();

  auto layer = HistoryScene::create();

  scene->addChild(layer);

  return scene;
}

bool HistoryScene::init() {
  if (!Layer::init()) {
    return false;
  }
  Config &vars = *Config::getInstance();

  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto stageLayer = Layer::create();

  auto bg = Sprite::create(vars["yaadv/ui/log/history_bg.png"]);
  bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
  stageLayer->addChild(bg);

  auto backgroundImage = Sprite::create(vars["yaadv/ui/log/history_dialog_box.png"]);
  backgroundImage->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
  stageLayer->addChild(backgroundImage);

  auto historyList = Sprite::create();

  historyList->setPosition(Vec2(0, 0));

  auto historyListView = ui::ScrollView::create();

  historyListView->setDirection(ui::ScrollView::Direction::VERTICAL);

  historyListView->setTouchEnabled(true);

  historyListView->setBounceEnabled(true);

  historyListView->setContentSize(Size(visibleSize.width / 2, visibleSize.height / 2));
  historyListView->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
  historyListView->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
  int startY = 0;
  for (int i = 0; i < HistoryLogger::getInstance()->getLength(); i++) {
    auto record = HistoryLogger::getInstance()->getRecord(i);
    auto hm = HistoryMessage::create(record);
    hm->setPosition(0, startY);
    historyList->addChild(hm);
    startY -= hm->getHeight() + 13;
  }

  historyListView->setInnerContainerSize(Size(visibleSize.width / 2, abs(startY)));

  historyList->setPosition(Vec2(0, historyListView->getInnerContainerSize().height));
  historyListView->addChild(historyList);

  stageLayer->addChild(historyListView);

  historyListView->jumpToBottom();

  scheduleOnce([=](float) { historyListView->jumpToBottom(); }, 0.01f, "XXX");

  auto buttonBack = MenuItemImage::create(vars["yaadv/ui/button_return.png"], vars["yaadv/ui/button_return_down.png"],
                                          CC_CALLBACK_0(HistoryScene::back, this));
  buttonBack->setPosition(Vec2(175 + origin.x, 90 + origin.y));
  auto menu = Menu::create(buttonBack, NULL);
  menu->setPosition(Vec2::ZERO);
  stageLayer->addChild(menu);

  addChild(stageLayer);

  return true;
}

void HistoryScene::back() { Director::getInstance()->popScene(); }

}  // namespace yaadv
