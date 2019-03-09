#ifndef __SCRIPT_READER_JSON_H__
#define __SCRIPT_READER_JSON_H__
#include "cocos2d.h"

USING_NS_CC;
struct Character;


#include "ScriptReader.h"
#include "json.hpp"

namespace yaadv {
class ScriptReaderJSON : public ScriptReader {
 public:


  //  std::map<std::string, std::vector<ScriptCommand *> *> _scripts;

  //  std::map<std::string, int> _record;
  //  bool _isHaveRead;

  //std::string _currentSignName;
//   int _currentCommandIndex;

  static ScriptReaderJSON *_instance;


  cocos2d::FiniteTimeAction *_currentAction;

 public:
  ScriptReaderJSON();
  ~ScriptReaderJSON();

  static ScriptReader *getInstance();

  static void destoryInstance();

  //    Sprite *charSprites[5];
  //    Layer *charStage;

  //    Character **chars[5];
  //    int *charNumber;

  //    bool isWaitingForSelection;

  //    Node *stage;

  //    bool nextPositionIsLeft;

  void initWithStage(Node *stage);

  //    std::function<void(std::string &text)> showText;
  //    std::function<void(std::string &text)> showName;
  //    std::function<void(std::string &background)> changeBackground;
  //    std::function<void(std::string &file)> playBackgroundMusic;
  //    std::function<void()> stopBackgroundMusic;
  //    std::function<void(std::string &file)> playSound;
  //    std::function<void()> stopSound;
  //    std::function<void(std::string &cName, std::string &face)>
  //    showCharacter; std::function<void(std::string &name)> hideCharacter;
  //    std::function<void(std::map<std::string, std::string> &options)>
  //    showSelect; std::function<void()> returnToMenu;

  void loadScriptFile(std::string path);
  void clearScript();
  void jumpToSign(const std::string &sign);
  void jumpToSign(const std::string &sign, int index);

  void nextScript();

  std::string getCurrentSignName();
  int getCurrentCommandIndex();
  void setCurrentCommandIndex(int value);

  bool getIsHaveRead();

  void runTick();
};
};  // namespace yaadv
#endif
