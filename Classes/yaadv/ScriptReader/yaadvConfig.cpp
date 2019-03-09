#include "yaadvConfig.h"
#include "cocos2d.h"
#define DEFINEJSON 1
#include "json.hpp"

using namespace yaadv;
#define USEJSON 0
#define USEINI 1
#if USEJSON
#include "json.hpp"
#define BGLISTPATH "yaadvconfig.json"
#elif USEINI
#define INI_HANDLER_LINENO 1
#include "ini.h"
#define BGLISTPATH "yaadvconfig.ini"
#else
#define BGLISTPATH "yaadvconfig.txt"
#endif


USING_NS_CC;

namespace yaadv {

 Config *Config::_instance = nullptr;


 Config *Config::getInstance()
{
  if (_instance == nullptr) {
    _instance = new Config();
  }
  return _instance;
}


static int config_handler(void* user, const char* section, const char* name,
                        const char* value, int lineno)

{

 Config* c = (Config*)user;
  std::string key(name);
  std::string v(value);
  c->vars[key] = v;
  
  return 0;
}

bool Config::load(std::string data) {
	#if USEJSON	
	#elif USEINI
    ini_parse_string(data.c_str(), config_handler, this);
#else
#endif
	
	return true;
}

bool Config::loadFile(std::string data) {
	  std::string ss = FileUtils::getInstance()->getStringFromFile(data);
	  load(ss);
	return true;
}





}  // namespace yaadv


