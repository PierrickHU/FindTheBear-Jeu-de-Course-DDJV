#ifndef OPTIONS_MENU_H
#define OPTIONS_MENU_H

#include "AfficheurSprite.h"
#include "AfficheurTexte.h"
#include "dispositifD3D11.h"
#include "Component/InputMenuComponent.h"

#include <memory>

namespace PM3D {
    using ZoneTextPtr = std::unique_ptr<CAfficheurTexte>;

    enum Selectable { DIFFICULTY_EASY, DIFFICULTY_MEDIUM_SAIGNANT, DIFFICULTY_HARD, BACK_TO_GAME };

   class OptionsMenu
   {
       std::unique_ptr<Gdiplus::Font> pPoliceTitle;
       std::unique_ptr<Gdiplus::Font> pPolice;
       ZoneTextPtr title;
       ZoneTextPtr difficulty;
       ZoneTextPtr backToGame;
       ZoneTextPtr selector;
       CAfficheurSprite* display;

       std::unique_ptr<InputMenuComponent> input;
 
   public:
       OptionsMenu() = default;
       void Init(CDispositifD3D11*, std::unique_ptr<CAfficheurSprite>&);
       ~OptionsMenu() = default;

       void Update(const float);
   };

} // namespace PM3D


#endif