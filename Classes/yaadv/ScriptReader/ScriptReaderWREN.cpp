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
  SCCharacterSpeak sc(reader, std::string(a), std::string(b),
                                    std::string(c), std::string(d));

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);

  return;
}

extern "C" void WREN_ns_leave(WrenVM *vm) {
  const char *name_str = wrenGetSlotString(vm, 1);
  ScriptReader *reader = ScriptReader::getInstance();
  SCLeave sc(reader, std::string(name_str));

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  return;
}

extern "C" void WREN_ns_bg(WrenVM *vm) {
  const char *name_str = wrenGetSlotString(vm, 1);
  ScriptReader *reader = ScriptReader::getInstance();
  SCBackground sc(reader, std::string(name_str));

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  return;
}

extern "C" void WREN_ns_fg(WrenVM *vm) {
  const char *face_str = wrenGetSlotString(vm, 1);
  const char *name_str = wrenGetSlotString(vm, 2);
  ScriptReader *reader = ScriptReader::getInstance();
  SCShowFg sc(reader, std::string(name_str),
                            std::string(face_str));

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  return;
}

extern "C" void WREN_ns_jump(WrenVM *vm) {
  const char *name_str = wrenGetSlotString(vm, 1);
  ScriptReader *reader = ScriptReader::getInstance();
  SCJump sc(reader, std::string(name_str));

  sc.execute(reader->stage);
  wrenSetSlotNull(vm, 0);
  return;
}

extern "C" void WREN_ns_bgm(WrenVM *vm) {
  const char *name_str = wrenGetSlotString(vm, 1);
  ScriptReader *reader = ScriptReader::getInstance();
  SCPlayBGM sc(reader, std::string(name_str));

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
  SCPlaySound sc(reader, std::string(name_str));

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
  SCSet sc(reader, std::string(name_str), imark, ival);

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
  SCSelect *sc = reader->_selects[std::string(name_str)];
  if (!sc) {
    wrenSetSlotNull(vm, -1);
    return;
  }
  const char *key = wrenGetSlotString(vm, 1);
  const char *value = wrenGetSlotString(vm, 2);

  sc->addOption(std::string(key), std::string(value));

  wrenSetSlotNull(vm, 0);
  return;
}
extern "C" void WREN_ns_select_exec(WrenVM *vm) {
  ScriptReader *reader = ScriptReader::getInstance();
  const char *name_str = wrenGetSlotString(vm, 1);
  SCSelect *sc = reader->_selects[std::string(name_str)];
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
  printf("%s\n", name_str)
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
  SCLeaveImg sc(reader, std::string(name_str));

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
  SCImgTween sc(reader, std::string(name_str),
                              std::string(text_str), duration,
                              std::string(ease_str), std::string(face_str));

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
  SCImg sc(reader, std::string(a), std::string(b),
                         std::string(c));

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
    SCLeaveImg sc(reader, std::string(a));
    sc.execute(reader->stage);
  }
  if (!strcmp(cmd, "select")) {
    SCSelect *sc = new SCSelect(reader);
    sc->setName(a);
  }
  if (!strcmp(cmd, "leave")) {
    SCLeave sc(reader, std::string(a));

    sc.execute(reader->stage);
  }
  if (!strcmp(cmd, "bg")) {
    SCBackground sc(reader, std::string(a));

    sc.execute(reader->stage);
  }
  if (!strcmp(cmd, "bgm")) {
    SCPlayBGM sc(reader, std::string(a));

    sc.execute(reader->stage);
  }
  if (!strcmp(cmd, "sound")) {
    SCPlaySound sc(reader, std::string(a));

    sc.execute(reader->stage);
  }
  if (!strcmp(cmd, "selectshow")) {
    SCSelect *sc = reader->_selects[std::string(a)];
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

  ScriptReader *reader = ScriptReader::getInstance();
  if (!strcmp(cmd, "img")) {
    SCImg sc(reader, std::string(a), std::string(b),
                           std::string(c));
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

  ScriptReader *reader = ScriptReader::getInstance();

  if (!strcmp(cmd, "say_")) {
    SCCharacterSpeak sc(reader, std::string(a), std::string(b),
                                      std::string(c), std::string(d));

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

  log("LoadScript::>");

  SCSelect *currentSelect = nullptr;

  const char *src = (ss.c_str());

  WrenInterpretResult r = runSRC(src);
  if (r != WREN_RESULT_SUCCESS) abort();
  WrenVM *vm = getVM();
  this->_wrenvm = vm;
}

void ScriptReaderWREN::jumpToSign(const std::string &sign) {
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

void ScriptReaderWREN::runTick() {
  // schedulerNext();
}

void ScriptReaderWREN::nextScript() {
  if (isWaitingForSelection) {
    log("waiting");
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
    log("Sign is null");
    return;
  }
#if 0
  isWaitingForSelection = false;
  auto list = _scripts.find(sign);
  if (list == _scripts.end()) {
    log("Sign [%s] not found", sign.c_str());
    return;
  }
#endif

  _currentSignName = sign;
  _currentCommandIndex = index;
}

bool ScriptReaderWREN::getIsHaveRead() { return _isHaveRead; }

} // namespace yaadv