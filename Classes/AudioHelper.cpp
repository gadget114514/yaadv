#include "AudioHelper.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

#include <Digitalv.h>
#include <mmsystem.h>
#include <tchar.h>
#include <cstdlib>
#include <map>
#include <string>
#include "cocos2d\cocos\platform\CCStdC.h"

#include "cocos2d\cocos\platform\CCFileUtils.h"

using namespace CocosDenshion;
using namespace std;

#define WIN_CLASS_NAME "CocosDenshionCallbackWnd"
#define BREAK_IF(cond) \
  if (cond) break;

static HINSTANCE s_hInstance;
static MCIERROR s_mciError;

LRESULT WINAPI _SoundPlayProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

class MMPlayer {
  void _SendGenericCommand(int nCommand, DWORD_PTR param1 = 0, DWORD_PTR parma2 = 0) {
    if (!_dev) {
      return;
    }
    mciSendCommand(_dev, nCommand, param1, parma2);
  }

  friend LRESULT WINAPI _SoundPlayProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    MMPlayer *pPlayer = NULL;
    if (MM_MCINOTIFY == Msg && MCI_NOTIFY_SUCCESSFUL == wParam &&
        (pPlayer = (MMPlayer *)GetWindowLongPtr(hWnd, GWLP_USERDATA))) {
      if (pPlayer->_times) {
        --pPlayer->_times;
      }

      if (pPlayer->_times) {
        mciSendCommand(lParam, MCI_SEEK, MCI_SEEK_TO_START, 0);

        MCI_PLAY_PARMS mciPlay = {0};
        mciPlay.dwCallback = reinterpret_cast<DWORD_PTR>(hWnd);
        mciSendCommand(lParam, MCI_PLAY, MCI_NOTIFY, reinterpret_cast<DWORD_PTR>(&mciPlay));
      } else {
        pPlayer->_playing = false;
      }
      return 0;
    }
    return DefWindowProc(hWnd, Msg, wParam, lParam);
  }

  HWND _wnd;
  MCIDEVICEID _dev;
  UINT _soundID;
  UINT _times;
  bool _playing;
  std::string strExt;

 public:
  MMPlayer() : _wnd(NULL), _dev(0L), _soundID(0), _times(0), _playing(false), strExt("") {
    if (!s_hInstance) {
      s_hInstance = GetModuleHandle(NULL);

      WNDCLASS wc;

      wc.style = 0;
      wc.lpfnWndProc = _SoundPlayProc;
      wc.cbClsExtra = 0;
      wc.cbWndExtra = 0;
      wc.hInstance = s_hInstance;
      wc.hIcon = 0;
      wc.hCursor = LoadCursor(NULL, IDC_ARROW);
      wc.hbrBackground = NULL;
      wc.lpszMenuName = NULL;
      wc.lpszClassName = _T(WIN_CLASS_NAME);

      if (!RegisterClass(&wc) && 1410 != GetLastError()) {
        return;
      }
    }

    _wnd = CreateWindowEx(WS_EX_APPWINDOW, _T(WIN_CLASS_NAME), NULL, WS_POPUPWINDOW, 0, 0, 0, 0, NULL, NULL,
                          s_hInstance, NULL);
    if (_wnd) {
      SetWindowLongPtr(_wnd, GWLP_USERDATA, (LONG_PTR) this);
    }
  }

  ~MMPlayer() {
    Close();
    DestroyWindow(_wnd);
  }

  UINT GetSoundID() { return _soundID; }
  boolean IsPlaying() { return _playing; }

  void Rewind() {
    if (!_dev) {
      return;
    }
    mciSendCommand(_dev, MCI_SEEK, MCI_SEEK_TO_START, 0);

    MCI_PLAY_PARMS mciPlay = {0};
    mciPlay.dwCallback = reinterpret_cast<DWORD_PTR>(_wnd);
    _playing = mciSendCommand(_dev, MCI_PLAY, MCI_NOTIFY, reinterpret_cast<DWORD_PTR>(&mciPlay)) ? false : true;
  }

  void Stop() {
    _SendGenericCommand(MCI_STOP);
    _playing = false;
    _times = 0;
  }

  void Resume() {
    if (strExt == ".mid") {
      MCI_STATUS_PARMS mciStatusParms;
      MCI_PLAY_PARMS mciPlayParms;
      mciStatusParms.dwItem = MCI_STATUS_POSITION;
      _SendGenericCommand(MCI_STATUS, MCI_STATUS_ITEM, reinterpret_cast<DWORD_PTR>(&mciStatusParms));
      mciPlayParms.dwFrom = mciStatusParms.dwReturn;
      _SendGenericCommand(MCI_PLAY, MCI_FROM, reinterpret_cast<DWORD_PTR>(&mciPlayParms));
    } else {
      _SendGenericCommand(MCI_RESUME);
      _playing = true;
    }
  }

  void Close() {
    if (_playing) {
      Stop();
    }
    if (_dev) {
      _SendGenericCommand(MCI_CLOSE);
    }
    _dev = 0;
    _playing = false;
  }

  void Pause() {
    _SendGenericCommand(MCI_PAUSE);
    _playing = false;
  }

  void Play(UINT uTimes) {
    if (!_dev) {
      return;
    }
    MCI_PLAY_PARMS mciPlay = {0};
    mciPlay.dwCallback = reinterpret_cast<DWORD_PTR>(_wnd);
    s_mciError = mciSendCommand(_dev, MCI_PLAY, MCI_FROM | MCI_NOTIFY, reinterpret_cast<DWORD_PTR>(&mciPlay));
    if (!s_mciError) {
      _playing = true;
      _times = uTimes;
    }
  }

  void Open(const char *pFileName, UINT uId) {
    do {
      BREAK_IF(!pFileName || !_wnd);
      int nLen = (int)strlen(pFileName);
      BREAK_IF(!nLen);

      strExt = cocos2d::FileUtils::getInstance()->getFileExtension(pFileName);

      Close();

      MCI_OPEN_PARMS mciOpen = {0};
      MCIERROR mciError;
      mciOpen.lpstrDeviceType = (LPCTSTR)MCI_ALL_DEVICE_ID;
      WCHAR *fileNameWideChar = new WCHAR[nLen + 1];
      BREAK_IF(!fileNameWideChar);
      MultiByteToWideChar(CP_ACP, 0, pFileName, nLen + 1, fileNameWideChar, nLen + 1);
      mciOpen.lpstrElementName = fileNameWideChar;

      mciError = mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT, reinterpret_cast<DWORD_PTR>(&mciOpen));
      CC_SAFE_DELETE_ARRAY(mciOpen.lpstrElementName);
      BREAK_IF(mciError);

      _dev = mciOpen.wDeviceID;
      _soundID = uId;
      _playing = false;
    } while (0);
  }

  void SetVolume(UINT volume) {
    if (!_dev) return;
    MCI_DGV_SETAUDIO_PARMS mciParams = {0};
    mciParams.dwItem = MCI_DGV_SETAUDIO_VOLUME;
    mciParams.dwValue = volume;
    mciSendCommand(_dev, MCI_SETAUDIO, MCI_DGV_SETAUDIO_ITEM | MCI_DGV_SETAUDIO_VALUE, (DWORD)&mciParams);
  }

  UINT GetVolume() const {
    if (!_dev) return 0;
    MCI_STATUS_PARMS mciParams = {0};
    mciParams.dwItem = MCI_DGV_STATUS_VOLUME;
    mciSendCommand(_dev, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mciParams);
    return mciParams.dwReturn;
  }
};

typedef map<unsigned int, MMPlayer *> EffectList;
typedef pair<unsigned int, MMPlayer *> Effect;

static char s_szRootPath[MAX_PATH];
static DWORD s_dwRootLen;
static char s_szFullPath[MAX_PATH];

static std::string _FullPath(const char *szPath);
static unsigned int _Hash(const char *key);

static EffectList &sharedList() {
  static EffectList s_List;
  return s_List;
}

static MMPlayer &sharedMusic() {
  static MMPlayer s_Music;
  return s_Music;
}

class CustomAudioEngine : public CocosDenshion::SimpleAudioEngine {
 private:
  static CustomAudioEngine *_instance;
  CustomAudioEngine() {
    m_effectsVolume = 1.0f;
    m_musicVolume = 1.0f;
  }
  ~CustomAudioEngine() {}

  float m_effectsVolume;
  float m_musicVolume;

 public:
  static CustomAudioEngine *getInstance() {
    static CustomAudioEngine s_SharedEngine;
    return &s_SharedEngine;
  }

  void end() {
    sharedMusic().Close();

    EffectList::iterator p = sharedList().begin();
    while (p != sharedList().end()) {
      delete p->second;
      p->second = nullptr;
      p++;
    }
    sharedList().clear();
    return;
  }

  void playBackgroundMusic(const char *pszFilePath, bool bLoop) {
    if (!pszFilePath) {
      return;
    }

    sharedMusic().Open(_FullPath(pszFilePath).c_str(), _Hash(pszFilePath));
    sharedMusic().Play((bLoop) ? -1 : 1);
    sharedMusic().SetVolume((UINT)(m_musicVolume * 1000.0));
  }

  void stopBackgroundMusic(bool bReleaseData) {
    if (bReleaseData) {
      sharedMusic().Close();
    } else {
      sharedMusic().Stop();
    }
  }

  void pauseBackgroundMusic() { sharedMusic().Pause(); }

  void resumeBackgroundMusic() { sharedMusic().Resume(); }

  void rewindBackgroundMusic() { sharedMusic().Rewind(); }

  bool willPlayBackgroundMusic() { return false; }

  bool isBackgroundMusicPlaying() { return sharedMusic().IsPlaying(); }

  unsigned int playEffect(const char *pszFilePath, bool bLoop, float pitch, float pan, float gain) {
    unsigned int nRet = _Hash(pszFilePath);

    preloadEffect(pszFilePath);

    EffectList::iterator p = sharedList().find(nRet);
    if (p != sharedList().end()) {
      p->second->Play((bLoop) ? -1 : 1);
      p->second->SetVolume((UINT)(m_effectsVolume * 1000.0));
    }

    return nRet;
  }

  void stopEffect(unsigned int nSoundId) {
    EffectList::iterator p = sharedList().find(nSoundId);
    if (p != sharedList().end()) {
      p->second->Stop();
    }
  }

  void preloadEffect(const char *pszFilePath) {
    int nRet = 0;
    do {
      BREAK_IF(!pszFilePath);

      nRet = _Hash(pszFilePath);

      BREAK_IF(sharedList().end() != sharedList().find(nRet));

      sharedList().insert(Effect(nRet, new MMPlayer()));
      MMPlayer *pPlayer = sharedList()[nRet];
      pPlayer->Open(_FullPath(pszFilePath).c_str(), nRet);

      BREAK_IF(nRet == pPlayer->GetSoundID());

      delete pPlayer;
      sharedList().erase(nRet);
      nRet = 0;
    } while (0);
  }

  void pauseEffect(unsigned int nSoundId) {
    EffectList::iterator p = sharedList().find(nSoundId);
    if (p != sharedList().end()) {
      p->second->Pause();
    }
  }

  void pauseAllEffects() {
    EffectList::iterator iter;
    for (iter = sharedList().begin(); iter != sharedList().end(); iter++) {
      iter->second->Pause();
    }
  }

  void resumeEffect(unsigned int nSoundId) {
    EffectList::iterator p = sharedList().find(nSoundId);
    if (p != sharedList().end()) {
      p->second->Resume();
    }
  }

  void resumeAllEffects() {
    EffectList::iterator iter;
    for (iter = sharedList().begin(); iter != sharedList().end(); iter++) {
      iter->second->Resume();
    }
  }

  void stopAllEffects() {
    EffectList::iterator iter;
    for (iter = sharedList().begin(); iter != sharedList().end(); iter++) {
      iter->second->Stop();
    }
  }

  void preloadBackgroundMusic(const char *pszFilePath) {}

  void unloadEffect(const char *pszFilePath) {
    unsigned int nID = _Hash(pszFilePath);

    EffectList::iterator p = sharedList().find(nID);
    if (p != sharedList().end()) {
      delete p->second;
      p->second = nullptr;
      sharedList().erase(nID);
    }
  }

  float getBackgroundMusicVolume() { return sharedMusic().GetVolume() / 1000.0f; }

  void setBackgroundMusicVolume(float volume) {
    log("setBackgroupMusicVolume: %.2f", volume);
    m_musicVolume = volume;
    sharedMusic().SetVolume((UINT)(m_musicVolume * 1000.0));
  }

  float getEffectsVolume() { return m_effectsVolume; }

  void setEffectsVolume(float volume) {
    m_effectsVolume = volume;
    EffectList::iterator iter;
    for (iter = sharedList().begin(); iter != sharedList().end(); iter++) {
      iter->second->SetVolume((UINT)(volume * 1000.0));
    }
  }

  static std::string _FullPath(const char *szPath) { return FileUtils::getInstance()->fullPathForFilename(szPath); }

  unsigned int _Hash(const char *key) {
    unsigned int len = strlen(key);
    const char *end = key + len;
    unsigned int hash;

    for (hash = 0; key < end; key++) {
      hash *= 16777619;
      hash ^= (unsigned int)(unsigned char)toupper(*key);
    }
    return (hash);
  }
};

CustomAudioEngine *CustomAudioEngine::_instance = nullptr;

CocosDenshion::SimpleAudioEngine *AudioHelper::getAudioEngine() { return CustomAudioEngine::getInstance(); }

#else
CocosDenshion::SimpleAudioEngine *AudioHelper::getAudioEngine() {
  return CocosDenshion::SimpleAudioEngine::getInstance();
}
#endif
