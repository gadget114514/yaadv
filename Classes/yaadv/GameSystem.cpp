#include "GameSystem.h"
#include <stdlib.h>
#include "AudioHelper.h"
#include "ScriptReader/BackgroundManager.h"
#include "ScriptReader/BackgroundMusicManager.h"
#include "ScriptReader/CharacterManager.h"
#include "ScriptReader/ScriptReader.h"
#include "ScriptReader/SoundManager.h"
#include "base/CCUserDefault.h"

USING_NS_CC;
using namespace yaadv;
namespace yaadv {

#define DEFAULT_SYSTEMVOLUME 1.0f
#define DEFAULT_MUSICVOLUME 1.0f
#define DEFAULT_SOUNDVOLUME 1.0f
#define DEFAULT_TEXTSPEED 1.0f
#define DEFAULT_AUTOSPEED 1.0f
#define DEFAULT_ISSKIPALL false
#define MAX_SAVEDATA_NUMBER 100
#define SAVEDATA_STR_LENGTH 8

#define ISINIT "isInitialization"

GameSystem *GameSystem::_instance = nullptr;

GameSystem::GameSystem() {
  if (!cocos2d::UserDefault::getInstance()->getBoolForKey(ISINIT)) {
    setDefault();
  }
  _isLoadSuccess = false;
  _savedata = new std::map<std::string, int>[100];
  _gameSceneInfo = nullptr;

  _savedataList = new GameSaveData[MAX_SAVEDATA_NUMBER];
  createSavedata();
  initGameSavedataList();
  _gameScene = nullptr;

  CharacterManager::getInstance();
  BackgroundManager::getInstance();
  BackgroundMusicManager::getInstance();
  SoundManager::getInstance();
  // ScriptReader::setInstance(ScriptReaderYASL::getInstance());
  ScriptReader::setInstance(ScriptReaderWREN::getInstance());
  ScriptReader::getInstance();

  AudioHelper::getAudioEngine()->setBackgroundMusicVolume(getMusicVolume());
}

GameSystem::~GameSystem() {
  CC_SAFE_DELETE_ARRAY(_savedata);
  CC_SAFE_DELETE(_gameSceneInfo);
  CC_SAFE_DELETE_ARRAY(_savedataList);

  if (_gameScene) {
    _gameScene = nullptr;
  }
}

GameSystem *GameSystem::getInstance() {
  if (_instance == nullptr) {
    _instance = new GameSystem();
  }
  return _instance;
}

void GameSystem::destoryInstance() {
  BackgroundManager::destoryInstance();
  BackgroundMusicManager::destoryInstance();
  CharacterManager::destoryInstance();
  ScriptReader::destoryInstance();
  SoundManager::destoryInstance();

  CC_SAFE_DELETE(_instance);
}
void GameSystem::setDefault() {
  setSystemVolume(DEFAULT_SYSTEMVOLUME);
  setMusicVolume(DEFAULT_MUSICVOLUME);
  setSoundVolume(DEFAULT_SOUNDVOLUME);
  setTextSpeed(DEFAULT_TEXTSPEED);
  setAutoSpeed(DEFAULT_AUTOSPEED);
  setIsSkipAll(DEFAULT_ISSKIPALL);
  cocos2d::UserDefault::getInstance()->setBoolForKey(ISINIT, true);
}

void GameSystem::setSystemVolume(float value) {
  cocos2d::UserDefault::getInstance()->setFloatForKey(SYSTEMVOLUME, value);
}

void GameSystem::setMusicVolume(float value) {
  cocos2d::UserDefault::getInstance()->setFloatForKey(MUSICVOLUME, value);
}

void GameSystem::setSoundVolume(float value) {
  cocos2d::UserDefault::getInstance()->setFloatForKey(SOUNDVOLUME, value);
}

void GameSystem::setTextSpeed(float value) { cocos2d::UserDefault::getInstance()->setFloatForKey(TEXTSPEED, value); }

void GameSystem::setAutoSpeed(float value) { cocos2d::UserDefault::getInstance()->setFloatForKey(AUTOSPEED, value); }

void GameSystem::setHaveRead(const std::string &key, int value) {
  cocos2d::UserDefault::getInstance()->setIntegerForKey(key.c_str(), value);
  cocos2d::UserDefault::sharedUserDefault()->flush();
}

void GameSystem::setIsSkipAll(bool value) { cocos2d::UserDefault::getInstance()->setBoolForKey(ISSKIPALL, value); }

float GameSystem::getSystemVolume() { return cocos2d::UserDefault::getInstance()->getFloatForKey(SYSTEMVOLUME); }

float GameSystem::getMusicVolume() { return cocos2d::UserDefault::getInstance()->getFloatForKey(MUSICVOLUME); }

float GameSystem::getSoundVolume() { return cocos2d::UserDefault::getInstance()->getFloatForKey(SOUNDVOLUME); }

float GameSystem::getTextSpeed() { return cocos2d::UserDefault::getInstance()->getFloatForKey(TEXTSPEED); }

float GameSystem::getAutoSpeed() { return cocos2d::UserDefault::getInstance()->getFloatForKey(AUTOSPEED); }

int GameSystem::getHaveRead(const std::string &key) {
  return cocos2d::UserDefault::getInstance()->getIntegerForKey(key.c_str());
}

bool GameSystem::getIsSkipAll() { return cocos2d::UserDefault::getInstance()->getBoolForKey(ISSKIPALL); }

void GameSystem::setDataValue(std::string &key, int value) {
  auto result = _savedata[0].find(key);
  if (result != _savedata[0].end()) {
    result->second = value;
  } else {
    _savedata[0].insert(std::pair<std::string, int>(key, value));
  }
}

int GameSystem::getDataValue(std::string &key) {
  auto result = _savedata[0].find(key);

  if (result != _savedata[0].end()) {
    return result->second;
  } else {
    return 0;
  }
}

bool GameSystem::isDataValueExists(const std::string &key) {
  auto result = _savedata[0].find(key);
  if (result != _savedata[0].end()) {
    return true;
  } else {
    return false;
  }
}

std::string GameSystem::dataValueToString() {
  std::string dataValueStr = "";
  for (auto it = _savedata[0].begin(); it != _savedata[0].end(); ++it) {
    dataValueStr += it->first;
    dataValueStr += "=";
    char *ch = new char[13];

    sprintf(ch, "%d", it->second);
    dataValueStr += ch;
    CC_SAFE_DELETE_ARRAY(ch);
    dataValueStr += ",";
  }
  return dataValueStr;
}

void GameSystem::stringToDataValue(const std::string &dataValueStr) {
  if (_savedata != nullptr) {
    CC_SAFE_DELETE_ARRAY(_savedata);
  }
  _savedata = new std::map<std::string, int>[128];

  int sPos = 0;
  int ePos = 0;
  std::string temp;
  while (sPos < dataValueStr.length()) {
    ePos = dataValueStr.find(',', sPos);
    temp = dataValueStr.substr(sPos, ePos - sPos);

    std::string key;
    std::string value;
    key = temp.substr(0, temp.find('=', 0));
    value = temp.substr(key.length() + 1, temp.length() - (key.length() + 1));
    setDataValue(value, atoi(value.c_str()));
    sPos = ePos + 1;
  }
}

void GameSystem::setData(std::map<std::string, int> *map) {
  if (map == nullptr) {
    if (!(_savedata[0].empty())) _savedata[0].clear();
  } else {
    _savedata[0] = *map;
  }
}

void GameSystem::setSavedata(int i, bool value) {
  char *ch = new char[13];

  sprintf(ch, "%d", i);
  cocos2d::UserDefault::getInstance()->setBoolForKey(ch, value);
  CC_SAFE_DELETE_ARRAY(ch);
}

bool GameSystem::getSavedata(int i) {
  char *ch = new char[13];

  sprintf(ch, "%d", i);
  bool result = cocos2d::UserDefault::getInstance()->getBoolForKey(ch);
  CC_SAFE_DELETE_ARRAY(ch);
  return result;
}

GameData *GameSystem::getGameSceneInfo() { return _gameSceneInfo; }

void GameSystem::setGameSceneInfo(GameData *gameData) {
  CC_SAFE_DELETE(_gameSceneInfo);
  _gameSceneInfo = gameData;
}

void GameSystem::setIsLoadSuccess(bool value) { _isLoadSuccess = value; }

bool GameSystem::getIsLoadSuccess() { return _isLoadSuccess; }

bool GameSystem::saveGameSceneInfo(int i) {
  bool ret = false;
  setSavedata(i, true);

  const char *path = "savedata/savedata";
  char *ch = new char[13];
  sprintf(ch, "%d", i + 1);
  char *file = new char[500];
  sprintf(file, "%s%s%s.sav", FileUtils::getInstance()->getWritablePath().c_str(), path, ch);
  char *image = new char[100];
  snprintf(image, 100, "%s%d.jpg", path, i + 1);
  FILE *savedata = fopen(file, "wb");
  if (savedata) {
    bool result = _screenShoot->saveToFile(image, false, [&](RenderTexture *r, const std::string &s) {});
    if (result) {
      char tmp[100];

      sprintf(tmp, "%s", image);
      fwrite(tmp, sizeof(char), strlen(tmp), savedata);
    }
    fputs("\r\n", savedata);

    time_t t;
    time(&t);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %X", localtime((&t)));
    fwrite(tmp, sizeof(char), strlen(tmp), savedata);
    fputs("\r\n", savedata);

    fwrite(_gameSceneInfo->currentText.c_str(), sizeof(char), strlen(_gameSceneInfo->currentText.c_str()), savedata);
    fputs("\r\n", savedata);

    fwrite(_gameSceneInfo->currentName.c_str(), sizeof(char), strlen(_gameSceneInfo->currentName.c_str()), savedata);
    fputs("\r\n", savedata);

    fwrite(_gameSceneInfo->backgroundKey.c_str(), sizeof(char), strlen(_gameSceneInfo->backgroundKey.c_str()),
           savedata);
    fputs("\r\n", savedata);

    char cCharacterCount[2];
    sprintf(cCharacterCount, "%d", _gameSceneInfo->characterCount);
    fwrite(cCharacterCount, sizeof(char), strlen(cCharacterCount), savedata);
    fputs("\r\n", savedata);

    for (int j = 0; j < _gameSceneInfo->characterCount; j++) {
      fwrite(_gameSceneInfo->fgCharacters[j].name.c_str(), sizeof(char),
             strlen(_gameSceneInfo->fgCharacters[j].name.c_str()), savedata);
      fputs("\r\n", savedata);

      fwrite(_gameSceneInfo->fgCharacters[j].face.c_str(), sizeof(char),
             strlen(_gameSceneInfo->fgCharacters[j].face.c_str()), savedata);
      fputs("\r\n", savedata);

      char num[2];
      sprintf(num, "%d", _gameSceneInfo->fgCharacters[j].number);
      fwrite(num, sizeof(char), strlen(num), savedata);
      fputs("\r\n", savedata);
    }

    fwrite(_gameSceneInfo->bgmKey.c_str(), sizeof(char), strlen(_gameSceneInfo->bgmKey.c_str()), savedata);
    fputs("\r\n", savedata);

    char num[2];
    sprintf(num, "%d", _gameSceneInfo->optionsNumber);
    fwrite(num, sizeof(char), strlen(num), savedata);
    fputs("\r\n", savedata);
    if (_gameSceneInfo->optionsNumber) {
      for (auto itr = _gameSceneInfo->options.begin(); itr != _gameSceneInfo->options.end(); itr++) {
        fwrite(itr->first.c_str(), sizeof(char), strlen(itr->first.c_str()), savedata);
        fputs("\r\n", savedata);
        fwrite(itr->second.c_str(), sizeof(char), strlen(itr->second.c_str()), savedata);
        fputs("\r\n", savedata);
      }
    }

    auto sign = ScriptReader::getInstance()->getCurrentSignName();
    fwrite(sign.c_str(), sizeof(char), strlen(sign.c_str()), savedata);
    fputs("\r\n", savedata);

    auto commandIndex = ScriptReader::getInstance()->getCurrentCommandIndex();
    char sCommandIndex[4];
    sprintf(sCommandIndex, "%d", commandIndex);
    fwrite(sCommandIndex, sizeof(char), strlen(sCommandIndex), savedata);
    fputs("\r\n", savedata);

    auto dataValueStr = dataValueToString();
    fwrite(dataValueStr.c_str(), sizeof(char), strlen(dataValueStr.c_str()), savedata);
    fputs("\r\n", savedata);

    fclose(savedata);

    ret = true;
  } else {
    ret = false;
    // save error: should be treated
  }
  CC_SAFE_DELETE_ARRAY(ch);
  CC_SAFE_DELETE_ARRAY(file);
  CC_SAFE_DELETE_ARRAY(image);
  savedata = NULL;
  return ret;
}

void GameSystem::initGameSavedataList() {
  bool isNeedModifyPopupSetting = FileUtils::sharedFileUtils()->isPopupNotify();

  if (isNeedModifyPopupSetting) {
    FileUtils::sharedFileUtils()->setPopupNotify(false);
  }

  for (int i = 0; i < MAX_SAVEDATA_NUMBER; i++) {
    updateGameSavedata(i);
  }

  if (isNeedModifyPopupSetting) {
    FileUtils::sharedFileUtils()->setPopupNotify(true);
  }
}

GameSaveData *GameSystem::getGameSavedata(int i) {
  if (_savedataList[i].date.compare("") == 0)
    return nullptr;
  else
    return &_savedataList[i];
}

bool GameSystem::createSavedata() {
  std::string path = FileUtils::getInstance()->getWritablePath();

  path += "savedata";

  if (!FileUtils::getInstance()->isDirectoryExist(path)) {
    FileUtils::getInstance()->createDirectory(path);
    return true;
  } else {
    return true;
  }
}

void GameSystem::setScreenShoot(RenderTexture *render) { _screenShoot = render; }

RenderTexture *GameSystem::getScreenShoot() { return _screenShoot; }

bool GameSystem::updateGameSavedata(int i) {
  char path[] = "savedata/savedata";
  char ch[4];
  sprintf(ch, "%d", i + 1);
  char file[100];
  sprintf(file, "%s%s%s%s", FileUtils::getInstance()->getWritablePath().c_str(), path, ch, ".sav");

  std::string data = FileUtils::getInstance()->getStringFromFile(file);
  if (data.compare("") != 0) {
    int sPos = 0;
    int ePos = 0;
    std::string temp;

    ePos = data.find('\n', sPos);
    temp = data.substr(sPos, ePos - sPos - 1);

    _savedataList[i].imageFile = FileUtils::getInstance()->getWritablePath() + temp;
    sPos = ePos + 1;

    ePos = data.find('\n', sPos);
    temp = data.substr(sPos, ePos - sPos - 1);

    _savedataList[i].date = temp;
    sPos = ePos + 1;

    ePos = data.find('\n', sPos);
    temp = data.substr(sPos, ePos - sPos - 1);

    _savedataList[i].text = temp;
    sPos = ePos + 1;
  }
  return true;
}

bool GameSystem::loadGameSceneInfo(int i) {
  char path[] = "savedata/savedata";
  char ch[4];
  sprintf(ch, "%d", i + 1);
  char file[100];
  sprintf(file, "%s%s%s%s", FileUtils::getInstance()->getWritablePath().c_str(), path, ch, ".sav");
  std::string data = FileUtils::getInstance()->getStringFromFile(file);

  setGameSceneInfo(new GameData());

  if (data.compare("") != 0) {
    int sPos = 0;
    int ePos = 0;
    std::string temp;

    ePos = data.find('\n', sPos);
    temp = data.substr(sPos, ePos - sPos - 1);

    sPos = ePos + 1;

    ePos = data.find('\n', sPos);
    temp = data.substr(sPos, ePos - sPos - 1);

    sPos = ePos + 1;

    ePos = data.find('\n', sPos);
    temp = data.substr(sPos, ePos - sPos - 1);
    _gameSceneInfo->currentText = temp;
    sPos = ePos + 1;

    ePos = data.find('\n', sPos);
    temp = data.substr(sPos, ePos - sPos - 1);
    sPos = ePos + 1;
    _gameSceneInfo->currentName = temp;

    ePos = data.find('\n', sPos);
    temp = data.substr(sPos, ePos - sPos - 1);
    sPos = ePos + 1;
    _gameSceneInfo->backgroundKey = temp;

    ePos = data.find('\n', sPos);
    temp = data.substr(sPos, ePos - sPos - 1);
    sPos = ePos + 1;
    _gameSceneInfo->characterCount = atoi(temp.c_str());

    for (int j = 0; j < _gameSceneInfo->characterCount; j++) {
      ePos = data.find('\n', sPos);
      temp = data.substr(sPos, ePos - sPos - 1);
      sPos = ePos + 1;
      _gameSceneInfo->fgCharacters[j].name = temp;

      ePos = data.find('\n', sPos);
      temp = data.substr(sPos, ePos - sPos - 1);
      sPos = ePos + 1;
      _gameSceneInfo->fgCharacters[j].face = temp;

      ePos = data.find('\n', sPos);
      temp = data.substr(sPos, ePos - sPos - 1);
      sPos = ePos + 1;
      _gameSceneInfo->fgCharacters[j].number = atoi(temp.c_str());
    }

    ePos = data.find('\n', sPos);
    temp = data.substr(sPos, ePos - sPos - 1);
    sPos = ePos + 1;
    _gameSceneInfo->bgmKey = temp;

    ePos = data.find('\n', sPos);
    temp = data.substr(sPos, ePos - sPos - 1);
    sPos = ePos + 1;
    _gameSceneInfo->optionsNumber = atoi(temp.c_str());
    for (int j = 0; j < _gameSceneInfo->optionsNumber; j++) {
      std::string sign;
      std::string text;
      ePos = data.find('\n', sPos);
      sign = data.substr(sPos, ePos - sPos - 1);
      sPos = ePos + 1;
      ePos = data.find('\n', sPos);
      text = data.substr(sPos, ePos - sPos - 1);
      sPos = ePos + 1;
      _gameSceneInfo->options.insert(std::pair<std::string, std::string>(sign, text));
    }

    ePos = data.find('\n', sPos);
    temp = data.substr(sPos, ePos - sPos - 1);
    sPos = ePos + 1;
    _gameSceneInfo->currentSignName = temp;

    ePos = data.find('\n', sPos);
    temp = data.substr(sPos, ePos - sPos - 1);
    sPos = ePos + 1;
    _gameSceneInfo->currentCommandIndex = atoi(temp.c_str());

    ePos = data.find('\n', sPos);
    temp = data.substr(sPos, ePos - sPos - 1);
    sPos = ePos + 1;
    stringToDataValue(temp);

    return true;
  } else {
    ;
    return false;
  }
}

void GameSystem::setGameScene(Scene *scene) { _gameScene = scene; }

Scene *GameSystem::getGameScene() { return _gameScene; }

}  // namespace yaadv
