
#include "stdafx.h"
#include "EndMenu.h"
#include "MoteurWindows.h"
#include "GameManager.h"

namespace PM3D
{
    void EndMenu::Init(CDispositifD3D11* pDispositif, std::unique_ptr<CAfficheurSprite>& spriteDisplay)
    {
        const Gdiplus::FontFamily oFamily(L"Arial", nullptr);

        pPolice = std::make_unique<Gdiplus::Font>(&oFamily, 25.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

        result = std::make_unique<CAfficheurTexte>(pDispositif, 350, 30, pPolice.get(), Gdiplus::Color(255, 235, 0, 0)); //black
        auto resultSize = result->getSize();
        spriteDisplay->AjouterSpriteTexte(
            result->GetTextureView(),
            { pDispositif->GetLargeur() * 0.5f - static_cast<float>(resultSize.x) * 0.5f, pDispositif->GetHauteur() * 0.5f - static_cast<float>(resultSize.y) * 0.5f }); //middle

        score = std::make_unique<CAfficheurTexte>(pDispositif, 350, 30, pPolice.get(), Gdiplus::Color(255, 235, 0, 0)); //black
        auto scoreSize = score->getSize();
        spriteDisplay->AjouterSpriteTexte(
            score->GetTextureView(),
            { pDispositif->GetLargeur() * 0.5f - static_cast<float>(scoreSize.x) * 0.5f, pDispositif->GetHauteur() * 0.55f - static_cast<float>(scoreSize.y) * 0.5f }); //middle

        //spriteDisplay->AjouterSprite(L"../ressources/textures/MainMenu.dds", 0, pDispositif->GetHauteur(), pDispositif->GetLargeur(), pDispositif->GetHauteur());
    }

    void EndMenu::Update(const float tempsEcoule, GameResult gameResult, std::chrono::milliseconds remainingTime)
    {

        if (gameResult == GameResult::WIN) {
            result->Ecrire(L"   Bon jeu l'ami! T'assures!");

            if (lastTime.empty()) {
                std::chrono::seconds remainingTimeSecondes = std::chrono::duration_cast<std::chrono::seconds>(remainingTime);
                std::chrono::minutes remainingTimeMinutes = std::chrono::duration_cast<std::chrono::minutes>(remainingTime);

                auto secCount = remainingTimeSecondes.count() - static_cast<long long>(remainingTimeMinutes.count() * 60);
                auto sec = std::to_string(secCount);

                auto min = std::to_string(remainingTimeMinutes.count());
                lastTime = L"Rêve complété en " + std::wstring(min.begin(), min.end()) + L":" + std::wstring(sec.begin(), sec.end()) + L"s !!!";
            }

            score->Ecrire(lastTime);
        }
        else if (gameResult == GameResult::LOSE) {
            result->Ecrire(L"Wow, t'as vraiment perdu...");
            std::wstring message = CMoteurWindows::GetInstance().getGameManager().getMessageDefaite();
            score->Ecrire(std::wstring(message.begin(), message.end()));
        }
    }

} // namespace PM3D
