#ifndef GAMESYSTEM_H
#define GAMESYSTEM_H

#include <list>
#include "ScriptReader/CharacterManager.h"
#include "cocos2d.h"

#define SYSTEMVOLUME "SystemVolume"
#define MUSICVOLUME "MusicVolume"
#define SOUNDVOLUME "SoundVolume"
#define TEXTSPEED "TextSpeed"
#define AUTOSPEED "AutoSpeed"
#define ISSKIPALL "IsSkipAll"

USING_NS_CC;

namespace yaadv {

struct fgCharacter {
  std::string name;
  std::string face;
  int number;
  fgCharacter(){};
  fgCharacter(std::string name, std::string face, int number)
      : name(name), face(face), number(number){};
};

struct GameData {
  fgCharacter fgCharacters[3];
  std::string backgroundKey;
  std::string bgmKey;
  std::string soundKey;
  int characterCount;
  std::string currentSignName;
  int currentCommandIndex;
  std::string currentName;
  std::string currentText;
  int optionsNumber;
  std::map<std::string, std::string> options;
};

struct GameSaveData {
  std::string imageFile;
  std::string date;
  std::string text;
  GameSaveData() {
    imageFile = "";
    date = "";
    text = "";
  }
};

class GameSystem {
  static GameSystem *_instance;

  float _systemVolume;
  float _musicVolume;
  float _soundVolume;

  float _textSpeed;
  float _autoSpeed;

  std::map<std::string, int> *_savedata;
  GameData *_gameSceneInfo;
  GameSaveData *_savedataList;
  bool _isLoadSuccess;
  RenderTexture *_screenShoot;

  Scene *_gameScene;

 public:
  GameSystem();
  ~GameSystem();
  static GameSystem *getInstance();
  static void destoryInstance();
  void setDefault();

  void setSystemVolume(float value);
  void setMusicVolume(float value);
  void setSoundVolume(float value);
  void setTextSpeed(float value);
  void setAutoSpeed(float value);
  void setHaveRead(const std::string &key, int value);
  void setIsSkipAll(bool value);

  float getSystemVolume();
  float getMusicVolume();
  float getSoundVolume();
  float getTextSpeed();
  float getAutoSpeed();
  int getHaveRead(const std::string &key);
  bool getIsSkipAll();

  void setDataValue(std::string &key, int value);

  int getDataValue(std::string &key);

  bool isDataValueExists(const std::string &key);

  void setData(std::map<std::string, int> *map = nullptr);

  std::string dataValueToString();

  void stringToDataValue(const std::string &dataValueStr);

  void setSavedata(int i, bool value);
  bool getSavedata(int i);

  GameData *getGameSceneInfo();
  void setGameSceneInfo(GameData *gameData);
  bool saveGameSceneInfo(int i);
  bool loadGameSceneInfo(int i);

  void setIsLoadSuccess(bool value);
  bool getIsLoadSuccess();

  void initGameSavedataList();
  GameSaveData *getGameSavedata(int i);
  bool updateGameSavedata(int i);
  bool deleteGameSavedata(int i);
  bool loadGameSavedata(int i);

  bool createSavedata();

  void setScreenShoot(RenderTexture *render);
  RenderTexture *getScreenShoot();

  void setGameScene(Scene *scene);
  Scene *getGameScene();

//  float getPosition(CMPositionType pt);
};
};  // namespace yaadv

#endif
