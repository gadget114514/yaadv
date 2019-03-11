#include "ScriptReader.h"

#include "CharacterManager.h"
#include "../GameSystem.h"
#include "../MainMenuScene.h"
#include "SCX.h"


USING_NS_CC;
using namespace yaadv;
namespace yaadv {

ScriptReaderJSON *ScriptReaderJSON::_instance = nullptr;

ScriptReaderJSON::ScriptReaderJSON()
 {}

ScriptReaderJSON::~ScriptReaderJSON() { clearScript(); }

void ScriptReaderJSON::initWithStage(Node *stage) {
  this->stage = stage;
  charStage = Layer::create();
  stage->addChild(charStage);
  for (int i = 0; i < 5; i++) {
    chars[i] = nullptr;
  }
  _currentSignName = "gameStart";
  _currentCommandIndex = 0;
}

ScriptReader *ScriptReaderJSON::getInstance() {
  if (_instance == nullptr) {
    _instance = new ScriptReaderJSON();
  }
  return _instance;
}

void ScriptReaderJSON::destoryInstance() { CC_SAFE_DELETE(_instance); }

void ScriptReaderJSON::clearScript() {
  for (auto itr = _scripts.begin(); itr != _scripts.end(); itr++) {
    auto list = itr->second;
    int size = list->size();
    ("DeleteList size[%d]", size);
    if (size > 0) {
      for (int i = 0; i < size; i++) {
        auto ptr = list->at(i);
        switch (ptr->type) {
          case ScriptCommandType::CharacterSpeak:
            delete (SCCharacterSpeak *)ptr;
            break;
          case ScriptCommandType::Leave:
            delete (SCLeave *)ptr;
            break;
          case ScriptCommandType::Jump:
            delete (SCJump *)ptr;
            break;
          case ScriptCommandType::Select:
            delete (SCSelect *)ptr;
            break;
          case ScriptCommandType::Background:
            delete (SCBackground *)ptr;
            break;
          case ScriptCommandType::PlayBGM:
            delete (SCPlayBGM *)ptr;
            break;
          case ScriptCommandType::StopBGM:
            delete (SCStopBGM *)ptr;
            break;
          case ScriptCommandType::PlaySd:
            delete (SCPlaySound *)ptr;
            break;
          case ScriptCommandType::StopSd:
            delete (SCStopSound *)ptr;
            break;
          case ScriptCommandType::Set:
            delete (SCSet *)ptr;
            break;
          case ScriptCommandType::If:
            delete (SCIf *)ptr;
            break;
          case ScriptCommandType::GameOver:
            delete (SCGameOver *)ptr;
            break;
        }

        ptr = nullptr;
      }
    }
    list->clear();
    delete list;
    list = nullptr;
  }
  _scripts.clear();
  isWaitingForSelection = false;
}

void ScriptReaderJSON::loadScriptFile(std::string path) {
  clearScript();

  std::string ss = FileUtils::getInstance()->getStringFromFile(path);

  int sPos = 0;
  int ePos = 0;

  std::string currentSign = "default";
#if 0
  auto cms = new std::vector<ScriptCommand *>();
  _scripts.insert(
      std::pair<std::string, std::vector<ScriptCommand *> *>(currentSign, cms));
#endif

}

void ScriptReaderJSON::jumpToSign(const std::string &sign) {
  if (sign.compare("") == 0) {
    return;
  }
  isWaitingForSelection = false;
  auto list = _scripts.find(sign);
  if (list == _scripts.end()) {
    return;
  }
  _currentSignName = sign;
  _currentCommandIndex = 0;
  nextScript();
}

void ScriptReaderJSON::nextScript() {
  if (isWaitingForSelection) {
    return;
  }
  _currentCommandIndex++;

  auto list = _scripts.find(_currentSignName);
  if (list == _scripts.end()) {
    return;
  }

  std::string readedSign = "readed_" + _currentSignName;

  if (_currentCommandIndex >
      GameSystem::getInstance()->getHaveRead(readedSign)) {
    GameSystem::getInstance()->setHaveRead(readedSign, _currentCommandIndex);
    _isHaveRead = false;
  } else {
    _isHaveRead = true;
  }

  auto cmdList = list->second;
  if (_currentCommandIndex - 1 >= (int)cmdList->size()) {
    if (returnToMenu) {
      returnToMenu();
    } else {
      ;
    }
    return;
  }

  auto cmd = cmdList->at(_currentCommandIndex - 1);


  switch (cmd->type) {
    case ScriptCommandType::CharacterSpeak:
      ((SCCharacterSpeak *)cmd)->execute(stage);
      break;
    case ScriptCommandType::Leave:
      ((SCLeave *)cmd)->execute(stage);
      break;
    case ScriptCommandType::Jump:
      ((SCJump *)cmd)->execute(stage);
      break;
    case ScriptCommandType::Select:
      ((SCSelect *)cmd)->execute(stage);
      break;
    case ScriptCommandType::Background:
      ((SCBackground *)cmd)->execute(stage);
      break;
    case ScriptCommandType::PlayBGM:
      ((SCPlayBGM *)cmd)->execute(stage);
      break;
    case ScriptCommandType::StopBGM:
      ((SCStopBGM *)cmd)->execute(stage);
      break;
    case ScriptCommandType::PlaySd:
      ((SCPlaySound *)cmd)->execute(stage);
      break;
    case ScriptCommandType::StopSd:
      ((SCStopSound *)cmd)->execute(stage);
      break;
    case ScriptCommandType::Set:
      ((SCSet *)cmd)->execute(stage);
      break;
    case ScriptCommandType::If:
      ((SCIf *)cmd)->execute(stage);
      break;
    case ScriptCommandType::GameOver:
      ((SCGameOver *)cmd)->execute(stage);
      break;
    default:
      break;
  }
}

std::string ScriptReaderJSON::getCurrentSignName() { return _currentSignName; }

int ScriptReaderJSON::getCurrentCommandIndex() { return _currentCommandIndex; }

void ScriptReaderJSON::setCurrentCommandIndex(int value) {
  _currentCommandIndex = value;
}

void ScriptReaderJSON::jumpToSign(const std::string &sign, int index) {
  if (sign.compare("") == 0) {
    return;
  }
  isWaitingForSelection = false;
  auto list = _scripts.find(sign);
  if (list == _scripts.end()) {
    return;
  }
  _currentSignName = sign;
  _currentCommandIndex = index;
}

bool ScriptReaderJSON::getIsHaveRead() { return _isHaveRead; }

} //namespace yaadv
