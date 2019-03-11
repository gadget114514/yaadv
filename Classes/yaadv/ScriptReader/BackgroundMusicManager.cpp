#include "BackgroundMusicManager.h"
using namespace yaadv;
#define USEJSON 0
#define USEINI 1

#if USEJSON
#include "json.hpp"
#define BGMLISTPATH "yaadv/bgm/bgmList.json"
#elif USEINI
#define INI_HANDLER_LINENO 1
#include "ini.h"
#define BGMLISTPATH "yaadv/bgm/bgmList.ini"
#else
#define BGMLISTPATH "yaadv/bgm/bgmList.txt"
#endif

namespace yaadv {

BackgroundMusicManager* BackgroundMusicManager::_instance = nullptr;

static int bgm_handler(void* user, const char* section, const char* name, const char* value, int lineno)

{
  BackgroundMusicManager* bm = (BackgroundMusicManager*)user;
  std::string key(name);
  std::string background(value);

  background = "yaadv/bgm/" + background + ".mp3";

  bm->addBackgroundMusic(key, background);

  return 0;
}

BackgroundMusicManager::BackgroundMusicManager() : _pool(nullptr) {
  _pool = new std::map<std::string, std::string>();

  defaultBackgroundMusic = "";

  std::string ss = FileUtils::getInstance()->getStringFromFile(BGMLISTPATH);

#if USEJSON

  BackgroundMusicManager* bm = (BackgroundMusicManager*)this;
  json::JSON data = json::JSON::Load(ss);
  for (int i = 0; i < data.length(); i++) {
    // { characterid: "", name: "", face: "", anchorx: "", ...}

    json::JSON line = data[i];

    std::string key = line["soundid"].ToString();

    std::string fn = line["filename"].ToString();
    fn = "bgm/" + fn + ".mp3";
    bm->addBackgroundMusic(key, fn);
  }
#elif USEINI
  ini_parse_string(ss.c_str(), bgm_handler, this);

#else

#endif
}

BackgroundMusicManager::~BackgroundMusicManager() {
  if (_pool) {
    _pool->clear();
    CC_SAFE_DELETE(_pool);
  }
}

void BackgroundMusicManager::addBackgroundMusic(std::string key, std::string background) {
  _pool->insert(std::pair<std::string, std::string>(key, background));
}

std::string BackgroundMusicManager::getBackgroundMusic(std::string key) {
  auto result = _pool->find(key);
  if (result != _pool->end()) {
    return result->second;
  } else {
    defaultBackgroundMusic = "";
    return defaultBackgroundMusic;
  }
}

BackgroundMusicManager* BackgroundMusicManager::getInstance() {
  if (_instance == nullptr) {
    _instance = new BackgroundMusicManager();
  }
  return _instance;
}

void BackgroundMusicManager::destoryInstance() { CC_SAFE_DELETE(_instance); }

}  // namespace yaadv