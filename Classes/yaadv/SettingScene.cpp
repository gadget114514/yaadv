#include "SettingScene.h"
#include "GameSystem.h"

#include "AudioHelper.h"
#include "Control/RadioButton.h"
#include "Control/Slidebar.h"
#include "ScriptReader/yaadvConfig.h"

using namespace yaadv;

namespace yaadv {

SettingScene::SettingScene() {}

SettingScene::~SettingScene() {}

Scene *SettingScene::createScene() {
  auto scene = Scene::create();

  auto layer = SettingScene::create();

  scene->addChild(layer);

  return scene;
}

bool SettingScene::init() {
  if (!Layer::init()) {
    return false;
  }
  Config &vars = *Config::getInstance();

  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto stageLayer = Layer::create();

  auto backgroundLayer = LayerColor::create(Color4B::BLACK);
  stageLayer->addChild(backgroundLayer);

  auto backgroundBox = Sprite::create(vars["yaadv/ui/config/bg_config.jpg"]);
  backgroundBox->setPosition(Vec2(visibleSize.width / 2 + origin.x,
                                  visibleSize.height / 2 + origin.y));
  stageLayer->addChild(backgroundBox);

  auto titletxt = Sprite::create(vars["yaadv/ui/config/txt_config.png"]);
  titletxt->setPosition(Vec2(
	  origin.x + 170,
	  visibleSize.height + origin.y - 90));
  stageLayer->addChild(titletxt);

  auto textSpeedTxtPh = Sprite::create(vars["yaadv/ui/config/txt_textspeed.jpg"]);
  textSpeedTxtPh->setPosition(Vec2(430 + origin.x, 510 + origin.y));
  stageLayer->addChild(textSpeedTxtPh);
  auto autoSpeedTxtPh =
      Sprite::create(vars["yaadv/ui/config/txt_autospeed.jpg"]);
  autoSpeedTxtPh->setPosition(Vec2(430 + origin.x, 442 + origin.y));
  stageLayer->addChild(autoSpeedTxtPh);
  auto bgmvolTxtPh =
      Sprite::create(vars["yaadv/ui/config/txt_bgmvol.jpg"]);
  bgmvolTxtPh->setPosition(Vec2(430 + origin.x, 374 + origin.y));
  stageLayer->addChild(bgmvolTxtPh);
  auto sevolTxtPh =
      Sprite::create(vars["yaadv/ui/config/txt_sevol.jpg"]);
  sevolTxtPh->setPosition(Vec2(430 + origin.x, 306 + origin.y));
  stageLayer->addChild(sevolTxtPh);
  auto skipreadonlyTxtPh =
      Sprite::create(vars["yaadv/ui/config/txt_skipreadonly.jpg"]);
  skipreadonlyTxtPh->setPosition(Vec2(640 + origin.x, 212 + origin.y));
  stageLayer->addChild(skipreadonlyTxtPh);
  auto skipallTxtPh =
      Sprite::create(vars["yaadv/ui/config/txt_skipall.jpg"]);
  skipallTxtPh->setPosition(Vec2(840 + origin.x, 212 + origin.y));
  stageLayer->addChild(skipallTxtPh);


  _textSpeedSlb = rx::Slidebar::createSlidebar(vars["yaadv/ui/config/slb_background.png"],
                                           vars["yaadv/ui/config/slb_ball.png"]);
  _textSpeedSlb->setPosition(Vec2(825 + origin.x, 510 + origin.y));
  _textSpeedSlb->setFloat(GameSystem::getInstance()->getTextSpeed());
  _textSpeedSlb->moveEvent = CC_CALLBACK_0(SettingScene::changeTextSpeed, this);
  _textSpeedSlb->touchEvent =
      CC_CALLBACK_0(SettingScene::changeTextSpeed, this);
  stageLayer->addChild(_textSpeedSlb);

  _autoSpeedSlidebar = rx::Slidebar::createSlidebar(
      vars["yaadv/ui/config/slb_background.png"],
                                                vars["yaadv/ui/config/slb_ball.png"]);
  _autoSpeedSlidebar->setPosition(Vec2(825 + origin.x, 442 + origin.y));
  _autoSpeedSlidebar->setFloat(GameSystem::getInstance()->getAutoSpeed() / 5);
  _autoSpeedSlidebar->moveEvent =
      CC_CALLBACK_0(SettingScene::changeAutoSpeed, this);
  _autoSpeedSlidebar->touchEvent =
      CC_CALLBACK_0(SettingScene::changeAutoSpeed, this);
  stageLayer->addChild(_autoSpeedSlidebar);

  _musicVolumeSlidebar = rx::Slidebar::createSlidebar(
      vars["yaadv/ui/config/slb_background.png"], 
	  vars["yaadv/ui/config/slb_ball.png"]);
  _musicVolumeSlidebar->setPosition(Vec2(825 + origin.x, 374 + origin.y));
  _musicVolumeSlidebar->setFloat(GameSystem::getInstance()->getMusicVolume());
  _musicVolumeSlidebar->moveEvent =
      CC_CALLBACK_0(SettingScene::changeMusicVolume, this);
  _musicVolumeSlidebar->touchEvent =
      CC_CALLBACK_0(SettingScene::changeMusicVolume, this);
  stageLayer->addChild(_musicVolumeSlidebar);

  _soundVolumeSlidebar = rx::Slidebar::createSlidebar(
      vars["yaadv/ui/config/slb_background.png"], 
	  vars["yaadv/ui/config/slb_ball.png"]);
  _soundVolumeSlidebar->setPosition(Vec2(825 + origin.x, 306 + origin.y));
  _soundVolumeSlidebar->setFloat(GameSystem::getInstance()->getSoundVolume());
  _soundVolumeSlidebar->moveEvent =
      CC_CALLBACK_0(SettingScene::changeSoundVolume, this);
  _soundVolumeSlidebar->touchEvent =
      CC_CALLBACK_0(SettingScene::changeSoundVolume, this);
  stageLayer->addChild(_soundVolumeSlidebar);

  auto ebSkipReadOnly = EasyButton::createEasyButton(
      vars["yaadv/ui/charbox_off.png"], 
	  vars["yaadv/ui/charbox_off.png"], 
	  vars["yaadv/ui/config/yes.png"]);
  ebSkipReadOnly->setPosition(Vec2(740, 212));

  auto ebSkipAll = EasyButton::createEasyButton(
      vars["yaadv/ui/charbox_off.png"], 
	  vars["yaadv/ui/charbox_off.png"], 
	  vars["yaadv/ui/config/yes.png"]);
  ebSkipAll->setPosition(Vec2(940, 212));

  _isSkipAll =
      rx::RadioButton::createRadioButton(ebSkipReadOnly, ebSkipAll, nullptr);
  if (GameSystem::getInstance()->getIsSkipAll()) {
    _isSkipAll->setSelectedNumber(1);
  } else {
    _isSkipAll->setSelectedNumber(0);
  }
  _isSkipAll->touchEvent = CC_CALLBACK_0(SettingScene::changeIsSkipAll, this);
  stageLayer->addChild(_isSkipAll);

  auto buttonBack = MenuItemImage::create(
      vars["yaadv/ui/config/btn_back_normal.png"], 
	  vars["yaadv/ui/config/btn_back_touch.png"],
      CC_CALLBACK_0(SettingScene::back, this));
  buttonBack->setPosition(Vec2(175, 90));
  auto menu = Menu::create(buttonBack, NULL);
  menu->setPosition(Vec2::ZERO);
  stageLayer->addChild(menu);

  this->addChild(stageLayer);

  return true;
}

void SettingScene::back() { Director::getInstance()->popScene(); }

void SettingScene::changeMusicVolume() {
  log("Music Value : %f", _musicVolumeSlidebar->getFloat());
  GameSystem::getInstance()->setMusicVolume(_musicVolumeSlidebar->getFloat());
  AudioHelper::getAudioEngine()->setBackgroundMusicVolume(
      GameSystem::getInstance()->getMusicVolume());
}

void SettingScene::changeSoundVolume() {
  GameSystem::getInstance()->setSoundVolume(_soundVolumeSlidebar->getFloat());
  AudioHelper::getAudioEngine()->setEffectsVolume(
      GameSystem::getInstance()->getSoundVolume());
}

void SettingScene::changeTextSpeed() {
  GameSystem::getInstance()->setTextSpeed(_textSpeedSlb->getFloat());
}

void SettingScene::changeAutoSpeed() {
  GameSystem::getInstance()->setAutoSpeed(5.01f -
                                          _autoSpeedSlidebar->getFloat() * 5);
}

void SettingScene::changeIsSkipAll() {
  log("Skip changed!");
  log("selected number = %d", _isSkipAll->getSelectedNumber());
  if (_isSkipAll->getSelectedNumber()) {
    GameSystem::getInstance()->setIsSkipAll(true);
  } else {
    GameSystem::getInstance()->setIsSkipAll(false);
  }
}

}  // namespace yaadv