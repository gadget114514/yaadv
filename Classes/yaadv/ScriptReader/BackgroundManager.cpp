#include "BackgroundManager.h"

using namespace yaadv;
#define USEJSON 0
#define USEINI 1
#if USEJSON
#include "json.hpp"
#define BGLISTPATH "yaadv/bgimage/bgList.json"
#elif USEINI
#define INI_HANDLER_LINENO 1
#include "ini.h"
#define BGLISTPATH "yaadv/bgimage/bgList.ini"
#else
#define BGLISTPATH "yaadv/bgimage/bgList.txt"
#endif


namespace yaadv {

BackgroundManager *BackgroundManager::_instance = nullptr;


static int bg_handler(void* user, const char* section, const char* name,
                        const char* value, int lineno)

{
  BackgroundManager* bm = (BackgroundManager*)user;
  std::string key(name);
  std::string background(value);

  background = "yaadv/bgimage/" + background;

  bm->addBackground(key, background);
  log("SCB#addBackground[%s]", key.c_str());
  return 0;
}


BackgroundManager::BackgroundManager() : _pool(nullptr) {
  _pool = new std::map<std::string, std::string>();

  defaultBackground = "";

  std::string ss = FileUtils::getInstance()->getStringFromFile(BGLISTPATH);

  #if USEJSON 
   
  BackgroundManager* bm = (BackgroundManager*)this;
  json::JSON data = json::JSON::Load(ss);
  for (int i = 0; i < data.length(); i++) {
    // { characterid: "", name: "", face: "", anchorx: "", ...}

    json::JSON line = data[i];

    std::string key = line["imageid"].ToString();

    std::string fn = line["filename"].ToString();
    fn = "yaadv/bgimage/" + fn + ".png";
    bm->addBackground(key, fn);
   
  }

  #elif USEINI
    ini_parse_string(ss.c_str(), bg_handler, this);

  #else


#endif
}

BackgroundManager::~BackgroundManager() {
  if (_pool) {
    _pool->clear();
    CC_SAFE_DELETE(_pool);
  }
}

void BackgroundManager::addBackground(std::string key, std::string background) {
  _pool->insert(std::pair<std::string, std::string>(key, background));
}

std::string BackgroundManager::getBackground(std::string key) {
  auto result = _pool->find(key);
  if (result != _pool->end()) {
    return result->second;
  } else {
    defaultBackground = "yaadv/bgimage/black.jpg";
    return defaultBackground;
  }
}

BackgroundManager *BackgroundManager::getInstance() {
  if (_instance == nullptr) {
    _instance = new BackgroundManager();
  }
  return _instance;
}

void BackgroundManager::destoryInstance() { CC_SAFE_DELETE(_instance); }


}  // namespace yaadv