#pragma once

#include "ScriptCommand.h"
#include "cocos2d.h"
//#include "ScriptReader.h"

using namespace cocos2d;
using namespace std;

namespace yaadv {
class ScriptReader;

class SCBackground : public ScriptCommand {
 public:
  SCBackground(ScriptReader *reader, std::string key);
  ~SCBackground();

  std::string key;

  void execute(cocos2d::Node *stage);
};

class SCClear : public ScriptCommand {
  int _sw;

 public:
  SCClear(ScriptReader *reader, int sw);
  ~SCClear();

  void execute(cocos2d::Node *stage);
};

class SCCharacterSpeak : public ScriptCommand {
 public:
  std::string cName;
  std::string displayname;
  std::string face;
  std::string text;
  std::string cb;

  SCCharacterSpeak(ScriptReader *reader, std::string &name, std::string &displayname, std::string &text,
                   std::string &face);

  ~SCCharacterSpeak();

  void execute(cocos2d::Node *stage);
};

class SCGameOver : public ScriptCommand {
 public:
  SCGameOver(ScriptReader *reader);
  ~SCGameOver();
  void execute(cocos2d::Node *stage);
};

class SCIf : public ScriptCommand {
  std::string _expression;
  std::string _trueTag;
  std::string _falseTag;

 public:
  SCIf(ScriptReader *reader, std::string expression, std::string trueTag, std::string falseTag);
  ~SCIf();
  void execute(cocos2d::Node *stage);
  bool expressionJudge();
  void jump(std::string tag);
};

class SCImg : public ScriptCommand {
 public:
  std::string cname;
  std::string _name, _loc, _face;
  SCImg(ScriptReader *reader, std::string &name, std::string &loc, std::string &face);
  ~SCImg();
  void execute(cocos2d::Node *stage);
};

class SCImgTween : public ScriptCommand {
 public:
  std::string cname;
  std::string _name, _loc, _face, _ease;
  float _duration;
  SCImgTween(ScriptReader *reader, std::string &name, std::string &loc, float dur, std::string &ease,
             std::string &face);
  ~SCImgTween();
  void execute(cocos2d::Node *stage);
};

class SCJump : public ScriptCommand {
 public:
  std::string sign;
  SCJump(ScriptReader *reader, std::string &sign);
  ~SCJump();
  void execute(cocos2d::Node *stage);
};

class SCLeave : public ScriptCommand {
 public:
  std::string cName;
  SCLeave(ScriptReader *reader, std::string &name);
  ~SCLeave();
  void execute(cocos2d::Node *stage);
};

class SCLeaveImg : public ScriptCommand {
 public:
  std::string cName;
  SCLeaveImg(ScriptReader *reader, std::string &name);
  ~SCLeaveImg();
  void execute(cocos2d::Node *stage);
};

class SCPlayBGM : public ScriptCommand {
 public:
  SCPlayBGM(ScriptReader *reader, std::string key);
  ~SCPlayBGM();

  std::string key;

  void execute(cocos2d::Node *stage);
};

class SCPlaySound : public ScriptCommand {
 public:
  SCPlaySound(ScriptReader *reader, std::string key);
  ~SCPlaySound();

  std::string key;

  void execute(cocos2d::Node *stage);
};

class SCSelect : public ScriptCommand {
 public:
  SCSelect(ScriptReader *reader);
  ~SCSelect();

  std::string name;
  void setName(const char *);
  std::map<std::string, std::string> options;

  void addOption(std::string &sign, std::string &text);

  void execute(cocos2d::Node *stage);
};

class SCSet : public ScriptCommand {
  char _mark;
  std::string _key;
  int _value;

 public:
  SCSet(ScriptReader *reader, std::string &key, char mark, int value);
  ~SCSet();
  void execute(cocos2d::Node *stage);

  void setDataValue(std::string key, int value);
  int getDataValue(std::string key);
};

class SCStopBGM : public ScriptCommand {
 public:
  SCStopBGM(ScriptReader *reader);
  ~SCStopBGM();

  void execute(cocos2d::Node *stage);
};

class SCShowFg : public ScriptCommand {
 public:
  std::string name;
  std::string face;

  SCShowFg(ScriptReader *reader, std::string &name, std::string &face);

  ~SCShowFg();

  void execute(cocos2d::Node *stage);
};

class SCStopSound : public ScriptCommand {
 public:
  SCStopSound(ScriptReader *reader);
  ~SCStopSound();

  void execute(cocos2d::Node *stage);
};
}  // yaadv
