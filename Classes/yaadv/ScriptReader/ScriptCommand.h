#pragma once
#include "cocos2d.h"



namespace yaadv {
enum ScriptCommandType;
class ScriptReader;
enum ScriptCommandType {
  PartSign,
  CharacterSpeak,
  Select,
  SelectOption,
  SelectShow,
  Leave,
  Set,
  GameOver,
  Jump,
  Background,
  PlayBGM,
  StopBGM,
  PlaySd,
  StopSd,
  If,
  ShowFg,
  Clear,
  Img,
  ImgTween,
  LeaveImg
};
class ScriptCommand {
 public:
  ScriptCommandType type;
  ScriptReader *reader;
  virtual void execute(cocos2d::Node *stage);
};

class ScriptCommand;

class SCCharacterSpeak;

class SCLeave;

class SCJump;

class SCSelect;

class SCBackground;

class SCPlayBGM;

class SCStopBGM;

class SCPlaySound;

class SCStopSound;

class SCSet;

class SCIf;

class SCShowFg;

class SCGameOver;

class SCClear;

class SCImg;
class SCImgTween;
class SCLeaveImg;

}  // namespace yaadv

#include "SCX.h"
