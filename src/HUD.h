#ifndef HUD_H
#define HUD_H

#include "Formes/CGameObject.h"
#include "AfficheurTexte.h"
#include "AfficheurSprite.h"
#include "dispositifD3D11.h"

#include <memory>
#include <string>
#include <chrono>

namespace PM3D
 {
    using ZoneTextPtr = std::unique_ptr<CAfficheurTexte>;
    using FontPtr = std::unique_ptr<Gdiplus::Font>;

    class HUD
    {
        const std::wstring SPEED_PREFIX = L"Vitesse: ";
        const std::wstring SPEED_SUFFIX = L" cm/s";

        ZoneTextPtr speedText;
        FontPtr pPoliceSpeed;

        ZoneTextPtr remainingTimeText;
        FontPtr pPoliceTime;

        ZoneTextPtr scoreText;
        ZoneTextPtr timeRemainingNightmare;

    public:
        HUD() = default;
        void Init(CDispositifD3D11*, std::unique_ptr<CAfficheurSprite>&);
        ~HUD() = default;

        void Update(const int speedValue, const std::chrono::milliseconds deltaTime, const std::chrono::milliseconds timeBeforeNightmare, std::pair<unsigned int, unsigned int> oursResult);
    };
}
#endif