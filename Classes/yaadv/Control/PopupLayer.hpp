#ifndef PopupLayer_hpp
#define PopupLayer_hpp

#include "cocos2d.h"
#include "extensions/cocos-ext.h"

USING_NS_CC;

#define DEFAULT_TEXT_FONT "arial"
#define DEFAULT_TEXT_SIZE 20

namespace yaadv {
struct PopupButton {
  MenuItem *button;
  enum ButtonType { FONT, IMAGE } buttonType;
};

class PopupLayer : public Node {
  Sprite *_backgroundSprite;
  LayerColor *_backgroundLayer;

  int _buttonCounter;

  int _buttonPositionY;

  Menu *_buttonMenu;

  Label *_popupText;
  int _popupTextSize;
  float _popupTextPositionX;
  float _popupTextPositionY;
  std::string _popupTextFont;
  std::vector<PopupButton *> *_buttonList;

  void arrange();

  PopupLayer(const std::string &backgroundImage);

 public:
  ~PopupLayer();

  static PopupLayer *create(const std::string &backgroundImage);

  void addButton(const std::string &normalImage, std::function<void(Ref *)> &callback);

  void addButton(const std::string &normalImage, const std::string &selectedImage,
                 std::function<void(Ref *)> &callback);

  void addButton(const std::string &normalImage, const std::string &touchImage, const std::string &selectedImage,
                 std::function<void(Ref *)> &callback);

  void addLabelButton(const std::string &buttonText, const ccMenuCallback &callback);

  void setString(const std::string &text);
};
};  // namespace yaadv

#endif /* PopuLayer_hpp */
