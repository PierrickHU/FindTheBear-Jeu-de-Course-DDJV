
#include "stdafx.h"
#include "OptionsMenu.h"


namespace PM3D
{
    void OptionsMenu::Init(CDispositifD3D11* pDispositif, std::unique_ptr<CAfficheurSprite>& spriteDisplay)
    {
        display = spriteDisplay.get();
        const Gdiplus::FontFamily oFamily(L"Arial", nullptr);

        DirectX::XMFLOAT2 pos = { pDispositif->GetLargeur() *0.1f, pDispositif->GetHauteur() * 0.9f };
        pPoliceTitle = std::make_unique<Gdiplus::Font>(&oFamily, 35.0f, Gdiplus::FontStyleBoldItalic, Gdiplus::UnitPixel);
        pPolice = std::make_unique<Gdiplus::Font>(&oFamily, 25.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

        selector = std::make_unique<CAfficheurTexte>(pDispositif, 50, 15, pPolice.get(), Gdiplus::Color(255, 255, 255, 0)); //yellow
        auto selectorSize = selector->getSize();
        std::vector<DirectX::XMFLOAT2> inputPosition = {
              { pDispositif->GetLargeur() * 0.395f - static_cast<float>(selectorSize.x) * 0.5f, pDispositif->GetHauteur() * 0.54f - static_cast<float>(selectorSize.y) * 0.5f }, //left
              { pDispositif->GetLargeur() * 0.5f - static_cast<float>(selectorSize.x) * 0.5f, pDispositif->GetHauteur() * 0.54f - static_cast<float>(selectorSize.y) * 0.5f }, //middle
              { pDispositif->GetLargeur() * 0.605f - static_cast<float>(selectorSize.x) * 0.5f, pDispositif->GetHauteur() * 0.54f - static_cast<float>(selectorSize.y) * 0.5f }, //right
              { pDispositif->GetLargeur() * 0.5f - static_cast<float>(selectorSize.x) * 0.5f, pDispositif->GetHauteur() * 0.66f - static_cast<float>(selectorSize.y) * 0.5f }  //bot
        };
        display->AjouterSpriteTexte(selector->GetTextureView(), inputPosition[3]);

        title = std::make_unique<CAfficheurTexte>(pDispositif, 150, 40, pPoliceTitle.get(), Gdiplus::Color(255, 255, 255, 255)); //white
        auto textSize = title->getSize();
        display->AjouterSpriteTexte(
            title->GetTextureView(),
            { pDispositif->GetLargeur() * 0.5f - static_cast<float>(textSize.x) * 0.5f, pDispositif->GetHauteur() * 0.35f - static_cast<float>(textSize.y) * 0.5f }); //middle

        difficulty = std::make_unique<CAfficheurTexte>(pDispositif, 310, 60, pPolice.get(), Gdiplus::Color(255, 255, 255, 255)); //white
        auto difficultySize = difficulty->getSize();
        display->AjouterSpriteTexte(
            difficulty->GetTextureView(),
            { pDispositif->GetLargeur() * 0.5f - static_cast<float>(difficultySize.x) * 0.5f, pDispositif->GetHauteur() * 0.55f - static_cast<float>(difficultySize.y) * 0.5f }); //middle

        backToGame = std::make_unique<CAfficheurTexte>(pDispositif, 175, 30, pPolice.get(), Gdiplus::Color(255, 255, 255, 255)); //white
        auto backToGameSize = backToGame->getSize();
        display->AjouterSpriteTexte(
            backToGame->GetTextureView(),
            { pDispositif->GetLargeur() * 0.5f - static_cast<float>(backToGameSize.x) * 0.5f, pDispositif->GetHauteur() * 0.65f - static_cast<float>(backToGameSize.y) * 0.5f }); //middle

        input = std::make_unique<InputMenuComponent>(std::move(inputPosition));
    }

    void OptionsMenu::Update(const float tempsEcoule)
    {

        input->Handle(tempsEcoule, display);

        title->Ecrire(L"Options");
        difficulty->Ecrire(L"             Difficulté:\n Facile    Moyen   Difficile"); // oui je sais, pas le temps de niaiser
        backToGame->Ecrire(L"Retour au jeu");
        selector->Ecrire(L"****");



    }
 

} // namespace PM3D
