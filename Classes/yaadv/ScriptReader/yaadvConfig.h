#pragma once
#include <map>
#include <string>

#include "json.hpp"
#define USEJSON 1
#define USEINI 0

namespace yaadv {

class Config {
  static Config *_instance;

 public:
  int designWidth = 1136;
  int designHeight = 640;
  std::string windowTitle = "noname";
  ResolutionPolicy resolutionPolicy = ResolutionPolicy::NO_BORDER;
#if USEJSON
  json::JSON content;
  std::string operator[](std::string &key);
  std::string operator[](char *key);

  bool contains(std::string &key);

#elif USEINI
  std::string operator[](std::string &key);
  std::string operator[](char *key);
  std::map<std::string, std::string> vars;
  bool contains(std::string &key);
#endif
  bool load(std::string data);
  bool loadFile(std::string filename);
  static Config *getInstance();
  bool init();

  bool loadConfigFile(std::string filename);
  bool loadConfig(std::string data);
  std::map<std::string, json::JSON> setting;
};

class NodeConfig {
 public:
  int x;
  int y;
  int w;
  int h;
  int depth;
  int fontsize;
  int textdimx;
  int textdimy;
  std::map<std::string, double> vars;

  cocos2d::Node *parent;
  cocos2d::Vec2 anchor;
  float opacity;
  void init(Config *config, std::string &key);
  void set(cocos2d::Node *node, cocos2d::Node *parent);

  cocos2d::Vec2 origin;
  cocos2d::Vec2 v;

  int evalToInt(std::string);
};
}  // namespace yaadv
