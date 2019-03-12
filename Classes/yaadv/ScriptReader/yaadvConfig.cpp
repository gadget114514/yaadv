
#include "yaadvConfig.h"
#include "cocos2d.h"

#include "../expr.h"

using namespace yaadv;

#if USEJSON
#include "json.hpp"
#define BGLISTPATH "yaadv/config.json"
#elif USEINI
#define INI_HANDLER_LINENO 1
#include "ini.h"
#define BGLISTPATH "yaadv/config.ini"
#else
#define BGLISTPATH "yaadv/config.txt"
#endif

USING_NS_CC;

namespace yaadv {

Config* Config::_instance = nullptr;

Config* Config::getInstance() {
  if (_instance == nullptr) {
    _instance = new Config();
  }
  return _instance;
}

#if USEINI
static int config_handler(void* user, const char* section, const char* name, const char* value, int lineno)

{
  Config* c = (Config*)user;
  std::string key(name);
  std::string v(value);
  c->vars[key] = v;

  return 0;
}
#endif

bool Config::load(std::string data) {
#if USEJSON
  content = json::JSON::Load(data);

  if (content.hasKey("designWidth")) {
    this->designWidth = content["designWidth"].ToInt();
  }
  if (content.hasKey("designHeight")) {
    this->designHeight = content["designHeight"].ToInt();
  }
  if (content.hasKey("windowTitle")) {
    this->windowTitle = content["windowTitle"].ToString();
  }
  if (content.hasKey("resolutionPolicy")) {
    this->resolutionPolicy = ResolutionPolicy(content["resolutionPolicy"].ToInt());
  }

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

bool Config::init() {
  loadFile(BGLISTPATH);
  return true;
}

bool Config::contains(std::string& key) { return content.hasKey(key); }

#if USEJSON

std::string Config::operator[](std::string& key) {
  if (content.hasKey(key)) {
    return content[key].ToString();
  }
  return key;
}
std::string Config::operator[](char* key) {
  std::string s(key);
  return (*this)[s];
}
std::map<std::string, std::string> vars;

#elif USEINI
std::string Config::operator[](std::string& key) {
  if (vars.find(key) != vars.end()) {
    return vars[key];
  }
  //    log(key.c_str());
  return key;
}
std::string Config::operator[](char* key) {
  std::string s(key);
  return (*this)[s];
}

bool Config::contains(std::string& key) { return vars.find(key) != vars.end(); }
#endif


int yaadv::NodeConfig::evalToInt(std::string e) { 
	double d = yaadv::Expr(e.c_str(), vars);
	return (int)d; 
}

void yaadv::NodeConfig::init(yaadv::Config* config, std::string& key) {
  json::JSON data = config->setting[key];
  this->x = 0;
  this->y = 0;
  this->depth = -1;
  this->anchor = cocos2d::Vec2(0, 0);
  this->opacity = 1.0;
  this->fontsize = 20;
  this->textdimx = 20;
  this->textdimy = 24;
  std::string rgb = "#FFFFFF";
  if (data.hasKey("x")) {
    auto item = data["x"];
    if (item.JSONType() == json::JSON::Class::String) {
      x = (this->evalToInt(item.ToString()));
    } else {
      x = (item.ToInt());
    }
  }
  if (data.hasKey("y")) {
    auto item = data["y"];
    if (item.JSONType() == json::JSON::Class::String) {
      y = (this->evalToInt(item.ToString()));
    } else {
      y = (item.ToInt());
    }
  }
  if (data.hasKey("w")) {
    auto item = data["w"];
    if (item.JSONType() == json::JSON::Class::String) {
      w = (this->evalToInt(item.ToString()));
    } else {
      w = (item.ToInt());
    }
  }
  if (data.hasKey("h")) {
    auto item = data["h"];
    if (item.JSONType() == json::JSON::Class::String) {
      h = (this->evalToInt(item.ToString()));
    } else {
      h = (item.ToInt());
    }
  }
  if (data.hasKey("d")) {
    auto item = data["d"];
    depth = item.ToInt();
  }
  if (data.hasKey("o")) {
    auto item = data["o"];
    x += this->origin.x;
    y += this->origin.y;
  }
  if (data.hasKey("a")) {
    auto item = data["a"];
    std::string v = item.ToString();
    if (v.compare("middle")) {
      anchor = Vec2::ANCHOR_MIDDLE;
    } else if (v.compare("middleright")) {
      anchor = Vec2::ANCHOR_MIDDLE_RIGHT;
    } else if (v.compare("middleleft")) {
      anchor = Vec2::ANCHOR_MIDDLE_LEFT;
    } else if (v.compare("middletop")) {
      anchor = Vec2::ANCHOR_MIDDLE_TOP;
    } else if (v.compare("middlebottom")) {
      anchor = Vec2::ANCHOR_MIDDLE_BOTTOM;
    } else if (v.compare("middle")) {
      anchor = Vec2::ANCHOR_MIDDLE;
    }
  }
  if (data.hasKey("opacity")) {
    auto item = data["opacity"];
    opacity = item.ToFloat();
  }

  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();
  vars["vw"] = visibleSize.width;
  vars["vh"] = visibleSize.height;
  vars["ox"] = origin.x;
  vars["oy"] = origin.y;
  if (parent) {
    vars["pw"] = parent->getContentSize().width;
    vars["ph"] = parent->getContentSize().height;
  }
}

void NodeConfig::set(cocos2d::Node* nd, cocos2d::Node* parent) {
  nd->setPosition(Vec2(x, y));
  nd->setAnchorPoint(anchor);
  if (opacity != 1.0) nd->setOpacity(opacity);
  if (depth >= 0)
    parent->addChild(nd, depth);
  else
    parent->addChild(nd);
}

}  // namespace yaadv
