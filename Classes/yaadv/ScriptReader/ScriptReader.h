#ifndef __SCRIPT_READER_H__
#define __SCRIPT_READER_H__
#include "ScriptCommand.h"
#include "cocos2d.h"
USING_NS_CC;

namespace yaadv {
struct Character;

class ScriptReader {
 public:
  std::map<std::string, std::vector<ScriptCommand *> *> _scripts;
  std::map<std::string, SCSelect *> _selects;

  std::map<std::string, int> _record;
  bool _isHaveRead;

  std::string _currentSignName;
  int _currentCommandIndex;

  static ScriptReader *_instance;

 public:
  ScriptReader();
  ~ScriptReader();

  static ScriptReader *getInstance();
  static void setInstance(ScriptReader *a) { _instance = a; }
  static void destoryInstance();

  Sprite *charSprites[5];
  Layer *charStage;

  Character **chars[5];
  int *charNumber;

  bool isWaitingForSelection;

  Node *stage;

  bool nextPositionIsLeft;

  virtual void initWithStage(Node *stage);

  std::function<void(std::string &text, std::string &loc, std::string &face)> showImage;

  std::function<void(std::string &text, std::string &loc, float dur, std::string &ease, std::string &face)> imageTween;
  std::function<void(std::string &text)> showText;
  std::function<void(std::string &text)> showName;
  std::function<void(std::string &background)> changeBackground;
  std::function<void(std::string &file)> playBackgroundMusic;
  std::function<void()> stopBackgroundMusic;
  std::function<void(std::string &file)> playSound;
  std::function<void()> stopSound;
  std::function<void(std::string &cName, std::string &face)> showCharacter;
  std::function<void(std::string &name)> hideCharacter;
  std::function<void(std::string &name)> hideImage;
  std::function<void(std::map<std::string, std::string> &options)> showSelect;
  std::function<void()> returnToMenu;
  std::function<void(int)> clear;

  virtual void loadScriptFile(std::string path);
  virtual void clearScript();
  virtual void jumpToSign(const std::string &sign);
  virtual void jumpToSign(const std::string &sign, int index);

  virtual void nextScript();

  virtual std::string getCurrentSignName();
  virtual int getCurrentCommandIndex();
  virtual void setCurrentCommandIndex(int value);

  virtual bool getIsHaveRead();
};

};  // namespace yaadv

#include "SCX.h"
#include "ScriptReaderJSON.h"
#include "ScriptReaderWREN.h"
//#include "ScriptReaderYASL.h"

#endif
