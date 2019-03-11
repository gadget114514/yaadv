#include "MainMenuScene.h"
#include "GameScene.h"
#include "SettingScene.h"

#include "AudioHelper.h"
#include "GameSystem.h"
#include "LoadScene.h"
#include "SaveScene.h"

USING_NS_CC;

USING_NS_CC;
using namespace yaadv;
#define RAIN_MAX 130
#define RAIN_LEN_MAX 30
#define RAIN_LEN_MIN 1
#define RAIN_SPEED 2500
#define RAIN_OVER_RADIS 50
#define RAIN_RANGE 100

namespace yaadv {
MainMenu::MainMenu() { rain = false; }

MainMenu::~MainMenu() {
  if (rains.size() > 0) {
    for (Rain *rain : rains) {
      delete (rain);
      rain = nullptr;
    }
    rains.clear();
  }
  if (rainOvers.size() > 0) {
    for (RainOver *ro : rainOvers) {
      delete (ro);
      ro = nullptr;
    }
  }
  rainOvers.clear();
}

Scene *MainMenu::createScene() {
  auto scene = Scene::create();

  auto layer = MainMenu::create();

  scene->addChild(layer);

  return scene;
}

bool MainMenu::init() {
  if (!Layer::init()) {
    return false;
  }

  Config &vars = *Config::getInstance();
  GameSystem::getInstance();

  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  AudioHelper::getAudioEngine()->stopBackgroundMusic();
  AudioHelper::getAudioEngine()->stopAllEffects();

  AudioHelper::getAudioEngine()->playBackgroundMusic(vars["yaadv/bgm/main.mp3"].c_str(), true);
  if (AudioHelper::getAudioEngine()->isBackgroundMusicPlaying()) {
    ;
  }

  auto backgroundLayer = LayerColor::create(Color4B::WHITE);
  auto backgroundjpg = Sprite::create(vars["yaadv/ui/title/bg_title.png"]);
  backgroundjpg->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
  backgroundjpg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height + origin.y));
  backgroundLayer->addChild(backgroundjpg, 0);

  auto lastdayLogo = Sprite::create(vars["yaadv/ui/title/bg_title_logo.png"]);
  lastdayLogo->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
  lastdayLogo->setPosition(130, 370);

  drawNode = DrawNode::create();
  addChild(drawNode, 5);

  backgroundLayer->addChild(lastdayLogo);

  scheduleUpdate();

  if (rain) appendRain();

  auto buttonNew =
      MenuItemImage::create(vars["yaadv/ui/title/btn_title_new_normal.png"],
                            vars["yaadv/ui/title/btn_title_new_touch.png"], CC_CALLBACK_0(MainMenu::newgame, this));
  buttonNew->setPosition(Vec2(850, 400));

  auto buttonLoad =
      MenuItemImage::create(vars["yaadv/ui/title/btn_title_load_normal.png"],
                            vars["yaadv/ui/title/btn_title_load_touch.png"], CC_CALLBACK_0(MainMenu::load, this));
  buttonLoad->setPosition(Vec2(850, 300));

  auto buttonConfig =
      MenuItemImage::create(vars["yaadv/ui/title/btn_title_config_normal.png"],
                            vars["yaadv/ui/title/btn_title_config_touch.png"], CC_CALLBACK_0(MainMenu::config, this));
  buttonConfig->setPosition(Vec2(850, 200));

  auto menu = Menu::create(buttonNew, buttonLoad, buttonConfig, NULL);
  menu->setPosition(Vec2::ZERO);

  backgroundLayer->addChild(menu, 4);
  this->addChild(backgroundLayer, 0);

  return true;
}

void MainMenu::menuExit(Ref *pSender) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
  MessageBox(
      "You pressed the close button. Windows Store Apps do not implement a "
      "close button.",
      "Alert");
  return;
#endif

  Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  exit(0);
#endif
}

void MainMenu::newgame() {
  GameSystem::getInstance()->setGameScene(GameScene::createScene());
  auto scene = GameSystem::getInstance()->getGameScene();
  Director::getInstance()->replaceScene(scene);
}

void MainMenu::config() { Director::getInstance()->pushScene(SettingScene::createScene()); }

void MainMenu::load() { Director::getInstance()->pushScene(LoadScene::createScene()); }

void MainMenu::update(float dt) {
  drawNode->clear();

  if (rain) {
    appendRain();

    for (auto i = rainOvers.begin(); i != rainOvers.end();) {
      auto o = *i;
      drawNode->drawCircle(Vec2(o->x, o->y), o->r, 360, 100, false, 1.f, 0.4f, Color4F(1, 1, 1, o->alpha));
      o->r += (RAIN_LEN_MAX - o->y) / RAIN_LEN_MAX * RAIN_OVER_RADIS * dt;
      o->alpha -= dt;
      if (o->alpha <= 0) {
        i = rainOvers.erase(i);
        delete (o);
      } else {
        i++;
      }
    }

    for (auto i = rains.begin(); i != rains.end();) {
      auto r = *i;
      drawNode->drawLine(Vec2(r->x, r->y), Vec2(r->x, r->y + r->len), Color4F(1, 1, 1, r->alpha * 0.5f));
      float s = RAIN_SPEED * dt;
      if (r->y > (RAIN_LEN_MAX - r->len) / RAIN_LEN_MAX * RAIN_RANGE) {
        r->y -= (r->landLen / RAIN_LEN_MAX) * s;
        i++;
      } else if (!r->isLand) {
        i++;
        r->isLand = true;

        auto over = new RainOver();
        over->x = r->x;
        over->y = r->y;
        over->alpha = r->len / RAIN_LEN_MAX * 0.5f + 0.3f;
        over->r = 0;
        rainOvers.push_back(over);
      } else {
        r->len -= (r->landLen / RAIN_LEN_MAX) * s;
        if (r->len <= 0) {
          i = rains.erase(i);
          delete (r);
        } else {
          i++;
        }
      }
    }
  }
}

void MainMenu::appendRain() {
  while (rains.size() < RAIN_MAX) {
    auto r = new Rain();
    r->x = rand_0_1() * getContentSize().width;
    r->y = getContentSize().height + RAIN_LEN_MAX;
    r->len = rand_0_1() * (RAIN_LEN_MAX - RAIN_LEN_MIN) + RAIN_LEN_MIN;
    r->landLen = r->len;
    r->alpha = (r->len - RAIN_LEN_MIN) / (RAIN_LEN_MAX - RAIN_LEN_MIN);
    r->isLand = false;
    rains.push_back(r);
  }
}
}