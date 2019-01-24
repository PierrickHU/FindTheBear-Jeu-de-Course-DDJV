#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "AfficheurSprite.h"
#include "AfficheurTexte.h"
#include "dispositifD3D11.h"

#include <memory>

namespace PM3D {
   using ZoneTextPtr = std::unique_ptr<CAfficheurTexte>;

   class MainMenu
   {
      std::unique_ptr<Gdiplus::Font> pPolice;
      ZoneTextPtr text;

      float accumulatedTime{};
      bool toggleText = true;
   public:
      MainMenu() = default;
      void Init(CDispositifD3D11*, std::unique_ptr<CAfficheurSprite>&);
      ~MainMenu() = default;

      void Update(const float);
   };

} // namespace PM3D


#endif