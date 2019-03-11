#include "ScriptReader.h"

#include "../GameSystem.h"
#include "../MainMenuScene.h"
#include "CharacterManager.h"
#include "SCX.h"

USING_NS_CC;
using namespace yaadv;
namespace yaadv {

ScriptReaderYASL *ScriptReaderYASL::_instance = nullptr;

ScriptReaderYASL::ScriptReaderYASL() {
  this->_yaslstate = 0;
  this->_currentAction = 0;
}

ScriptReaderYASL::~ScriptReaderYASL() { clearScript(); }

void ScriptReaderYASL::initWithStage(Node *stage) {
  this->stage = stage;
  charStage = Layer::create();
  stage->addChild(charStage);
  for (int i = 0; i < 5; i++) {
    chars[i] = nullptr;
  }
  _currentSignName = "gameStart";
  _currentCommandIndex = 0;
}

ScriptReader *ScriptReaderYASL::getInstance() {
  if (_instance == nullptr) {
    _instance = new ScriptReaderYASL();
  }
  return _instance;
}

void ScriptReaderYASL::destoryInstance() { CC_SAFE_DELETE(_instance); }

void ScriptReaderYASL::clearScript() {
#if 0
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
#endif

  isWaitingForSelection = false;
}

int YASL_ns_say(struct YASL_State *st) {
  struct YASL_Object *face = YASL_popobject(st);
  char *face_str;
  if (YASL_isundef(face) == YASL_SUCCESS) {
    face_str = "";
  } else if (YASL_isstring(face) == YASL_SUCCESS) {
    face_str = YASL_getcstring(face);
  } else {
    return -1;
  }

  struct YASL_Object *text = YASL_popobject(st);
  if (YASL_isstring(text) != YASL_SUCCESS) {
    return -1;
  }

  char *text_str = YASL_getcstring(text);

  struct YASL_Object *dname = YASL_popobject(st);
  if (YASL_isstring(dname) != YASL_SUCCESS) {
    return -1;
  }

  char *dname_str = YASL_getcstring(dname);

  struct YASL_Object *name = YASL_popobject(st);
  if (YASL_isstring(name) != YASL_SUCCESS) {
    return -1;
  }

  char *name_str = YASL_getcstring(name);

  ScriptReader *reader = ScriptReader::getInstance();
  SCCharacterSpeak sc(reader, std::string(name_str), std::string(dname_str), std::string(text_str),
                      std::string(face_str));

  sc.execute(reader->stage);
  YASL_pushobject(st, YASL_Undef());
  YASL_suspend(st);
  return 0;
}

int YASL_ns_leave(struct YASL_State *st) {
  struct YASL_Object *name = YASL_popobject(st);

  char *name_str = YASL_getcstring(name);
  ScriptReader *reader = ScriptReader::getInstance();
  SCLeave sc(reader, std::string(name_str));

  sc.execute(reader->stage);
  YASL_pushobject(st, YASL_Undef());
  return 0;
}

int YASL_ns_bg(struct YASL_State *st) {
  struct YASL_Object *name = YASL_popobject(st);

  char *name_str = YASL_getcstring(name);
  ScriptReader *reader = ScriptReader::getInstance();
  SCBackground sc(reader, std::string(name_str));

  sc.execute(reader->stage);
  YASL_pushobject(st, YASL_Undef());
  return 0;
}

int YASL_ns_fg(struct YASL_State *st) {
  struct YASL_Object *face = YASL_popobject(st);
  char *face_str = YASL_getcstring(face);

  struct YASL_Object *name = YASL_popobject(st);
  char *name_str = YASL_getcstring(name);
  ScriptReader *reader = ScriptReader::getInstance();
  SCShowFg sc(reader, std::string(name_str), std::string(face_str));

  sc.execute(reader->stage);
  YASL_pushobject(st, YASL_Undef());
  return 0;
}

int YASL_ns_jump(struct YASL_State *st) {
  struct YASL_Object *name = YASL_popobject(st);

  char *name_str = YASL_getcstring(name);
  ScriptReader *reader = ScriptReader::getInstance();
  SCJump sc(reader, std::string(name_str));

  sc.execute(reader->stage);
  YASL_pushobject(st, YASL_Undef());
  return 0;
}

int YASL_ns_bgm(struct YASL_State *st) {
  struct YASL_Object *name = YASL_popobject(st);

  char *name_str = YASL_getcstring(name);
  ScriptReader *reader = ScriptReader::getInstance();
  SCPlayBGM sc(reader, std::string(name_str));

  sc.execute(reader->stage);
  YASL_pushobject(st, YASL_Undef());
  return 0;
}
int YASL_ns_stopbgm(struct YASL_State *st) {
  ScriptReader *reader = ScriptReader::getInstance();
  SCStopBGM sc(reader);

  sc.execute(reader->stage);
  YASL_pushobject(st, YASL_Undef());
  return 0;
}
int YASL_ns_sound(struct YASL_State *st) {
  struct YASL_Object *name = YASL_popobject(st);

  char *name_str = YASL_getcstring(name);
  ScriptReader *reader = ScriptReader::getInstance();
  SCPlaySound sc(reader, std::string(name_str));

  sc.execute(reader->stage);
  YASL_pushobject(st, YASL_Undef());
  return 0;
}
int YASL_ns_stopsound(struct YASL_State *st) {
  ScriptReader *reader = ScriptReader::getInstance();
  SCStopSound sc(reader);

  sc.execute(reader->stage);
  YASL_pushobject(st, YASL_Undef());
  return 0;
}
int YASL_ns_gameover(struct YASL_State *st) {
  ScriptReader *reader = ScriptReader::getInstance();
  SCGameOver sc(reader);

  sc.execute(reader->stage);
  YASL_pushobject(st, YASL_Undef());
  return 0;
}
int YASL_ns_set(struct YASL_State *st) {
  struct YASL_Object *val = YASL_popobject(st);
  int ival;
  int imark;
  if (YASL_isinteger(val) == YASL_SUCCESS) {
    ival = YASL_getinteger(val);
  } else {
    return -1;
  }
  struct YASL_Object *mark = YASL_popobject(st);
  if (YASL_isinteger(mark) != YASL_SUCCESS) {
    return -1;
  }
  imark = YASL_getinteger(mark);
  struct YASL_Object *name = YASL_popobject(st);
  if (YASL_isstring(name) != YASL_SUCCESS) {
    return -1;
  }
  char *name_str = YASL_getcstring(name);
  ScriptReader *reader = ScriptReader::getInstance();
  SCSet sc(reader, std::string(name_str), imark, ival);

  sc.execute(reader->stage);
  YASL_pushobject(st, YASL_Undef());
  return 0;
}
int YASL_ns_select(struct YASL_State *st) {
  struct YASL_Object *options = YASL_popobject(st);
  if (YASL_istable(options) != YASL_SUCCESS) {
    return -1;
  }

  ScriptReader *reader = ScriptReader::getInstance();
  SCSelect sc(reader);

  char *key = "abc";
  char *value = "def";
  sc.addOption(std::string(key), std::string(value));

  sc.execute(reader->stage);
  YASL_pushobject(st, YASL_Undef());
  return 0;
}
int YASL_ns_print(struct YASL_State *st) {
  struct YASL_Object *name = YASL_popobject(st);

  char *name_str = YASL_getcstring(name);
#ifdef _MSC_VER
  OutputDebugStringA(name_str);
#else
  printf("%s\n", name_str)
#endif

  YASL_pushobject(st, YASL_Undef());
  return 0;
}
int YASL_ns_clear(struct YASL_State *st) {
  ScriptReader *reader = ScriptReader::getInstance();

  struct YASL_Object *name = YASL_popobject(st);

  int sw = YASL_getinteger(name);

  SCClear sc(reader, sw);

  sc.execute(reader->stage);
  YASL_pushobject(st, YASL_Undef());
  return 0;
}

int YASL_ns_leaveimg(struct YASL_State *st) {
  struct YASL_Object *name = YASL_popobject(st);
  if (YASL_isstring(name) != YASL_SUCCESS) {
    return -1;
  }
  char *name_str = YASL_getcstring(name);
  ScriptReader *reader = ScriptReader::getInstance();
  SCLeaveImg sc(reader, std::string(name_str));

  sc.execute(reader->stage);
  YASL_pushobject(st, YASL_Undef());
  // YASL_suspend(st);
  return 0;
}

int YASL_ns_imgtween(struct YASL_State *st) {
  struct YASL_Object *face = YASL_popobject(st);
  char *face_str;
  if (YASL_isundef(face) == YASL_SUCCESS) {
    face_str = "";
  } else if (YASL_isstring(face) == YASL_SUCCESS) {
    face_str = YASL_getcstring(face);
  } else {
    return -1;
  }

  struct YASL_Object *ease = YASL_popobject(st);
  if (YASL_isstring(ease) != YASL_SUCCESS) {
    return -1;
  }
  char *ease_str = YASL_getcstring(face);

  struct YASL_Object *durtext = YASL_popobject(st);
  if (YASL_isdouble(durtext) != YASL_SUCCESS) {
    return -1;
  }
  float duration = YASL_getdouble(durtext);

  struct YASL_Object *text = YASL_popobject(st);
  if (YASL_isstring(text) != YASL_SUCCESS) {
    return -1;
  }
  char *text_str = YASL_getcstring(text);
  struct YASL_Object *name = YASL_popobject(st);
  if (YASL_isstring(name) != YASL_SUCCESS) {
    return -1;
  }
  char *name_str = YASL_getcstring(name);
  ScriptReader *reader = ScriptReader::getInstance();
  SCImgTween sc(reader, std::string(name_str), std::string(text_str), duration, std::string(ease_str),
                std::string(face_str));

  sc.execute(reader->stage);
  YASL_pushobject(st, YASL_Undef());
  // YASL_suspend(st);
  return 0;
}

int YASL_ns_img(struct YASL_State *st) {
  struct YASL_Object *face = YASL_popobject(st);
  char *face_str;
  if (YASL_isundef(face) == YASL_SUCCESS) {
    face_str = "";
  } else if (YASL_isstring(face) == YASL_SUCCESS) {
    face_str = YASL_getcstring(face);
  } else {
    return -1;
  }
  struct YASL_Object *text = YASL_popobject(st);
  if (YASL_isstring(text) != YASL_SUCCESS) {
    return -1;
  }
  char *text_str = YASL_getcstring(text);
  struct YASL_Object *name = YASL_popobject(st);
  if (YASL_isstring(name) != YASL_SUCCESS) {
    return -1;
  }
  char *name_str = YASL_getcstring(name);
  ScriptReader *reader = ScriptReader::getInstance();
  SCImg sc(reader, std::string(name_str), std::string(text_str), std::string(face_str));

  sc.execute(reader->stage);
  YASL_pushobject(st, YASL_Undef());
  // YASL_suspend(st);
  return 0;
}

static int YASL_load_ns(struct YASL_State *state) {
  struct YASL_Object *ns = YASL_Table();

  struct YASL_Object *method1_str = YASL_CString("say");
  struct YASL_Object *method1_fn = YASL_CFunction(YASL_ns_say, 3);
  YASL_Table_set(ns, method1_str, method1_fn);

  struct YASL_Object *method2_str = YASL_CString("leave");
  struct YASL_Object *method2_fn = YASL_CFunction(YASL_ns_leave, 1);
  YASL_Table_set(ns, method2_str, method2_fn);

  struct YASL_Object *method3_str = YASL_CString("bg");
  struct YASL_Object *method3_fn = YASL_CFunction(YASL_ns_bg, 1);
  YASL_Table_set(ns, method3_str, method3_fn);

  struct YASL_Object *method4_str = YASL_CString("fg");
  struct YASL_Object *method4_fn = YASL_CFunction(YASL_ns_fg, 2);
  YASL_Table_set(ns, method4_str, method4_fn);

  struct YASL_Object *method5_str = YASL_CString("bgm");
  struct YASL_Object *method5_fn = YASL_CFunction(YASL_ns_bgm, 1);
  YASL_Table_set(ns, method5_str, method5_fn);

  struct YASL_Object *method6_str = YASL_CString("stopbgm");
  struct YASL_Object *method6_fn = YASL_CFunction(YASL_ns_stopbgm, 0);
  YASL_Table_set(ns, method6_str, method6_fn);

  struct YASL_Object *method7_str = YASL_CString("jump");
  struct YASL_Object *method7_fn = YASL_CFunction(YASL_ns_jump, 0);
  YASL_Table_set(ns, method7_str, method7_fn);

  struct YASL_Object *method8_str = YASL_CString("sound");
  struct YASL_Object *method8_fn = YASL_CFunction(YASL_ns_sound, 1);
  YASL_Table_set(ns, method8_str, method8_fn);

  struct YASL_Object *method9_str = YASL_CString("stopsound");
  struct YASL_Object *method9_fn = YASL_CFunction(YASL_ns_stopsound, 0);
  YASL_Table_set(ns, method9_str, method9_fn);

  struct YASL_Object *method10_str = YASL_CString("print");
  struct YASL_Object *method10_fn = YASL_CFunction(YASL_ns_print, 1);
  YASL_Table_set(ns, method10_str, method10_fn);

  struct YASL_Object *method11_str = YASL_CString("clear");
  struct YASL_Object *method11_fn = YASL_CFunction(YASL_ns_clear, 1);
  YASL_Table_set(ns, method11_str, method11_fn);

  struct YASL_Object *method12_str = YASL_CString("img");
  struct YASL_Object *method12_fn = YASL_CFunction(YASL_ns_img, 3);
  YASL_Table_set(ns, method12_str, method12_fn);

  struct YASL_Object *method13_str = YASL_CString("imgtween");
  struct YASL_Object *method13_fn = YASL_CFunction(YASL_ns_imgtween, 5);
  YASL_Table_set(ns, method13_str, method13_fn);

  struct YASL_Object *method14_str = YASL_CString("leaveimg");
  struct YASL_Object *method14_fn = YASL_CFunction(YASL_ns_leaveimg, 1);
  YASL_Table_set(ns, method14_str, method12_fn);
  // struct YASL_Object *read_str = YASL_CString("read");
  // struct YASL_Object *read_fn = YASL_CFunction(YASL_io_read, 2);

  // YASL_Table_set(ns, read_str, read_fn);

  YASL_declglobal(state, "ns");
  YASL_pushobject(state, ns);
  YASL_setglobal(state, "ns");

  free(method1_str);
  free(method1_fn);
  free(method2_str);
  free(method2_fn);

  free(method3_str);
  free(method3_fn);
  free(method4_str);
  free(method4_fn);

  free(method5_str);
  free(method5_fn);
  free(method6_str);
  free(method6_fn);

  free(method7_str);
  free(method7_fn);
  free(method8_str);
  free(method8_fn);
  free(method9_str);
  free(method9_fn);
  free(method10_str);
  free(method10_fn);
  free(method11_str);
  free(method11_fn);
  free(method12_str);
  free(method12_fn);
  free(method13_str);
  free(method13_fn);
  free(method14_str);
  free(method14_fn);
  //  free(read_str);
  //  free(read_fn);

  return (int)YASL_SUCCESS;
}

void ScriptReaderYASL::loadScriptFile(std::string path) {
  clearScript();

  std::string ss = FileUtils::getInstance()->getStringFromFile(path);

  int sPos = 0;
  int ePos = 0;

  std::string currentSign = "default";

  SCSelect *currentSelect = nullptr;

  if (this->_yaslstate != 0) abort();
  const char *src = (ss.c_str());
  this->_yaslstate = YASL_newstate_bb((char *)src, strlen(src));
  // YASL_load_io(this->_yaslstate);
  YASL_load_ns(this->_yaslstate);
  YASL_load(this->_yaslstate);
}

void ScriptReaderYASL::jumpToSign(const std::string &sign) {
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

void ScriptReaderYASL::runTick() {
  int count = 0;
  int ret = 0;
  if (YASL_get_vmstatus(this->_yaslstate) != 0) return;
  while (count < 300) {
    ret = YASL_runbc(this->_yaslstate);
    if (YASL_get_vmstatus(this->_yaslstate) != 0) break;
    if (ret != YASL_EXEC_CONTINUE) {
      break;
    }
    count++;
  }
}

void ScriptReaderYASL::nextScript() {
  if (isWaitingForSelection) {
    return;
  }
  if (this->_currentAction) stage->stopAction(this->_currentAction);
  YASL_resume(this->_yaslstate);
  auto act = CallFunc::create([this]() { this->runTick(); });
  auto tick = Sequence::create(act, DelayTime::create(0.5), nullptr);
  auto repeat = Repeat::create(tick, -1);
  auto seq = Sequence::create(repeat, nullptr);
  this->_currentAction = seq;
  stage->runAction(seq);
}

std::string ScriptReaderYASL::getCurrentSignName() { return _currentSignName; }

int ScriptReaderYASL::getCurrentCommandIndex() { return _currentCommandIndex; }

void ScriptReaderYASL::setCurrentCommandIndex(int value) { _currentCommandIndex = value; }

void ScriptReaderYASL::jumpToSign(const std::string &sign, int index) {
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

bool ScriptReaderYASL::getIsHaveRead() { return _isHaveRead; }
}  // namespace yaadv
