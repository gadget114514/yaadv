#pragma once
#include <map>
#include <string>

namespace yaadv {

class Config {
  static Config *_instance;

 public:
  std::string operator[](std::string &key) { 
	if (vars.find(key) != vars.end()) {
      return vars[key]; 
    }
    //    log(key.c_str());
        return key;
  }
  std::string operator[](char*key) {
    std::string s(key);
    return (*this)[s];
  }
  std::map<std::string, std::string> vars;
  bool load(std::string data);
  bool loadFile(std::string filename);
  static Config *getInstance();
};

}  // namespace yaadv
