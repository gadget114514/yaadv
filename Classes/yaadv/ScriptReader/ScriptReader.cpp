#include "ScriptReader.h"

#include "CharacterManager.h"
#include "../GameSystem.h"
#include "../MainMenuScene.h"
#include "SCX.h"


USING_NS_CC;
using namespace yaadv;
namespace yaadv {

ScriptReader *ScriptReader::_instance = nullptr;

ScriptReader::ScriptReader()
    : _currentSignName("gameStart"),
      _currentCommandIndex(0),
      nextPositionIsLeft(true),
      charNumber(0),
      stage(nullptr),
      isWaitingForSelection(0) {}

ScriptReader::~ScriptReader() { clearScript(); }

void ScriptReader::initWithStage(Node *stage) {
  this->stage = stage;
  charStage = Layer::create();
  stage->addChild(charStage);
  for (int i = 0; i < 5; i++) {
    chars[i] = nullptr;
  }
  _currentSignName = "gameStart";
  _currentCommandIndex = 0;
}

ScriptReader *ScriptReader::getInstance() {
  if (_instance == nullptr) {
    _instance = new ScriptReader();
  }
  return _instance;
}

void ScriptReader::destoryInstance() { CC_SAFE_DELETE(_instance); }

void ScriptReader::clearScript() {
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

void ScriptReader::loadScriptFile(std::string path) {
  clearScript();

  std::string ss = FileUtils::getInstance()->getStringFromFile(path);

  int sPos = 0;
  int ePos = 0;

  std::string currentSign = "default";
  auto cms = new std::vector<ScriptCommand *>();
  _scripts.insert(
      std::pair<std::string, std::vector<ScriptCommand *> *>(currentSign, cms));

  log("LoadScript::>");

  SCSelect *currentSelect = nullptr;

  while (sPos >= 0) {
    ePos = ss.find('\n', sPos);
    std::string command;
    if (ePos < 0) {
      command = ss.substr(sPos, ss.length() - sPos);
      sPos = -1;
    } else {
      if (ss[ePos - 1] == 13)
        command = ss.substr(sPos, ePos - sPos - 1);
      else
        command = ss.substr(sPos, ePos - sPos);
      sPos = ePos + 1;
    }

    if (command.length() > 1 && !command.empty()) {
      if (command[0] == '/') {
      } else {
        if (command[0] == '@') {
          currentSign = command.substr(1, command.length() - 1);
          cms = new std::vector<ScriptCommand *>();
          _scripts.insert(
              std::pair<std::string, std::vector<ScriptCommand *> *>(
                  currentSign, cms));
          log("SC> Add sign[%s]", currentSign.c_str());
        } else {
          if (command[0] == '#') {
            std::string cmd = command.substr(1, command.length() - 1);
            int scriptNamePos = cmd.find_first_of(':', 0);
            std::string cmdParams = "";
            if (scriptNamePos > 0) {
              cmdParams = cmd.substr(scriptNamePos + 1,
                                     cmd.length() - scriptNamePos - 1);
              cmd = cmd.substr(0, scriptNamePos);
            }
            log("SC> Add command[%s] params[%s] Sign[%s]", cmd.c_str(),
                cmdParams.c_str(), currentSign.c_str());

            if (cmd.compare("Select") == 0) {
              SCSelect *selectCMD = new SCSelect(this);
              currentSelect = selectCMD;
              cms->push_back(selectCMD);
            } else if (cmd.compare("GameOver") == 0) {
              SCGameOver *gameOverCMD = new SCGameOver(this);
              cms->push_back(gameOverCMD);
            } else if (cmd.compare("Leave") == 0) {
              SCLeave *leaveCMD = new SCLeave(this, cmdParams);
              cms->push_back(leaveCMD);
            } else if (cmd.compare("Set") == 0) {
              std::string key;
              char mark;
              int value;
              int pos;
              pos = cmdParams.find_first_of("+-=", 0);
              key = cmdParams.substr(0, pos);
              mark = cmdParams[pos];
              cmdParams = cmdParams.substr(pos + 1, cmdParams.length() - 1);
              pos = cmdParams.find_first_of("+-", 0);
              if (pos < 0) {
                pos = cmdParams.find_first_of("=", 0);
                value = atoi(cmdParams.c_str());
              } else {
                char tmpChar = cmdParams[pos];
                std::string tmpString;
                switch (tmpChar) {
                  case '-':

                    break;
                  case '+':

                    break;
                  default:
                    break;
                }
              }
              SCSet *setCMD = new SCSet(this, key, mark, value);
              cms->push_back(setCMD);
            } else if (cmd.compare("Jump") == 0) {
              SCJump *jumpCMD = new SCJump(this, cmdParams);
              cms->push_back(jumpCMD);
            } else if (cmd.compare("Background") == 0) {
              SCBackground *backgroundCMD = new SCBackground(this, cmdParams);
              cms->push_back(backgroundCMD);
            } else if (cmd.compare("PlayBGM") == 0) {
              SCPlayBGM *playBGMCMD = new SCPlayBGM(this, cmdParams);
              cms->push_back(playBGMCMD);
            } else if (cmd.compare("StopBGM") == 0) {
              SCStopBGM *stopBGMCMD = new SCStopBGM(this);
              cms->push_back(stopBGMCMD);
            } else if (cmd.compare("PlaySound") == 0) {
              SCPlaySound *playSoundCMD = new SCPlaySound(this, cmdParams);
              cms->push_back(playSoundCMD);
            } else if (cmd.compare("StopSound") == 0) {
              SCStopSound *stopSoundCMD = new SCStopSound(this);
              cms->push_back(stopSoundCMD);
            } else if (cmd.compare("If") == 0) {
              std::string expression = "";
              std::string trueTag = "";
              std::string falseTag = "";
              int pos;

              pos = cmdParams.find_first_of(":", 0);
              expression = cmdParams.substr(0, pos);

              cmdParams = cmdParams.substr(pos + 1, cmdParams.length() - 1);
              pos = cmdParams.find_first_of(":", 0);
              if (pos < 0) {
                trueTag = cmdParams;

              } else {
                trueTag = cmdParams.substr(0, pos);
                cmdParams = cmdParams.substr(pos + 1, cmdParams.length() - 1);
                falseTag = cmdParams.substr(0, pos);
              }
              SCIf *ifCMD = new SCIf(this, expression, trueTag, falseTag);
              cms->push_back(ifCMD);
            } else {
              log("Unknow Script Command> [%s]:[%s]", cmd.c_str(),
                  cmdParams.c_str());
            }

          } else {
            if (command[0] == '>') {
              if (currentSelect != nullptr) {
                std::string cmd = command.substr(1, command.length() - 1);
                int scriptNamePos = cmd.find_first_of(':', 0);
                std::string cmdParams = "";
                if (scriptNamePos > 0) {
                  cmdParams = cmd.substr(scriptNamePos + 1,
                                         cmd.length() - scriptNamePos - 1);
                  cmd = cmd.substr(0, scriptNamePos);
                }
                log("SC> options Sign[%s] text[%s]", cmd.c_str(),
                    cmdParams.c_str());
                currentSelect->addOption(cmd, cmdParams);
              }
            } else {
              std::string cmd = command;
              int scriptNamePos = cmd.find_first_of(':', 0);
              std::string cmdParams = "";
              std::string face = "";
              if (scriptNamePos < 0) {
                log("SC Csay Text[%s]", cmd.c_str());
                SCCharacterSpeak *csCMD =
                    new SCCharacterSpeak(this, face, std::string(""), cmd, face);
                cms->push_back(csCMD);
              } else {
                cmdParams = cmd.substr(scriptNamePos + 1,
                                       cmd.length() - scriptNamePos - 1);
                cmd = cmd.substr(0, scriptNamePos);
                scriptNamePos = cmd.find_first_of('#', 0);
                if (scriptNamePos > 0) {
                  face = cmd.substr(scriptNamePos + 1,
                                    cmd.length() - scriptNamePos - 1);
                  cmd = cmd.substr(0, scriptNamePos);
                }
                log("SC Csay CHA[%s] Face[%s] Text[%s]", cmd.c_str(),
                    face.c_str(), cmdParams.c_str());
                SCCharacterSpeak *csCMD =
                    new SCCharacterSpeak(this, std::string(""), cmd, cmdParams, face);
                cms->push_back(csCMD);
              }
            }
          }
        }
      }
    }
  }
}

void ScriptReader::jumpToSign(const std::string &sign) {
  log("sign: %s", sign.c_str());
  if (sign.compare("") == 0) {
    log("Sign is null");
    return;
  }
  isWaitingForSelection = false;
  auto list = _scripts.find(sign);
  if (list == _scripts.end()) {
    log("Sign [%s] not found", sign.c_str());
    return;
  }
  _currentSignName = sign;
  _currentCommandIndex = 0;
  nextScript();
}

void ScriptReader::nextScript() {
  if (isWaitingForSelection) {
    log("waiting");
    return;
  }
  _currentCommandIndex++;

  auto list = _scripts.find(_currentSignName);
  if (list == _scripts.end()) {
    log("No Sign of currentSign [%s]", _currentSignName.c_str());
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
    log("End of Script..! CurrentScript");

    if (returnToMenu) {
      returnToMenu();
    } else {
      log("returnToMenu >>> CallBack not linked!!.");
    }
    return;
  }

  auto cmd = cmdList->at(_currentCommandIndex - 1);

  log("sign = %s, commandIndex = %d", _currentSignName.c_str(),
      _currentCommandIndex);

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
      log("Unhandle ScriptCommandType [%d]", cmd->type);
  }
}

std::string ScriptReader::getCurrentSignName() { return _currentSignName; }

int ScriptReader::getCurrentCommandIndex() { return _currentCommandIndex; }

void ScriptReader::setCurrentCommandIndex(int value) {
  _currentCommandIndex = value;
}

void ScriptReader::jumpToSign(const std::string &sign, int index) {
  if (sign.compare("") == 0) {
    log("Sign is null");
    return;
  }
  isWaitingForSelection = false;
  auto list = _scripts.find(sign);
  if (list == _scripts.end()) {
    log("Sign [%s] not found", sign.c_str());
    return;
  }
  _currentSignName = sign;
  _currentCommandIndex = index;
}

bool ScriptReader::getIsHaveRead() { return _isHaveRead; }

} // namespace yaadv