#include "stdafx.h"
#include "HUD.h"
#include "MoteurWindows.h"
#include "CScene.h"
#include "GameManager.h"
#include <sstream>
#include <chrono>
using namespace std;
using namespace std::chrono;

namespace PM3D
{
    void HUD::Init(CDispositifD3D11* pDispositif, std::unique_ptr<CAfficheurSprite>& spriteDisplay)
    {
        const Gdiplus::FontFamily oFamily(L"Arial", nullptr);

        pPoliceSpeed = std::make_unique<Gdiplus::Font>(&oFamily, 16.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        speedText = std::make_unique<CAfficheurTexte>(pDispositif, 200, 20, pPoliceSpeed.get(), Gdiplus::Color(255,255,0,0)); //red
        auto textSize = speedText->getSize();
        spriteDisplay->AjouterSpriteTexte(
            speedText->GetTextureView(),
            { pDispositif->GetLargeur() - static_cast<float>(textSize.x), pDispositif->GetHauteur() - static_cast<float>(textSize.y) }); //bottom right corner-ish

        pPoliceTime = std::make_unique<Gdiplus::Font>(&oFamily, 25.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        remainingTimeText = std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, pPoliceTime.get(), Gdiplus::Color(255, 255, 255, 0)); //yellow
        textSize = remainingTimeText->getSize();
        spriteDisplay->AjouterSpriteTexte(
            remainingTimeText->GetTextureView(),
            { pDispositif->GetLargeur() * 0.5f - static_cast<float>(textSize.x) * 0.5f, 5 + static_cast<float>(textSize.y) }); //top middle

        scoreText = std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, pPoliceTime.get(), Gdiplus::Color(255, 255, 255, 0)); //yellow
        textSize = scoreText->getSize();
        spriteDisplay->AjouterSpriteTexte(
            scoreText->GetTextureView(),
            { 5.f, 5 + static_cast<float>(textSize.y) }); //top left


        timeRemainingNightmare = std::make_unique<CAfficheurTexte>(pDispositif, 150, 100, pPoliceSpeed.get(), Gdiplus::Color(255, 255, 0, 0)); //yellow
        textSize = timeRemainingNightmare->getSize();
        spriteDisplay->AjouterSpriteTexte(
            timeRemainingNightmare->GetTextureView(),
            { pDispositif->GetLargeur() - static_cast<float>(textSize.x), 15 + static_cast<float>(textSize.y) }); //top left       
    }

    void HUD::Update(const int speedValue, const std::chrono::milliseconds deltaTime, const std::chrono::milliseconds timeBeforeNightmare, std::pair<unsigned int, unsigned int> oursResult)
    {
        //speed
        auto speedTextValue = std::to_string(speedValue);
        speedText->Ecrire(SPEED_PREFIX + std::wstring(speedTextValue.begin(), speedTextValue.end()) + SPEED_SUFFIX);

        //time remaining
        seconds remainingTimeSecondes = duration_cast<seconds>(deltaTime);
        minutes remainingTimeMinutes = duration_cast<minutes>(deltaTime);

        auto min = std::to_string(remainingTimeMinutes.count());
        auto secCount = remainingTimeSecondes.count() - static_cast<long long>(remainingTimeMinutes.count() * 60);
        auto sec = std::to_string(secCount);
        if (secCount < 10 && secCount >= 0) {
            sec = "0" + sec;
        }

        remainingTimeText->Ecrire(std::wstring(min.begin(), min.end()) + L" : " + std::wstring(sec.begin(), sec.end()));      

        //score
        scoreText->Ecrire(std::to_wstring(oursResult.first) + L" / " + std::to_wstring(oursResult.second));

        //time nightmare
        if (timeBeforeNightmare.count() > 0)
        {
            remainingTimeSecondes = duration_cast<seconds>(timeBeforeNightmare);
            remainingTimeMinutes = duration_cast<minutes>(timeBeforeNightmare);
            min = std::to_string(remainingTimeMinutes.count());
            secCount = remainingTimeSecondes.count() - static_cast<long long>(remainingTimeMinutes.count() * 60);
            sec = std::to_string(secCount);
            if (secCount < 10 && secCount >= 0) {
                sec = "0" + sec;
            }
            timeRemainingNightmare->Ecrire(L"Reve dans " + std::wstring(min.begin(), min.end()) + L" : " + std::wstring(sec.begin(), sec.end()));
        }
        else {
            timeRemainingNightmare->Ecrire(L"");
        }
    }
}