#ifndef HISTORY_H
#define HISTORY_H

#include <stdio.h>
#include "cocos2d.h"

USING_NS_CC;
#define MAXIMUM 80

namespace yaadv {

enum recordType { null, talk, selecte };

struct record {
  recordType type;
  std::string text;
  std::string name;
  Color4B color;
  record(recordType type, std::string text, std::string name = "", Color4B color = Color4B::WHITE)
      : type(type),
        text(text),
        name(name),
        color(color){

        };
};

class HistoryLogger {
  int _maximum;
  int _currentLog;
  record* _records[MAXIMUM];
  static HistoryLogger* _instance;

 public:
  HistoryLogger();
  ~HistoryLogger();
  static HistoryLogger* getInstance();
  static void destoryInstance();
  record* createRecord(const std::string type, std::string text, std::string name, Color4B color = Color4B::WHITE);
  void addRecord(const std::string type, std::string text, std::string name, Color4B color = Color4B::WHITE);
  void pushRecord(record record);
  void addRecord(record* record);
  int getLength();
  record* getRecord(int i);
};
};
#endif