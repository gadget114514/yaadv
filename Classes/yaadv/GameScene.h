
#ifndef GAMESCENE_H

#define GAMESCENE_H
#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "CharLabel.h"
#include "ScriptReader/CharacterManager.h"
#include "ScriptReader/ScriptReader.h"

#define MAX_character_NUMBER 5

#define TEXTLABEL_SIZE_WIDTH 800
#define TEXTLABEL_SIZE_HEIGHT 100

#define SCREEN_SHOOT_WIDTH 156
#define SCREEN_SHOOT_HEIGHT 99

USING_NS_CC;

namespace yaadv {
using namespace ui;

class GameScene : public cocos2d::Layer {
  cocos2d::Label *_label;

  Label *_nameLabel;
  Sprite *_nameWindow;
  CharLabel *_textLabel;
  Sprite *_dialogWindow;
  Layer *_backgroundLayer;
  Sprite *_backgroundSprite;
  Layer *_charactersLayer;
  Layer *_selectLayer;
  Layer *_menuLayer;
  Sprite *_wtIcon;
  Sprite *_autoIcon;
  Sprite *_skipIcon;

  MenuItemImage *_btnMenu;

  CheckBox *CBskip;
  CheckBox *CBauto;

  std::vector<Character *> *_characters;
  std::vector<YaImg *> *_yaimgs;

  bool _isPlayingMusic;
  bool _isAutoPlaying;
  bool _isSkipPlaying;
  bool _immediate;

  std::string _backgroundKey;
  std::string _backgroundMusicKey;
  std::string _soundKey;
  std::string _currentText;
  std::string _currentName;
  std::map<std::string, std::string> _currentOptions;
  int _optionsNumber;

  void showMenuSceneYes();
  void showMenuSceneNo();

  void updateDisplayYaImg();
  void updateDisplayCharacter();

 public:
  GameScene();
  ~GameScene();

  static cocos2d::Scene *createScene();

  virtual bool init();

  CREATE_FUNC(GameScene);

  void update(float dt);

  void updateLabel(std::string text);

  void showName(std::string &name);

  void showImage(std::string &text, std::string &loc, std::string &face);
  void imageTween(std::string &text, std::string &loc, float dur, std::string &ease, std::string &face);

  void showText(std::string &text);

  void showClickSign();

  void screenClicked();

  void dialogClicked();

  void changeBackground(std::string &key);

  void playBackgroundMusic(std::string &key);
  void stopBackgroundMusic();
  void scheduleVolumeFadeIn();
  void scheduleVolumeFadeOut();

  void playSound(std::string &file);
  void stopSound();

  void startAutoPlay();
  void stopAutoPlay();
  void startSkipPlay();
  void stopSkipPlay();
  void autoPlay(float dt);
  void skipPlay(float dt);

  void selectEventOfSkip(Ref *pSender, CheckBox::EventType type);
  void selectEventOfAuto(Ref *pSender, CheckBox::EventType type);

  void displayYaImg(std::string cName, std::string loc, std::string face);
  void hideYaImg(std::string cName);

  void displayCharacter(std::string cName, std::string face);
  void hideCharacter(std::string cName);
  void hideImage(std::string cName);
  void createGameDate();

  void showSaveScene();
  void showLoadScene();
  void showHistoryScene();
  void showMenuScene();
  void showMenuLayer();
  void showConfigScene();

  void ScreenShoot();

  void clear(int sw = 1);
  void reloadScene();

  void showSelect(std::map<std::string, std::string> &options);

  void skipAction();
  void showWaitingPrompt();
  void hideWaitingPrompt();
};
};  // namespace yaadv

#endif
