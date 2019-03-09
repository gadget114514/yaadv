#include "SoundManager.h"

#define USEJSON 0
#define USEINI 1
#if USEJSON
#define SOUNDLISTPATH "yaadv/sound/soundList.json"
#include "json.hpp"
#elif USEINI
#define INI_HANDLER_LINENO 1
#include "ini.h"
#define SOUNDLISTPATH "yaadv/sound/soundList.ini"
#else
#define SOUNDLISTPATH "yaadv/sound/soundList.txt"
#endif
using namespace yaadv;

namespace yaadv {

SoundManager* SoundManager::_instance = nullptr;

static int snd_handler(void* user, const char* section, const char* name,
                       const char* value, int lineno)

{
  SoundManager* bm = (SoundManager*)user;
  std::string key(name);
  std::string snd(value);
  snd = "yaadv/sound/" + snd + ".mp3";

  bm->addSound(key, snd);
  log("SM#addSound[%s]", key.c_str());
  return 0;
}

SoundManager::SoundManager() : _pool(nullptr) {
  _pool = new std::map<std::string, std::string>();

  defaultSound = "";

  std::string ss = FileUtils::getInstance()->getStringFromFile(SOUNDLISTPATH);

#if USEJSON
  SoundManager* bm = (SoundManager*)this;
  json::JSON data = json::JSON::Load(ss);
  for (int i = 0; i < data.length(); i++) {
    // { characterid: "", name: "", face: "", anchorx: "", ...}

    json::JSON line = data[i];

    std::string key = line["soundid"].ToString();

    std::string snd = line["filename"].ToString();
    snd = "sound/" + snd + ".mp3";

    bm->addSound(key, snd);
	}

#elif USEINI

  ini_parse_string(ss.c_str(), snd_handler, this);

#else
  int sPos = 0;
  int ePos = 0;

  ePos = ss.find('\n', sPos);
  sPos = ePos + 1;

  while (1) {
    std::string key;
    std::string sound;
    std::string temp;
    ePos = ss.find('\n', sPos);
    temp = ss.substr(sPos, ePos - sPos - 1);
    if (temp.compare("") == 0) {
      log("SM> Load Sound ending");
      break;
    }
    sPos = ePos + 1;

    int tempPos = temp.find_first_of(',', 0);
    key = temp.substr(0, tempPos);
    sound = temp.substr(tempPos + 1, temp.length() - tempPos - 1);
    sound = "sound/" + sound + ".mp3";

    log("soundkey = %s , soundPath = %s", key.c_str(), sound.c_str());

    addSound(key, sound);
    log("SM> addSound[%s]", key.c_str());
  }
#endif
  }

  SoundManager::~SoundManager() {
    if (_pool) {
      _pool->clear();
      CC_SAFE_DELETE(_pool);
    }
  }

  void SoundManager::addSound(std::string key, std::string sound) {
    _pool->insert(std::pair<std::string, std::string>(key, sound));
  }

  std::string SoundManager::getSound(std::string key) {
    auto result = _pool->find(key);
    if (result != _pool->end()) {
      return result->second;
    } else {
      log("SM> Not found &s", key.c_str());
      defaultSound = "";
      return defaultSound;
    }
  }

  SoundManager* SoundManager::getInstance() {
    if (_instance == nullptr) {
      _instance = new SoundManager();
    }
    return _instance;
  }

  void SoundManager::destoryInstance() { CC_SAFE_DELETE(_instance); }

  
}  // namespace yaadv