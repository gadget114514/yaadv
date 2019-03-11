#include "ScriptReader.h"
#include "../GameSystem.h"
#include "../MainMenuScene.h"
#include "CharacterManager.h"
#include "SCX.h"
#include "ScriptReader.h"

#include "../wren/src/mini/io.h"
#include "../wren/src/mini/modules.h"
#include "../wren/src/mini/ns.h"
#include "../wren/src/mini/path.h"
#include "../wren/src/mini/scheduler.h"
#include "../wren/src/mini/stat.h"
#include "../wren/src/mini/vm.h"

USING_NS_CC;

using namespace yaadv;
namespace yaadv {

ScriptReaderWREN *ScriptReaderWREN::_instance = nullptr;

ScriptReaderWREN::ScriptReaderWREN() {
  this->_wrenvm = 0;
  this->_currentAction = 0;
}

ScriptReaderWREN::~ScriptReaderWREN() { clearScript(); }

void ScriptReaderWREN::initWithStage(Node *stage) {
  this->stage = stage;
  charStage = Layer::create();
  stage->addChild(charStage);
  for (int i = 0; i < 5; i++) {
    chars[i] = nullptr;
  }
  _currentSignName = "gameStart";
  _currentCommandIndex = 0;
}

ScriptReader *ScriptReaderWREN::getInstance() {
  if (_instance == nullptr) {
    _instance = new ScriptReaderWREN();
  }
  return _instance;
}

void ScriptReaderWREN::destoryInstance() { CC_SAFE_DELETE(_instance); }

void ScriptReaderWREN::clearScript() { isWaitingForSelection = false; }

extern "C" void WREN_ns_say(WrenVM *vm) {
  const char *a = wrenGetSlotString(vm, 1);
  const char *b = wrenGetSlotString(vm, 2);
  const char *c = wrenGetSlotString(vm, 3);
  const char *d = wrenGetSlotString(vm, 4);
  //    const char* d = wrenGetSlotString(vm, 4);

  ScriptReader *reader = ScriptReader::getInstance();
  std::string _1(a);
  std::string _2(b);
  std::string _3(c);
  std::string _4(d);
  SCCharacterSpeak sc(reader, _1,_2,_3,_4);
  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);

  return;
}

extern "C" void WREN_ns_leave(WrenVM *vm) {
  const char *name_str = wrenGetSlotString(vm, 1);
  ScriptReader *reader = ScriptReader::getInstance();
  std::string _1(name_str);
  SCLeave sc(reader, _1);

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  return;
}

extern "C" void WREN_ns_bg(WrenVM *vm) {
  const char *name_str = wrenGetSlotString(vm, 1);
  ScriptReader *reader = ScriptReader::getInstance();
  std::string _1(name_str);
  SCBackground sc(reader, _1);
  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  return;
}

extern "C" void WREN_ns_fg(WrenVM *vm) {
  const char *face_str = wrenGetSlotString(vm, 1);
  const char *name_str = wrenGetSlotString(vm, 2);
  ScriptReader *reader = ScriptReader::getInstance();
  std::string _1(name_str);
  std::string _2(face_str)  ;
  SCShowFg sc(reader, _1,_2);

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  return;
}

extern "C" void WREN_ns_jump(WrenVM *vm) {
  const char *name_str = wrenGetSlotString(vm, 1);
  ScriptReader *reader = ScriptReader::getInstance();
  std::string _1(name_str);
  SCJump sc(reader, _1);

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  return;
}

extern "C" void WREN_ns_bgm(WrenVM *vm) {
  const char *name_str = wrenGetSlotString(vm, 1);
  ScriptReader *reader = ScriptReader::getInstance();
  std::string _1(name_str);
  SCPlayBGM sc(reader, _1);

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  return;
}
extern "C" void WREN_ns_stopbgm(WrenVM *vm) {
  ScriptReader *reader = ScriptReader::getInstance();
  SCStopBGM sc(reader);

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  return;
}
extern "C" void WREN_ns_sound(WrenVM *vm) {
  const char *name_str = wrenGetSlotString(vm, 1);
  ScriptReader *reader = ScriptReader::getInstance();
  std::string _1(name_str);
  SCPlaySound sc(reader, _1);

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  return;
}
extern "C" void WREN_ns_stopsound(WrenVM *vm) {
  ScriptReader *reader = ScriptReader::getInstance();
  SCStopSound sc(reader);

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  return;
}
extern "C" void WREN_ns_gameover(WrenVM *vm) {
  ScriptReader *reader = ScriptReader::getInstance();
  SCGameOver sc(reader);

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  return;
}
extern "C" void WREN_ns_set(WrenVM *vm) {
  int ival;
  int imark;
  const char *name_str = wrenGetSlotString(vm, 1);
  imark = wrenGetSlotDouble(vm, 2);
  ival = wrenGetSlotDouble(vm, 3);

  ScriptReader *reader = ScriptReader::getInstance();
  std::string _1(name_str);
  SCSet sc(reader, _1, imark, ival);

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  return;
}
extern "C" void WREN_ns_select(WrenVM *vm) {
  ScriptReader *reader = ScriptReader::getInstance();
  SCSelect *sc = new SCSelect(reader);

  const char *name_str = wrenGetSlotString(vm, 1);
  sc->setName(name_str);

  wrenSetSlotNull(vm, 0);
  return;
}
extern "C" void WREN_ns_select_add_option(WrenVM *vm) {
  ScriptReader *reader = ScriptReader::getInstance();
  const char *name_str = wrenGetSlotString(vm, 1);
  std::string i(name_str);
  SCSelect *sc = reader->_selects[i];
  if (!sc) {
    wrenSetSlotNull(vm, -1);
    return;
  }
  const char *key = wrenGetSlotString(vm, 1);
  const char *value = wrenGetSlotString(vm, 2);
  std::string a(key);
  std::string b(value);
  sc->addOption(a,b);

  wrenSetSlotNull(vm, 0);
  return;
}
extern "C" void WREN_ns_select_exec(WrenVM *vm) {
  ScriptReader *reader = ScriptReader::getInstance();
  const char *name_str = wrenGetSlotString(vm, 1);
  std::string i(name_str);
  SCSelect *sc = reader->_selects[i];
  if (!sc) {
    wrenSetSlotNull(vm, -1);
    return;
  }
  sc->execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  return;
}

extern "C" void WREN_ns_print(WrenVM *vm) {
  const char *name_str = wrenGetSlotString(vm, 1);
#ifdef _MSC_VER
  OutputDebugStringA(name_str);
#else
  printf("%s\n", name_str);
#endif

  wrenSetSlotNull(vm, 0);
  return;
}
extern "C" void WREN_ns_clear(WrenVM *vm) {
  ScriptReader *reader = ScriptReader::getInstance();

  int sw = wrenGetSlotDouble(vm, 1);

  SCClear sc(reader, sw);

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  return;
}

extern "C" void WREN_ns_leaveimg(WrenVM *vm) {
  const char *name_str = wrenGetSlotString(vm, 1);
  ScriptReader *reader = ScriptReader::getInstance();
  std::string s(name_str);
  SCLeaveImg sc(reader, s);

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  // WREN_suspend(st);
  return;
}

extern "C" void WREN_ns_imgtween(WrenVM *vm) {
  const char *face_str;
  face_str = wrenGetSlotString(vm, 1);

  const char *ease_str = wrenGetSlotString(vm, 2);
  float duration = wrenGetSlotDouble(vm, 3);

  const char *text_str = wrenGetSlotString(vm, 3);

  const char *name_str = wrenGetSlotString(vm, 4);
  ScriptReader *reader = ScriptReader::getInstance();
  std::string a(name_str);
  std::string b(text_str);
  std::string c(ease_str);
  std::string d(face_str);
  SCImgTween sc(reader, a,
		b, duration,
		c, d);

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  // WREN_suspend(st);
  return;
}

extern "C" void WREN_ns_img(WrenVM *vm) {
  const char *a = wrenGetSlotString(vm, 1);
  const char *b = wrenGetSlotString(vm, 2);
  const char *c = wrenGetSlotString(vm, 3);

  ScriptReader *reader = ScriptReader::getInstance();
  std::string _1(a);
  std::string _2(b);
  std::string _3(c);
  SCImg sc(reader,_1,_2,_3);

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  // WREN_suspend(st);
  return;
}

extern "C" void WREN_ns__s(WrenVM *vm) {
  const char *cmd = wrenGetSlotString(vm, 1);
  const char *a = wrenGetSlotString(vm, 2);

  ScriptReader *reader = ScriptReader::getInstance();

  if (!strcmp(cmd, "leaveimg")) {
    std::string s(a);
    SCLeaveImg sc(reader, s);
    sc.execute(reader->stage);
  }
  if (!strcmp(cmd, "select")) {
    SCSelect *sc = new SCSelect(reader);
    sc->setName(a);
  }
  if (!strcmp(cmd, "leave")) {
    std::string s(a);
    SCLeave sc(reader, s);

    sc.execute(reader->stage);
  }
  if (!strcmp(cmd, "bg")) {
        std::string s(a);
	SCBackground sc(reader,s);

    sc.execute(reader->stage);
  }
  if (!strcmp(cmd, "bgm")) {
            std::string s(a);
	    SCPlayBGM sc(reader, s);

    sc.execute(reader->stage);
  }
  if (!strcmp(cmd, "sound")) {
            std::string s(a);    
	    SCPlaySound sc(reader, s);

    sc.execute(reader->stage);
  }
  if (!strcmp(cmd, "selectshow")) {
std::string s(a);    
    SCSelect *sc = reader->_selects[s];
    if (!sc) {
      wrenSetSlotNull(vm, -1);
      return;
    }
    sc->execute(reader->stage);
  }
wrenSetSlotNull(vm, 0);
}
extern "C" void WREN_ns__ss(WrenVM *vm) {
  const char *cmd = wrenGetSlotString(vm, 1);
  const char *a = wrenGetSlotString(vm, 2);
  const char *b = wrenGetSlotString(vm, 3);

  ScriptReader *reader = ScriptReader::getInstance();
  if (!strcmp(cmd, "")) {
  }

  wrenSetSlotNull(vm, 0);
}
extern "C" void WREN_ns__sss(WrenVM *vm) {
  const char *cmd = wrenGetSlotString(vm, 1);
  const char *a = wrenGetSlotString(vm, 2);
  const char *b = wrenGetSlotString(vm, 3);
  const char *c = wrenGetSlotString(vm, 4);
  std::string _1(a);
  std::string _2(b);
  std::string _3(c);

  ScriptReader *reader = ScriptReader::getInstance();
  if (!strcmp(cmd, "img")) {
    SCImg sc(reader, _1,_2,_3);
    sc.execute(reader->stage);
  }

  wrenSetSlotNull(vm, 0);
}
extern "C" void WREN_ns__ssss(WrenVM *vm) {
  const char *cmd = wrenGetSlotString(vm, 1);
  const char *a = wrenGetSlotString(vm, 2);
  const char *b = wrenGetSlotString(vm, 3);
  const char *c = wrenGetSlotString(vm, 4);
  const char *d = wrenGetSlotString(vm, 5);
  std::string _1(a);
  std::string _2(b);
  std::string _3(c);
  std::string _4(d);  

  ScriptReader *reader = ScriptReader::getInstance();

  if (!strcmp(cmd, "say_")) {
    SCCharacterSpeak sc(reader, _1,_2,_3,_4);

    sc.execute(reader->stage);
  }
  wrenSetSlotNull(vm, 0);
}
extern "C" void WREN_ns__v(WrenVM *vm) {
  const char *cmd = wrenGetSlotString(vm, 1);

  ScriptReader *reader = ScriptReader::getInstance();

  if (!strcmp(cmd, "stopsound")) {
    SCStopSound sc(reader);

    sc.execute(reader->stage);
  }
  if (!strcmp(cmd, "stopbgm")) {
    SCStopBGM sc(reader);

    sc.execute(reader->stage);
  }

  wrenSetSlotNull(vm, 0);
}

void ScriptReaderWREN::loadScriptFile(std::string path) {
  clearScript();

  std::string ss = FileUtils::getInstance()->getStringFromFile(path);

  int sPos = 0;
  int ePos = 0;

  std::string currentSign = "default";

  SCSelect *currentSelect = nullptr;

  const char *src = (ss.c_str());

  WrenInterpretResult r = runSRC(src);
  if (r != WREN_RESULT_SUCCESS) abort();
  WrenVM *vm = getVM();
  this->_wrenvm = vm;
}

void ScriptReaderWREN::jumpToSign(const std::string &sign) {
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

void ScriptReaderWREN::runTick() {
  // schedulerNext();
}

void ScriptReaderWREN::nextScript() {
  if (isWaitingForSelection) {
    return;
  }
  if (this->_currentAction) stage->stopAction(this->_currentAction);

  auto resume = CallFunc::create([this]() {
    //  	 WrenInterpretResult r = wrenInterpretExpr(this->_wrenvm, ".",
    //  "Scheduler.add(CC)");
    //    if (r != WREN_RESULT_SUCCESS) abort();
    WREN_ns_resume();
  });

  auto act = CallFunc::create([this]() { this->runTick(); });
  auto tick = Sequence::create(act, DelayTime::create(0.5), nullptr);
  auto repeat = Repeat::create(tick, -1);
  auto seq = Sequence::create(resume, repeat, nullptr);
  this->_currentAction = seq;
  stage->runAction(seq);
}

std::string ScriptReaderWREN::getCurrentSignName() { return _currentSignName; }

int ScriptReaderWREN::getCurrentCommandIndex() { return _currentCommandIndex; }

void ScriptReaderWREN::setCurrentCommandIndex(int value) {
  _currentCommandIndex = value;
}

void ScriptReaderWREN::jumpToSign(const std::string &sign, int index) {
  if (sign.compare("") == 0) {
    return;
  }
#if 0
  isWaitingForSelection = false;
  auto list = _scripts.find(sign);
  if (list == _scripts.end()) {
    return;
  }
#endif

  _currentSignName = sign;
  _currentCommandIndex = index;
}

bool ScriptReaderWREN::getIsHaveRead() { return _isHaveRead; }

} // namespace yaadv
