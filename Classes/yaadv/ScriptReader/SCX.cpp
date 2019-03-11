#include "SCX.h"
#include "BackgroundManager.h"
#include "ScriptCommand.h"
#include "ScriptReader.h"
#include "cocos2d.h"
USING_NS_CC;

using namespace cocos2d;
using namespace std;

namespace yaadv {

SCBackground::SCBackground(ScriptReader *reader, std::string key) : key(key) {
  this->reader = reader;
  this->type = ScriptCommandType::Background;
}

SCBackground::~SCBackground() {}

void SCBackground::execute(cocos2d::Node *stage) {
  this->reader->changeBackground(key);
  this->reader->nextScript();
}

SCClear::SCClear(ScriptReader *reader, int sw) : _sw(sw) {
  this->reader = reader;
  this->type = ScriptCommandType::Clear;
}

SCClear::~SCClear() {}

void SCClear::execute(cocos2d::Node *stage) {
  this->reader->clear(_sw);
  this->reader->nextScript();
}

SCCharacterSpeak::SCCharacterSpeak(ScriptReader *reader, std::string &name,
                                   std::string &displayname, std::string &text,
                                   std::string &face)
    : cName(name), displayname(displayname), face(face), text(text) {
  this->reader = reader;
  this->type = ScriptCommandType::CharacterSpeak;
}

SCCharacterSpeak::~SCCharacterSpeak() {}

}  // namespace yaadv

#include "../History.h"
#include "CharacterManager.h"

namespace yaadv {
void SCCharacterSpeak::execute(cocos2d::Node *stage) {

  Character *cha = CM->getCharacter(cName);
  std::string showName = cName;
  if (!cha) {
  } else {
    showName = cha->name;
  }

  if (cName.compare("") != 0) {
    if (face.compare("") != 0) {
      reader->showCharacter(cName, face);
    }
  } else {
    showName = "";
  }
  if (displayname.compare("") != 0) showName = displayname;
  reader->showName(showName);

  if (text.compare("") == 0) {
    reader->nextScript();
  } else {
    reader->showText(text);
  }
  if (showName.size() > 0 && text.compare("")) {
    HistoryLogger::getInstance()->addRecord("talk", text, showName);
  } else {
    HistoryLogger::getInstance()->addRecord("null", text, "");
  }
}

SCGameOver::SCGameOver(ScriptReader *reader) {
  this->reader = reader;
  this->type = ScriptCommandType::GameOver;
}

SCGameOver::~SCGameOver() {}

void SCGameOver::execute(cocos2d::Node *stage) { reader->returnToMenu(); }
}  // namespace yaadv

#include <sstream>
#include "../GameSystem.h"

namespace yaadv {
SCIf::SCIf(ScriptReader *reader, std::string expression, std::string trueTag,
           std::string falseTag)
    : _expression(expression), _trueTag(trueTag), _falseTag(falseTag) {
  this->reader = reader;
  this->type = ScriptCommandType::If;
}
SCIf::~SCIf() {}

void SCIf::execute(cocos2d::Node *stage) {
  if (_expression.compare("") == 0) {
    reader->nextScript();
  }
  if (expressionJudge()) {
    jump(_trueTag);
  } else {
    jump(_falseTag);
  }
}

bool SCIf::expressionJudge() {
  int pos;
  std::string exp = _expression;
  pos = exp.find_first_of("><=", 0);
  if (pos < 0) {
    return true;
  } else {
    std::string sFirstValue;
    std::string sSecondValue;
    int iFirstValue;
    int iSecondValue;
    std::stringstream ssFirstValue;
    std::stringstream ssSecondValue;

    sFirstValue = exp.substr(0, pos);
    if ((exp[0] >= '0') && (exp[0] <= '9')) {
      ssFirstValue << sFirstValue;
      ssFirstValue >> iFirstValue;
    } else {
      iFirstValue = GameSystem::getInstance()->getDataValue(sFirstValue);
    }

    if (exp[pos] == '>') {
      if (exp[pos + 1] == '=') {
        sSecondValue = exp.substr(pos + 2, exp.length() - 1);
        if ((exp[pos + 3] >= '0') && (exp[pos + 3] <= '9')) {
          ssSecondValue << sSecondValue;
          ssSecondValue >> iSecondValue;
        } else {
          iSecondValue = GameSystem::getInstance()->getDataValue(sSecondValue);
        }

        if (iFirstValue >= iSecondValue)
          return true;
        else
          return false;
      } else {
        sSecondValue = exp.substr(pos + 1, exp.length() - 1);
        if ((exp[pos + 2] >= '0') && (exp[pos + 2] <= '9')) {
          ssSecondValue << sSecondValue;
          ssSecondValue >> iSecondValue;
        } else {
          iSecondValue = GameSystem::getInstance()->getDataValue(sSecondValue);
        }

        if (iFirstValue > iSecondValue)
          return true;
        else
          return false;
      }
    } else if (exp[pos] == '<') {
      if (exp[pos + 1] == '=') {
        sSecondValue = exp.substr(pos + 2, exp.length() - 1);
        if ((exp[pos + 3] >= '0') && (exp[pos + 3] <= '9')) {
          ssSecondValue << sSecondValue;
          ssSecondValue >> iSecondValue;
        } else {
          iSecondValue = GameSystem::getInstance()->getDataValue(sSecondValue);
        }

        if (iFirstValue <= iSecondValue)
          return true;
        else
          return false;
      } else {
        sSecondValue = exp.substr(pos + 1, exp.length() - 1);
        if ((exp[pos + 2] >= '0') && (exp[pos + 2] <= '9')) {
          ssSecondValue << sSecondValue;
          ssSecondValue >> iSecondValue;
        } else {
          iSecondValue = GameSystem::getInstance()->getDataValue(sSecondValue);
        }

        if (iFirstValue < iSecondValue)
          return true;
        else
          return false;
      }

    } else if (exp[pos] == '=') {
      sSecondValue = exp.substr(pos + 1, exp.length() - 1);
      if ((sSecondValue[0] >= '0') && (sSecondValue[0] <= '9')) {
        ssSecondValue << sSecondValue;
        ssSecondValue >> iSecondValue;
      } else {
        iSecondValue = GameSystem::getInstance()->getDataValue(sSecondValue);
      }

      if (iFirstValue == iSecondValue)
        return true;
      else
        return false;
    }
  }
  return false;
}

void SCIf::jump(std::string tag) {
  if (tag.compare("") == 0) {
    reader->nextScript();
    return;
  } else {
    reader->jumpToSign(tag);
  }
}

SCImg::SCImg(ScriptReader *reader, std::string &name, std::string &loc,
             std::string &face)
    : _name(name), _loc(loc), _face(face) {
  this->reader = reader;
  this->type = ScriptCommandType::Img;
}

SCImg::~SCImg() {}

void SCImg::execute(cocos2d::Node *stage) {
  this->reader->showImage(_name, _loc, _face);
  reader->nextScript();
}

SCImgTween::SCImgTween(ScriptReader *reader, std::string &name,
                       std::string &loc, float dur, std::string &ease,
                       std::string &face)
    : _name(name), _loc(loc), _duration(dur), _ease(ease), _face(face) {
  this->reader = reader;
  this->type = ScriptCommandType::ImgTween;
}

SCImgTween::~SCImgTween() {}

void SCImgTween::execute(cocos2d::Node *stage) {
  this->reader->imageTween(_name, _loc, _duration, _ease, _face);
  reader->nextScript();
}

SCJump::SCJump(ScriptReader *reader, std::string &sign) : sign(sign) {
  this->reader = reader;
  this->type = ScriptCommandType::Jump;
}

SCJump::~SCJump() {}

void SCJump::execute(cocos2d::Node *stage) { reader->jumpToSign(sign); }
}  // namespace yaadv

#include "CharacterManager.h"

namespace yaadv {
SCLeave::SCLeave(ScriptReader *reader, std::string &name) : cName(name) {
  this->reader = reader;
  this->type = ScriptCommandType::Leave;
}

SCLeave::~SCLeave() {}

void SCLeave::execute(cocos2d::Node *stage) {
  reader->hideCharacter(cName);
  reader->nextScript();
}

SCLeaveImg::SCLeaveImg(ScriptReader *reader, std::string &name) : cName(name) {
  this->reader = reader;
  this->type = ScriptCommandType::LeaveImg;
}

SCLeaveImg::~SCLeaveImg() {}

void SCLeaveImg::execute(cocos2d::Node *stage) {
  reader->hideImage(cName);
  reader->nextScript();
}

}  // namespace yaadv

#include "BackgroundMusicManager.h"

namespace yaadv {
SCPlayBGM::SCPlayBGM(ScriptReader *reader, std::string key) : key(key) {
  this->reader = reader;
  this->type = ScriptCommandType::PlayBGM;
}

SCPlayBGM::~SCPlayBGM() {}

void SCPlayBGM::execute(cocos2d::Node *stage) {
  reader->playBackgroundMusic(key);

  reader->nextScript();
}
}  // namespace yaadv

#include "SoundManager.h"

namespace yaadv {
SCPlaySound::SCPlaySound(ScriptReader *reader, std::string key) : key(key) {
  this->reader = reader;
  this->type = ScriptCommandType::PlaySd;
}

SCPlaySound::~SCPlaySound() {}

void SCPlaySound::execute(cocos2d::Node *stage) {
  auto sound = SM->getSound(key);
  if (sound.compare("") == 0) {
    ;
  } else {
    reader->playSound(sound);
  }

  reader->nextScript();
}

SCSelect::SCSelect(ScriptReader *reader) {
  this->reader = reader;
  this->type = ScriptCommandType::Select;
}

SCSelect::~SCSelect() {}

void SCSelect::addOption(std::string &sign, std::string &text) {
  options.insert(std::pair<std::string, std::string>(sign, text));
}

void SCSelect::setName(const char *nam) {
  this->name = std::string(nam);
  this->reader->_selects[this->name] = this;
}

void SCSelect::execute(Node *stage) {
  reader->isWaitingForSelection = true;

  reader->showSelect(options);
}
}  // namespace yaadv

#include "../GameSystem.h"

namespace yaadv {
SCSet::SCSet(ScriptReader *reader, std::string &key, char mark, int value)
    : _key(key), _mark(mark), _value(value) {
  this->reader = reader;
  this->type = ScriptCommandType::Set;
}

SCSet::~SCSet() {}

void SCSet::execute(cocos2d::Node *stage) {
  switch (_mark) {
    case '=':
      break;
    case '-':
      _value = getDataValue(_key) - _value;
      break;
    case '+':
      _value = getDataValue(_key) + _value;
      break;
    default:
      break;
  }
  setDataValue(_key, _value);

  reader->nextScript();
}

void SCSet::setDataValue(std::string key, int value) {
  GameSystem::getInstance()->setDataValue(key, value);
}

int SCSet::getDataValue(std::string key) {
  return GameSystem::getInstance()->getDataValue(key);
}
}  // namespace yaadv

#include "CharacterManager.h"

namespace yaadv {
SCShowFg::SCShowFg(ScriptReader *reader, std::string &name, std::string &face)
    : name(name), face(face) {
  this->reader = reader;
  this->type = ScriptCommandType::ShowFg;
}

SCShowFg::~SCShowFg() {}

void SCShowFg::execute(cocos2d::Node *stage) {
  Character *cha = nullptr;
  cha = CharacterManager::getInstance()->getCharacter(name);
  cha->getCharacterFace(face);
}

SCStopBGM::SCStopBGM(ScriptReader *reader) {
  this->reader = reader;
  this->type = ScriptCommandType::StopBGM;
}

SCStopBGM::~SCStopBGM() {}

void SCStopBGM::execute(cocos2d::Node *stage) {
  reader->stopBackgroundMusic();

  reader->nextScript();
}

SCStopSound::SCStopSound(ScriptReader *reader) {
  this->reader = reader;
  this->type = ScriptCommandType::StopSd;
}

SCStopSound::~SCStopSound() {}

void SCStopSound::execute(cocos2d::Node *stage) {
  reader->stopSound();

  reader->nextScript();
}

}  // namespace yaadv
