#ifndef END_MENU_H
#define END_MENU_H

#include "AfficheurSprite.h"
#include "AfficheurTexte.h"
#include "dispositifD3D11.h"
#include "GameManager.h"

#include <memory>

namespace PM3D {
    using ZoneTextPtr = std::unique_ptr<CAfficheurTexte>;

   class EndMenu
   {
       std::unique_ptr<Gdiplus::Font> pPolice;
       ZoneTextPtr result;
       ZoneTextPtr score;
       std::wstring lastTime;

   public:
       EndMenu() = default;
       void Init(CDispositifD3D11*, std::unique_ptr<CAfficheurSprite>&);
       ~EndMenu() = default;

       void Update(const float, GameResult, std::chrono::milliseconds remainingTime);
   };

} // namespace PM3D


#endif