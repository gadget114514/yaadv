#include "GameScene.h"

#include "AudioHelper.h"
#include "Control/Between.h"
#include "Control/PopupLayer.hpp"
#include "GameSystem.h"
#include "History.h"
#include "HistoryScene.h"
#include "LoadScene.h"
#include "MainMenuScene.h"
#include "SaveScene.h"
#include "ScriptReader/BackgroundManager.h"
#include "ScriptReader/BackgroundMusicManager.h"
#include "ScriptReader/SoundManager.h"
#include "SettingScene.h"

#define SCRIPT_FILE "scenario/SetTest.txt"

USING_NS_CC;

using namespace yaadv;
namespace yaadv {

GameScene::GameScene()
    : _label(nullptr),
      _backgroundSprite(nullptr),
      _backgroundKey(""),
      _backgroundMusicKey(""),
      _soundKey(""),
      _currentName(""),
      _currentText(""),
      _optionsNumber(0),
      _isAutoPlaying(false),
      _isSkipPlaying(false),
      _immediate(false) {
  _characters = new std::vector<Character *>();
  _yaimgs = new std::vector<YaImg *>();
}

GameScene::~GameScene() {
  HistoryLogger::destoryInstance();

  _characters->clear();
  CC_SAFE_DELETE(_characters);
  _yaimgs->clear();
  CC_SAFE_DELETE(_yaimgs);

  _isAutoPlaying = false;
  _isSkipPlaying = false;
}

Scene *GameScene::createScene() {
  auto scene = Scene::create();

  auto layer = GameScene::create();

  scene->addChild(layer);

  return scene;
}

bool GameScene::init() {
  if (!Layer::init()) {
    return false;
  }

  Config &vars = *Config::getInstance();

  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto stageLayer = Layer::create();
  addChild(stageLayer, 13);

  _backgroundLayer = Layer::create();

  this->addChild(_backgroundLayer, -1);

  _charactersLayer = Layer::create();
  this->addChild(_charactersLayer, 1);

  _dialogWindow = Sprite::create(vars["yaadv/ui/dialog/dialog_bg.png"]);
  _dialogWindow->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
  _dialogWindow->setPosition(Vec2(visibleSize.width / 2, 150));
  this->addChild(_dialogWindow, 10);

  _textLabel = CharLabel::create(
      "", 30, CC_CALLBACK_0(GameScene::showWaitingPrompt, this));
  _textLabel->setPosition(100, _dialogWindow->getContentSize().height - 10);
  _textLabel->setColor(Color3B::WHITE);

  _textLabel->setContentSize(
      Size(_dialogWindow->getContentSize().width - 100 * 2,
           _dialogWindow->getContentSize().height - 10 * 2));
  _dialogWindow->addChild(_textLabel, 12);

  _wtIcon = Sprite::create(vars["yaadv/ui/dialog/wait_icon.png"]);
  _wtIcon->setPosition(_dialogWindow->getContentSize().width - 50, 25);
  _wtIcon->setOpacity(0);
  _dialogWindow->addChild(_wtIcon);

  _nameWindow = Sprite::create(vars["yaadv/ui/dialog/dialog_name.png"]);
  _nameWindow->setPosition(170, 190);
  _nameWindow->setOpacity(0);
  _dialogWindow->addChild(_nameWindow, 10);

  _nameLabel = Label::createWithSystemFont("", vars["arial"], 30);
  _nameLabel->setColor(Color3B::WHITE);
  _nameLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
  _nameLabel->setPosition(Vec2(_nameWindow->getContentSize().width / 2,
                               _nameWindow->getContentSize().height / 2));
  _nameWindow->addChild(_nameLabel, 11);

  _nameLabel->enableShadow(Color4B(0, 0, 0, 200), Size(2, -2), 1);

  _btnMenu =
      MenuItemImage::create(vars["yaadv/ui/dialog/btn_menu_normal.png"],
                            vars["yaadv/ui/dialog/btn_menu_touch.png"],
                            CC_CALLBACK_0(GameScene::showMenuLayer, this));
  _btnMenu->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
  _btnMenu->setPosition(visibleSize.width + origin.x - 50,
                        visibleSize.height + origin.y - 50);
  auto menu = Menu::create(_btnMenu, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 13);

  _autoIcon = Sprite::create(vars["yaadv/ui/dialog/dialog_auto.png"]);
  _autoIcon->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
  _autoIcon->setPosition(Vec2(_dialogWindow->getContentSize().width,
                              _dialogWindow->getContentSize().height + 10));
  _autoIcon->setOpacity(0);
  _dialogWindow->addChild(_autoIcon);

  _skipIcon = Sprite::create(vars["yaadv/ui/dialog/dialog_skip.png"]);
  _skipIcon->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
  _skipIcon->setPosition(Vec2(_dialogWindow->getContentSize().width,
                              _dialogWindow->getContentSize().height + 10));
  _skipIcon->setOpacity(0);
  _dialogWindow->addChild(_skipIcon);

  _selectLayer = Layer::create();
  this->addChild(_selectLayer, 13);

  auto clickEvent = EventListenerTouchOneByOne::create();
  clickEvent->onTouchBegan = [=](Touch *t, Event *e) {
    this->screenClicked();
    return false;
  };
  stageLayer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
      clickEvent, stageLayer);

  auto dialogClickEvent = EventListenerTouchOneByOne::create();
  dialogClickEvent->onTouchBegan = [=](Touch *t, Event *e) {
    if (_dialogWindow->getBoundingBox().containsPoint(
            _dialogWindow->convertTouchToNodeSpace(t))) {
      this->dialogClicked();
    }
    return true;
  };

  ScriptReader::getInstance()->initWithStage(stageLayer);

  ScriptReader::getInstance()->showImage =
      CC_CALLBACK_3(GameScene::showImage, this);
  ScriptReader::getInstance()->imageTween =
      CC_CALLBACK_5(GameScene::imageTween, this);
  ScriptReader::getInstance()->hideImage =
      CC_CALLBACK_1(GameScene::hideImage, this);

  ScriptReader::getInstance()->showText =
      CC_CALLBACK_1(GameScene::showText, this);
  ScriptReader::getInstance()->showName =
      CC_CALLBACK_1(GameScene::showName, this);
  ScriptReader::getInstance()->changeBackground =
      CC_CALLBACK_1(GameScene::changeBackground, this);
  ScriptReader::getInstance()->playBackgroundMusic =
      CC_CALLBACK_1(GameScene::playBackgroundMusic, this);
  ScriptReader::getInstance()->stopBackgroundMusic =
      CC_CALLBACK_0(GameScene::stopBackgroundMusic, this);
  ScriptReader::getInstance()->playSound =
      CC_CALLBACK_1(GameScene::playSound, this);
  ScriptReader::getInstance()->stopSound =
      CC_CALLBACK_0(GameScene::stopSound, this);
  ScriptReader::getInstance()->showCharacter =
      CC_CALLBACK_2(GameScene::displayCharacter, this);
  ScriptReader::getInstance()->hideCharacter =
      CC_CALLBACK_1(GameScene::hideCharacter, this);
  ScriptReader::getInstance()->showSelect =
      CC_CALLBACK_1(GameScene::showSelect, this);
  ScriptReader::getInstance()->returnToMenu =
      CC_CALLBACK_0(GameScene::showMenuSceneYes, this);
  ScriptReader::getInstance()->clear = CC_CALLBACK_1(GameScene::clear, this);

  // ScriptReader::getInstance()->loadScriptFile(SCRIPT_FILE);
  // ScriptReader::getInstance()->loadScriptFile("scenario/test.yasl");
  ScriptReader::getInstance()->loadScriptFile(vars["yaadv/scenario/test.wren"]);

  if (!GameSystem::getInstance()->getIsLoadSuccess()) {
    ScriptReader::getInstance()->nextScript();
  } else {
    clear();
    reloadScene();
    GameSystem::getInstance()->setIsLoadSuccess(false);
  }

  return true;
}

void GameScene::update(float dt) {}

void GameScene::updateLabel(std::string text) { _label->setString(text); }

void GameScene::showClickSign() {}

void GameScene::screenClicked() {
  if (_isAutoPlaying) {
    stopAutoPlay();
  }
  if (_isSkipPlaying) {
    stopSkipPlay();
  }

  dialogClicked();
}

void GameScene::dialogClicked() {
  skipAction();
  if (_textLabel->isRunning()) {
    _textLabel->finishShow();
    return;
  }
  hideWaitingPrompt();
  ScriptReader::getInstance()->nextScript();
}

void GameScene::showName(std::string &name) {
  _currentName = name;
  _nameLabel->setString(name);
  if (name.compare("") == 0) {
    _nameWindow->setOpacity(0);
  } else {
    _nameWindow->setOpacity(255);
  }
}

#define YIM YaImgManager::getInstance()

void GameScene::showImage(std::string &cName, std::string &loc,
                          std::string &face) {
  auto cha = YIM->getYaImg(cName);
  if (!cha) {
    log("GameScene::displayCharacter: Not found character:%s", cName.c_str());
    return;
  }
  if (std::find(_yaimgs->begin(), _yaimgs->end(), cha) == _yaimgs->end()) {
    _yaimgs->push_back(cha);
  }
  if (cha->getFace(face)) {
    log("showImage cName = %s, Name = %s", cName.c_str(), cha->name.c_str());
    bool isNeedShow = false;
    if (cha->_sprite) {
      if (cha->currentFace != face && face.compare("") != 0) {
        cha->leave();
        isNeedShow = true;
      }
    } else {
      isNeedShow = true;
    }

    if (isNeedShow) {
      Sprite *sp = nullptr;
      auto pic = cha->getFace(face);
      if (pic) {
        sp = Sprite::create(pic);
        sp->setAnchorPoint(Vec2(0.5, 0));
        _charactersLayer->addChild(sp);
        cha->currentPosition = CMPositionType::EMPTY;
      }
      cha->_sprite = sp;
      cha->key = cName;
      cha->currentFace = face;
      _immediate = true;
      updateDisplayYaImg();
      _immediate = false;
    }
  } else {
    log("GameScene::showImage: Not found face:%s", face.c_str());
  }
}

void GameScene::imageTween(std::string &cName, std::string &loc, float duration,
                           std::string &ease, std::string &face) {
  auto cha = YIM->getYaImg(cName);
  if (!cha) {
    log("GameScene::displayCharacter: Not found character:%s", cName.c_str());
    return;
  }
  if (std::find(_yaimgs->begin(), _yaimgs->end(), cha) == _yaimgs->end()) {
    _yaimgs->push_back(cha);
  }
  if (cha->getFace(face)) {
    log("showImage cName = %s, Name = %s", cName.c_str(), cha->name.c_str());
    bool isNeedShow = false;
    if (cha->_sprite) {
      if (cha->currentFace != face && face.compare("") != 0) {
        cha->leave();
        isNeedShow = true;
      }
    } else {
      isNeedShow = true;
    }

    if (isNeedShow) {
      Sprite *sp = nullptr;
      auto pic = cha->getFace(face);
      if (pic) {
        sp = Sprite::create(pic);
        sp->setAnchorPoint(Vec2(0.5, 0));
        _charactersLayer->addChild(sp);
        cha->currentPosition = CMPositionType::EMPTY;
      }
      cha->_sprite = sp;
      cha->key = cName;
      cha->currentFace = face;
      //    _immediate = true;
      //    updateDisplayYaImg();
      //    _immediate = false;
      Between::to(cha->_sprite, loc, duration, Between::easing(ease));
    }
  } else {
    log("GameScene::showImage: Not found face:%s", face.c_str());
  }
}

void GameScene::showText(std::string &text) {
  _currentText = text;
  _textLabel->setString(text);
}

void GameScene::changeBackground(std::string &key) {
  auto background = BM->getBackground(key);
  if (background.compare("") == 0) return;
  _backgroundKey = key;
  auto backgroundSprite = Sprite::create(background);
  backgroundSprite->setAnchorPoint(Vec2(0, 0));
  backgroundSprite->setOpacity(0);
  _backgroundLayer->addChild(backgroundSprite);

  auto mtAction = Sequence::createWithTwoActions(
      FadeIn::create(0.8f), CallFunc::create([&]() {
        if (_backgroundSprite) {
          _backgroundSprite = backgroundSprite;

        } else {
          _backgroundSprite = backgroundSprite;
        }
      }));
  mtAction->setTag(1);
  backgroundSprite->runAction(mtAction);
}

void GameScene::playBackgroundMusic(std::string &key) {
  stopBackgroundMusic();
  auto bgm = BMM->getBackgroundMusic(key);
  if (bgm.compare("") == 0) {
    log("Undefine bgm %s", key.c_str());
    return;
  }

  AudioHelper::getAudioEngine()->playBackgroundMusic(bgm.c_str(), true);
  if (AudioHelper::getAudioEngine()->isBackgroundMusicPlaying()) {
    log("Playing BGM %s", bgm.c_str());
  }
  _isPlayingMusic = true;
  _backgroundMusicKey = key;
}

void GameScene::stopBackgroundMusic() {
  if (_isPlayingMusic) {
    AudioHelper::getAudioEngine()->stopBackgroundMusic();
    _isPlayingMusic = false;
    _backgroundMusicKey = "";
  }
}

void GameScene::scheduleVolumeFadeIn() {}

void GameScene::scheduleVolumeFadeOut() {}

void GameScene::playSound(std::string &file) {
  stopSound();
  AudioHelper::getAudioEngine()->playEffect(file.c_str());
}

void GameScene::stopSound() { AudioHelper::getAudioEngine()->stopAllEffects(); }

void GameScene::startAutoPlay() {
  _isAutoPlaying = true;
  _autoIcon->setOpacity(255);
  
  _menuLayer->removeAllChildren();
  _btnMenu->setOpacity(255);


  if (!_textLabel->isRunning()) {
    this->runAction(Sequence::create(
        DelayTime::create(GameSystem::getInstance()->getAutoSpeed()),
        CallFunc::create(CC_CALLBACK_0(GameScene::dialogClicked, this)), NULL));
  }
}

void GameScene::stopAutoPlay() {
  _isAutoPlaying = false;
  _autoIcon->setOpacity(0);
  unschedule(schedule_selector(GameScene::autoPlay));
}

void GameScene::startSkipPlay() {
  _isSkipPlaying = true;
  _skipIcon->setOpacity(255);
  if (true) {
    _menuLayer->removeAllChildren();
    _btnMenu->setOpacity(255);
  }
  if (true) {
    log("start skip");
    if (ScriptReader::getInstance()->getIsHaveRead()) {
      log("IsHaveRead");
      schedule(schedule_selector(GameScene::skipPlay), 0.01f);
    }

  } else {
    schedule(schedule_selector(GameScene::autoPlay), 0.01f);
  }
}

void GameScene::stopSkipPlay() {
  _isSkipPlaying = false;
  _skipIcon->setOpacity(0);
  unschedule(schedule_selector(GameScene::skipPlay));
  unschedule(schedule_selector(GameScene::autoPlay));
}

void GameScene::autoPlay(float dt) { dialogClicked(); }

void GameScene::skipPlay(float dt) {

  if (!ScriptReader::getInstance()->getIsHaveRead()) {
    stopSkipPlay();
  }
  dialogClicked();
}

void GameScene::createGameDate() {
  auto tmpGameData = new GameData();
  tmpGameData->backgroundKey = _backgroundKey;
  tmpGameData->bgmKey = _backgroundMusicKey;
  tmpGameData->soundKey = _soundKey;
  tmpGameData->characterCount = _characters->size();

  for (int n = 0; n < 3; n++) {
    if (n < (int)_characters->size()) {
      auto c = _characters->at(n);
      tmpGameData->fgCharacters[n].name = c->key;
      tmpGameData->fgCharacters[n].face = c->currentFace;
    } else {
      tmpGameData->fgCharacters[n].name = "";
      tmpGameData->fgCharacters[n].face = "";
    }
    tmpGameData->fgCharacters[n].number = n;
  }
  tmpGameData->currentCommandIndex =
      ScriptReader::getInstance()->getCurrentCommandIndex();
  tmpGameData->currentSignName =
      ScriptReader::getInstance()->getCurrentSignName();
  tmpGameData->currentName = _currentName;
  tmpGameData->currentText = _currentText;
  tmpGameData->optionsNumber = _optionsNumber;

  if (_optionsNumber) {
    tmpGameData->options.insert(_currentOptions.begin(), _currentOptions.end());
  }

  GameSystem::getInstance()->setGameSceneInfo(tmpGameData);
}

void GameScene::displayCharacter(std::string cName, std::string face) {
  auto cha = CM->getCharacter(cName);
  if (!cha) {
    return;
  }
  if (std::find(_characters->begin(), _characters->end(), cha) ==
      _characters->end()) {
    _characters->push_back(cha);
  }
  if (cha->getCharacterFace(face)) {
    bool isNeedShow = false;
    if (cha->_sprite) {
      if (cha->currentFace != face && face.compare("") != 0) {
        cha->leave();
        isNeedShow = true;
      }
    } else {
      isNeedShow = true;
    }

    if (isNeedShow) {
      Sprite *sp = nullptr;
      auto pic = cha->getCharacterFace(face);
      if (pic) {
        auto bamiri = cha->getBamiri(face);
        float anchorx = 0.5;
        float anchory = 0.0;
        if (bamiri) anchorx = bamiri->anchorx, anchory = bamiri->anchory;

        sp = Sprite::create(pic);
        sp->setAnchorPoint(Vec2(anchorx, anchory));
        if (bamiri) {
          sp->setScaleX(bamiri->scalex);
          sp->setScaleY(bamiri->scaley);
        }
        _charactersLayer->addChild(sp);
        cha->currentPosition = CMPositionType::EMPTY;
      }
      cha->_sprite = sp;
      cha->key = cName;
      cha->currentFace = face;
      updateDisplayCharacter();
    }
  } else {

  }
}

void GameScene::updateDisplayYaImg() {
  for (auto i = _yaimgs->begin(); i != _yaimgs->end(); i++) {
    auto c = *i;
    if (c->_sprite) {
      c->moveTo(CMPositionType::CENTER, _immediate);
    }
  }
}

void GameScene::hideYaImg(std::string cName) {
  auto cha = YIM->getYaImg(cName);
  if (!cha) {
    return;
  }
  if (cha->_sprite) {
    auto mtAction = Sequence::createWithTwoActions(
        FadeOut::create(0.8f), CallFunc::create([=]() {
          auto i = std::find(_yaimgs->begin(), _yaimgs->end(), cha);
          if (i != _yaimgs->end()) {
            _yaimgs->erase(i);
          }
          updateDisplayYaImg();
          cha->leave();
          return;
        }));
    mtAction->setTag(1);
    cha->_sprite->runAction(mtAction);
  }

  return;
}

void GameScene::updateDisplayCharacter() {
  int showCount = 0;
  for (auto i = _characters->begin(); i != _characters->end(); i++) {
    auto c = *i;
    if (c->_sprite) {
      showCount++;
    }
  }
  int index = 0;
  for (auto i = _characters->begin(); i != _characters->end(); i++) {
    auto c = *i;
    if (c->_sprite) {
      bool immediately = false;
      if (c->currentPosition == CMPositionType::EMPTY) {
        immediately = true;
      }
      CMPositionType pt = CMPositionType::CENTER;

      if (showCount == 1) {
      } else if (showCount == 2) {
        pt = index == 0 ? CMPositionType::LEFT_CENTER
                        : CMPositionType::RIGHT_CENTER;
      } else if (showCount == 3) {
        if (index == 0) {
          pt = CMPositionType::LEFT;
        }
        if (index == 1) {
          pt = CMPositionType::CENTER;
        }
        if (index == 2) {
          pt = CMPositionType::RIGHT;
        }
      } else if (showCount == 4) {
        if (index == 0) {
          pt = CMPositionType::LEFT;
        }
        if (index == 1) {
          pt = CMPositionType::LEFT_CENTER;
        }
        if (index == 2) {
          pt = CMPositionType::RIGHT_CENTER;
        }
        if (index == 3) {
          pt = CMPositionType::RIGHT;
        }
      } else if (showCount == 5) {
        if (index == 0) {
          pt = CMPositionType::LEFT;
        }
        if (index == 1) {
          pt = CMPositionType::LEFT_CENTER;
        }
        if (index == 2) {
          pt = CMPositionType::CENTER;
        }
        if (index == 3) {
          pt = CMPositionType::RIGHT_CENTER;
        }
        if (index == 4) {
          pt = CMPositionType::RIGHT;
        }
      }

      c->moveTo(pt, immediately);
      index++;
    }
  }
}

void GameScene::hideCharacter(std::string cName) {
  auto cha = CM->getCharacter(cName);
  if (!cha) {
    return;
  }
  if (cha->_sprite) {
    auto mtAction = Sequence::createWithTwoActions(
        FadeOut::create(0.8f), CallFunc::create([=]() {
          auto i = std::find(_characters->begin(), _characters->end(), cha);
          if (i != _characters->end()) {
            _characters->erase(i);
          }
          updateDisplayCharacter();
          cha->leave();
          return;
        }));
    mtAction->setTag(1);
    cha->_sprite->runAction(mtAction);
  }

  return;
}

void GameScene::hideImage(std::string cName) {
  auto cha = YIM->getYaImg(cName);
  if (!cha) {
    return;
  }
  if (cha->_sprite) {
    auto mtAction = Sequence::createWithTwoActions(
        FadeOut::create(0.8f), CallFunc::create([=]() {
          auto i = std::find(_yaimgs->begin(), _yaimgs->end(), cha);
          if (i != _yaimgs->end()) {
            _yaimgs->erase(i);
          }
          updateDisplayYaImg();
          cha->leave();
          return;
        }));
    mtAction->setTag(1);
    cha->_sprite->runAction(mtAction);
  }

  return;
}

void GameScene::showSaveScene() {
  createGameDate();
  Director::getInstance()->pushScene(SaveScene::createScene());
}

void GameScene::showConfigScene() {
  Director::getInstance()->pushScene(SettingScene::createScene());
}

void GameScene::ScreenShoot() {
  float screenShootWidth = SCREEN_SHOOT_WIDTH;
  float screenShootHeight = SCREEN_SHOOT_HEIGHT;

  float scale = SCREEN_SHOOT_HEIGHT / getContentSize().height;
  this->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
  auto render = RenderTexture::create(getContentSize().width * scale,
                                      getContentSize().height * scale);

  render->begin();
  this->setScale(scale);
  this->visit();
  render->end();
  Director::getInstance()->getRenderer()->render();
  render->retain();
  GameSystem::getInstance()->setScreenShoot(render);
  this->setScale(1.0f);
}

void GameScene::clear(int sw) {
  Config &vars = *Config::getInstance();

  for (auto i = _characters->begin(); i != _characters->end(); i++) {
    auto c = *i;
    c->leave();
  }
  _characters->clear();

  for (auto i = _yaimgs->begin(); i != _yaimgs->end(); i++) {
    auto c = *i;
    c->leave();
  }
  _yaimgs->clear();

  if (sw) stopBackgroundMusic();

  auto backgroundSprite = Sprite::create(vars["yaadv/bgimage/black.jpg"]);
  backgroundSprite->setAnchorPoint(Vec2(0, 0));
  _backgroundLayer->addChild(backgroundSprite);
  _backgroundSprite = backgroundSprite;

  if (sw) stopSound();

  std::string empty = "";
  showName(empty);
  showText(empty);

  if (_optionsNumber) {
    _optionsNumber = 0;
    _currentOptions.clear();
    _selectLayer->removeAllChildren();
  }
}

void GameScene::showLoadScene() {
  Director::getInstance()->pushScene(LoadScene::createScene());
}

void GameScene::reloadScene() {
  if (GameSystem::getInstance()->getIsLoadSuccess()) {
    auto background = BM->getBackground(
        GameSystem::getInstance()->getGameSceneInfo()->backgroundKey);
    if (background.compare("") == 0) {
      background = "black";
    }
    auto backgroundSprite = Sprite::create(background);
    backgroundSprite->setAnchorPoint(Vec2(0, 0));
    _backgroundLayer->addChild(backgroundSprite);
    _backgroundSprite = backgroundSprite;

    showName(GameSystem::getInstance()->getGameSceneInfo()->currentName);

    showText(GameSystem::getInstance()->getGameSceneInfo()->currentText);

    int charNumber =
        GameSystem::getInstance()->getGameSceneInfo()->characterCount;

    for (int i = 0; i < charNumber; i++) {
      auto name =
          GameSystem::getInstance()->getGameSceneInfo()->fgCharacters[i].name;
      auto face =
          GameSystem::getInstance()->getGameSceneInfo()->fgCharacters[i].face;
      auto number =
          GameSystem::getInstance()->getGameSceneInfo()->fgCharacters[i].number;
      auto cha = CM->getCharacter(name);
      if (cha) {
        if (std::find(_characters->begin(), _characters->end(), cha) ==
            _characters->end()) {
          _characters->push_back(cha);
        }

        Sprite *sp = nullptr;
        if (cha->getCharacterFace(face)) {
          sp = Sprite::create(cha->getCharacterFace(face));
          sp->setAnchorPoint(Vec2(0.5, 0));
          _charactersLayer->addChild(sp);
          cha->currentPosition = CMPositionType::EMPTY;
        }
        cha->_sprite = sp;
        cha->key = name;
        cha->currentFace = face;

        updateDisplayCharacter();
      }
    }

    playBackgroundMusic(GameSystem::getInstance()->getGameSceneInfo()->bgmKey);

    if (GameSystem::getInstance()->getGameSceneInfo()->optionsNumber) {
      showSelect(GameSystem::getInstance()->getGameSceneInfo()->options);
    }

    auto sign = GameSystem::getInstance()->getGameSceneInfo()->currentSignName;
    auto commandIndex =
        GameSystem::getInstance()->getGameSceneInfo()->currentCommandIndex;
    ScriptReader::getInstance()->jumpToSign(sign, commandIndex);
  }
}

void GameScene::showSelect(std::map<std::string, std::string> &options) {
  Config &vars = *Config::getInstance();
  _currentOptions = options;

  auto menu = Menu::create();
  int startY = options.size() * (60) / 2;
  int size = options.size();

  for (auto itr = options.begin(); itr != options.end(); itr++) {
    _optionsNumber++;
    auto label = Label::createWithSystemFont(itr->second, vars["arial"], 30);
    label->setColor(Color3B::WHITE);
    label->enableShadow();
    auto tmp = itr->first;
    auto tmp2 = itr->second;
    auto button = MenuItemLabel::create(label, [=](Ref *) {
      HistoryLogger::getInstance()->addRecord("null", tmp2, "");
      ScriptReader::getInstance()->jumpToSign(tmp);
      menu->removeFromParent();
      _currentOptions.clear();
      _optionsNumber = 0;
    });

    menu->addChild(button);
    button->setPosition(0, startY);
    startY -= 60;
  }
  _selectLayer->addChild(menu, 13);
}

void GameScene::showHistoryScene() {
  Director::getInstance()->pushScene(HistoryScene::createScene());
}

void GameScene::skipAction() {
  auto bgList = _backgroundLayer->getChildren();
  for (auto iter = bgList.begin(); iter != bgList.end(); iter++) {
    if ((*iter)->getActionByTag(1)) {
      (*iter)->getActionByTag(1)->step(3.0f);
    }
  }

  auto chaList = _charactersLayer->getChildren();
  for (auto iter = chaList.begin(); iter != chaList.end(); iter++) {
    if ((*iter)->getActionByTag(1)) {
      (*iter)->getActionByTag(1)->step(3.0f);
    }
  }
}

void GameScene::showWaitingPrompt() {
  _wtIcon->setOpacity(255);
  auto action = FadeOut::create(0.8f);
  auto actionBack = action->reverse();
  _wtIcon->runAction(
      RepeatForever::create(Sequence::create(action, actionBack, NULL)));

  if (_isAutoPlaying) {
    this->runAction(Sequence::create(
        DelayTime::create(GameSystem::getInstance()->getAutoSpeed()),
        CallFunc::create(CC_CALLBACK_0(GameScene::dialogClicked, this)), NULL));
  }
}

void GameScene::hideWaitingPrompt() {
  _wtIcon->stopAllActions();
  _wtIcon->setOpacity(0);
}

void GameScene::selectEventOfSkip(Ref *pSender, CheckBox::EventType type) {
  switch (type) {
    case CheckBox::EventType::SELECTED:

      startSkipPlay();
      break;

    case CheckBox::EventType::UNSELECTED:

      stopSkipPlay();
      break;
    default:
      break;
  }
}

void GameScene::selectEventOfAuto(cocos2d::Ref *pSender,
                                  CheckBox::EventType type) {
  switch (type) {
    case CheckBox::EventType::SELECTED:
      startAutoPlay();
      break;

    case CheckBox::EventType::UNSELECTED:
      stopAutoPlay();
      break;
    default:
      break;
  }
}

void GameScene::showMenuScene() {
  Config &vars = *Config::getInstance();
  Director::getInstance()->pause();

  auto popupDialog =
      PopupLayer::create(vars["yaadv/ui/popupwindow/bg_popup.png"]);
  popupDialog->addLabelButton(vars["Yes"],
                              CC_CALLBACK_0(GameScene::showMenuSceneYes, this));
  popupDialog->addLabelButton(vars["No"],
                              CC_CALLBACK_0(GameScene::showMenuSceneNo, this));

  popupDialog->setString(vars["Do you want to return to menu?"]);
  this->addChild(popupDialog, 255);
}

void GameScene::showMenuSceneYes() {
  Director::getInstance()->resume();

  auto backLayer = LayerColor::create(Color4B::BLACK);

  GameSystem::getInstance()->setGameScene(MainMenu::createScene());
  auto scene = GameSystem::getInstance()->getGameScene();
  Director::getInstance()->replaceScene(scene);
}

void GameScene::showMenuSceneNo() { Director::getInstance()->resume(); }

void GameScene::showMenuLayer() {
  Config *_vars = Config::getInstance();
  Config &vars = *_vars;
  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  _btnMenu->setOpacity(0);

  ScreenShoot();

  _menuLayer = Layer::create();

  auto background = LayerColor::create(Color4B(30, 30, 30, 128));
  _menuLayer->addChild(background);

  auto menuBackground =
      Sprite::create(vars["yaadv/ui/dialog_menu/bg_menu.png"]);
  menuBackground->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
  menuBackground->setPosition(visibleSize.width + origin.x,
                              visibleSize.height + origin.y);
  _menuLayer->addChild(menuBackground);

  auto btnSave = MenuItemImage::create(
      vars["yaadv/ui/dialog_menu/btn_menu_save_normal.png"],
      vars["yaadv/ui/dialog_menu/btn_menu_save_touch.png"],
      CC_CALLBACK_0(GameScene::showSaveScene, this));

  auto btnLoad = MenuItemImage::create(
      vars["yaadv/ui/dialog_menu/btn_menu_load_normal.png"],
      vars["yaadv/ui/dialog_menu/btn_menu_load_touch.png"],
      CC_CALLBACK_0(GameScene::showLoadScene, this));

  auto btnAuto = MenuItemImage::create(
      vars["yaadv/ui/dialog_menu/btn_menu_auto_normal.png"],
      vars["yaadv/ui/dialog_menu/btn_menu_auto_touch.png"],
      CC_CALLBACK_0(GameScene::startAutoPlay, this));

  auto btnSkip = MenuItemImage::create(
      vars["yaadv/ui/dialog_menu/btn_menu_skip_normal.png"],
      vars["yaadv/ui/dialog_menu/btn_menu_skip_touch.png"],
      CC_CALLBACK_0(GameScene::startSkipPlay, this));

  auto btnLog = MenuItemImage::create(
      vars["yaadv/ui/dialog_menu/btn_menu_log_normal.png"],
      vars["yaadv/ui/dialog_menu/btn_menu_log_touch.png"],
      CC_CALLBACK_0(GameScene::showHistoryScene, this));

  auto btnConfig = MenuItemImage::create(
      vars["yaadv/ui/dialog_menu/btn_menu_config_normal.png"],
      vars["yaadv/ui/dialog_menu/btn_menu_config_touch.png"],
      CC_CALLBACK_0(GameScene::showConfigScene, this));

  auto btnTitle = MenuItemImage::create(
      vars["yaadv/ui/dialog_menu/btn_menu_title_normal.png"],
      vars["yaadv/ui/dialog_menu/btn_menu_title_touch.png"],
      CC_CALLBACK_0(GameScene::showMenuScene, this));

  auto menu = Menu::create(btnSave, btnLoad, btnAuto, btnSkip, btnLog,
                           btnConfig, btnTitle, NULL);

  menu->setPosition(
      visibleSize.width + origin.x - menuBackground->getContentSize().width / 2,
      visibleSize.height + origin.y -
          menuBackground->getContentSize().height / 2);
  menu->alignItemsVerticallyWithPadding(0.0f);
  _menuLayer->addChild(menu);

  this->addChild(_menuLayer, 20);

  auto backgroundTouch = EventListenerTouchOneByOne::create();
  backgroundTouch->onTouchBegan = [=](Touch *t, Event *e) { return true; };
  backgroundTouch->onTouchEnded = [=](Touch *t, Event *e) {

    this->runAction(Sequence::create(
        CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, _menuLayer)),
        CallFunc::create(
            CC_CALLBACK_0(MenuItemImage::setOpacity, _btnMenu, 255)),
        NULL));
  };
  backgroundTouch->setSwallowTouches(true);
  _menuLayer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
      backgroundTouch, background);

  auto backgroundSpriteTouch = EventListenerTouchOneByOne::create();
  backgroundSpriteTouch->onTouchBegan = [=](Touch *t, Event *e) {
    if (menuBackground->getBoundingBox().containsPoint(
            this->convertTouchToNodeSpace(t))) {
      return true;
    }
    return false;
  };
  backgroundSpriteTouch->setSwallowTouches(true);
  menuBackground->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
      backgroundSpriteTouch, menuBackground);
}

}  // namespace yaadv
