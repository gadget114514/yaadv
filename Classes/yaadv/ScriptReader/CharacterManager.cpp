#include "CharacterManager.h"

USING_NS_CC;
using namespace yaadv;

#define USEJSON 1
#define USEINI 0

#if USEJSON
#include "json.hpp"
#define characterPath "yaadv/character/characterList.json"
#define YaImgPath "yaadv/img/imgList.json"
#elif USEINI
#define INI_HANDLER_LINENO 1
#include "ini.h"
#define characterPath "yaadv/character/characterList.ini"
#define YaImgPath "yaadv/img/imgList.ini"
#else
#define characterPath "yaadv/character/characterList.txt"
#deinfe YaImgPath "yaadv/img/imgList.txt"
#endif

namespace yaadv {
CharacterManager *CharacterManager::_instance = nullptr;

static int char_handler(void *user, const char *section, const char *name, const char *value, int lineno)

{
  CharacterManager *cm = (CharacterManager *)user;

  if (!strcmp(name, "name")) {
    auto character = new Character();
    std::string key(section);
    cm->addCharacter(key, character);
    character->name = value;
  } else {
    std::string key(section);
    Character *character = cm->getCharacter(key);

    if (!strcmp(name, "face")) {
      std::string temp(value);
      std::string face;
      std::string facePath;
      int tempPos = temp.find_first_of(',', 0);
      face = temp.substr(0, tempPos);
      facePath = temp.substr(tempPos + 1, temp.length() - tempPos - 1);
      facePath = "yaadv/character/" + facePath + ".png";
      char *cfacePath = new char[128];
      facePath.copy(cfacePath, facePath.length(), 0);
      *(cfacePath + facePath.length()) = '\0';
      character->fgList->insert(std::pair<std::string, char *>(face, cfacePath));
      float anchorx = 0.5;
      float anchory = 0.0;
      float scalex = 1.0;
      float scaley = 1.0;
      Bamiri *b = new Bamiri(0, 0, anchorx, anchory, scalex, scaley, 0, 0);
      character->bamiriList->insert(std::pair<std::string, Bamiri *>(face, b));
    }
  }
  return 0;
}

CharacterManager::CharacterManager()
    : _pool(nullptr)

{
  _pool = new std::map<std::string, Character *>();

  std::string ss = FileUtils::getInstance()->getStringFromFile(characterPath);

#if USEJSON
  // https://github.com/nbsdx/SimpleJSON
  CharacterManager *cm = (CharacterManager *)this;
  json::JSON data = json::JSON::Load(ss);
  for (int i = 0; i < data.length(); i++) {
    // { characterid: "", name: "", face: "", anchorx: "", ...}
    Character *character;
    json::JSON line = data[i];
    if (line.hasKey("name")) {
      character = new Character();
      std::string section = line["characterid"].ToString();
      std::string key(section);
      cm->addCharacter(key, character);
      std::string value = line["name"].ToString();
      character->name = value;

    } else {
      std::string section = line["characterid"].ToString();
      std::string key(section);
      character = cm->getCharacter(key);
    }

    std::string face = line["face"].ToString();
    std::string filename = line["filename"].ToString();

    std::string facePath = "yaadv/character/" + filename + ".png";
    char *cfacePath = new char[128];
    facePath.copy(cfacePath, facePath.length(), 0);
    *(cfacePath + facePath.length()) = '\0';
    character->fgList->insert(std::pair<std::string, char *>(face, cfacePath));
    float anchorx = line["anchorx"].ToFloat();
    float anchory = line["anchory"].ToFloat();
    float scalex = line["scalex"].ToFloat();
    float scaley = line["scaley"].ToFloat();
    float angle = line["angle"].ToFloat();
    float fadetime = line["fadetime"].ToFloat();
    std::string action = line["action"].ToString();
    Bamiri *b = new Bamiri(0, 0, anchorx, anchory, scalex, scaley, 0, 0, action);
    character->bamiriList->insert(std::pair<std::string, Bamiri *>(face, b));
  }

#elif USEINI
  ini_parse_string(ss.c_str(), char_handler, this);
#else

#endif
}

CharacterManager::~CharacterManager() {
  if (_pool) {
    for (auto i = _pool->begin(); i != _pool->end(); i++) {
      auto c = i->second;
      CC_SAFE_DELETE(c);
    }
    _pool->clear();
    CC_SAFE_DELETE(_pool);
  }
}

CharacterManager *CharacterManager::getInstance() {
  if (_instance == nullptr) {
    _instance = new CharacterManager();
  }
  return _instance;
}

void CharacterManager::destoryInstance() { CC_SAFE_DELETE(_instance); }

Character *CharacterManager::getCharacter(std::string &key) {
  auto result = _pool->find(key);
  if (result != _pool->end()) {
    return result->second;
  } else {
    return nullptr;
  }
}

void CharacterManager::addCharacter(std::string &key, Character *cha) {
  auto c = _pool->find(key);
  if (c != _pool->end()) {
    _pool->erase(c);
    CC_SAFE_DELETE(c->second);
  }

  _pool->insert(std::pair<std::string, Character *>(key, cha));
}

YaImgManager *YaImgManager::_instance = nullptr;

static int yaimg_handler(void *user, const char *section, const char *name, const char *value, int lineno)

{
  YaImgManager *cm = (YaImgManager *)user;

  if (!strcmp(name, "name")) {
    auto img = new YaImg();
    std::string key(section);
    cm->addYaImg(key, img);
    img->name = value;

  } else {
    std::string key(section);
    YaImg *img = cm->getYaImg(key);

    if (!strcmp(name, "face")) {
      std::string temp(value);
      std::string face;
      std::string facePath;
      int tempPos = temp.find_first_of(',', 0);
      face = temp.substr(0, tempPos);
      facePath = temp.substr(tempPos + 1, temp.length() - tempPos - 1);
      facePath = "yaadv/img/" + facePath + ".png";
      char *cfacePath = new char[50];
      facePath.copy(cfacePath, facePath.length(), 0);
      *(cfacePath + facePath.length()) = '\0';
      img->fgList->insert(std::pair<std::string, char *>(face, cfacePath));
    }
  }
  return 0;
}

YaImgManager::YaImgManager()
    : _pool(nullptr)

{
  _pool = new std::map<std::string, YaImg *>();

  std::string ss = FileUtils::getInstance()->getStringFromFile(YaImgPath);

#if USEJSON
  YaImgManager *cm = (YaImgManager *)this;
  json::JSON data = json::JSON::Load(ss);
  for (int i = 0; i < data.length(); i++) {
    // { characterid: "", name: "", face: "", anchorx: "", ...}
    YaImg *yaimg;
    json::JSON line = data[i];
    if (line.hasKey("name")) {
      yaimg = new YaImg();
      std::string section = line["imageid"].ToString();
      std::string key(section);
      cm->addYaImg(key, yaimg);
      std::string value = line["name"].ToString();
      yaimg->name = value;

    } else {
      std::string section = line["imageid"].ToString();
      std::string key(section);
      yaimg = cm->getYaImg(key);
    }

    std::string filename = line["filename"].ToString();
    std::string face = line["face"].ToString();
    std::string facePath = "yaadv/img/" + filename + ".png";
    char *cfacePath = new char[128];
    facePath.copy(cfacePath, facePath.length(), 0);
    *(cfacePath + facePath.length()) = '\0';
    yaimg->fgList->insert(std::pair<std::string, char *>(face, cfacePath));
    float anchorx = line["anchorx"].ToFloat();
    float anchory = line["anchory"].ToFloat();
    float scalex = line["scalex"].ToFloat();
    float scaley = line["scaley"].ToFloat();
    Bamiri *b = new Bamiri(0, 0, anchorx, anchory, scalex, scaley, 0, 0);
    yaimg->bamiriList->insert(std::pair<std::string, Bamiri *>(face, b));
  }

#elif USEINI
  ini_parse_string(ss.c_str(), yaimg_handler, this);
#else

#endif
}

YaImgManager::~YaImgManager() {
  if (_pool) {
    for (auto i = _pool->begin(); i != _pool->end(); i++) {
      auto c = i->second;
      CC_SAFE_DELETE(c);
    }
    _pool->clear();
    CC_SAFE_DELETE(_pool);
  }
}

YaImgManager *YaImgManager::getInstance() {
  if (_instance == nullptr) {
    _instance = new YaImgManager();
  }
  return _instance;
}

void YaImgManager::destoryInstance() { CC_SAFE_DELETE(_instance); }

YaImg *YaImgManager::getYaImg(std::string &key) {
  auto result = _pool->find(key);
  if (result != _pool->end()) {
    return result->second;
  } else {
    return nullptr;
  }
}

void YaImgManager::addYaImg(std::string &key, YaImg *cha) {
  auto c = _pool->find(key);
  if (c != _pool->end()) {
    _pool->erase(c);
    CC_SAFE_DELETE(c->second);
  }

  _pool->insert(std::pair<std::string, YaImg *>(key, cha));
}

}  // namespace yaadv
