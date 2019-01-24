
#include "stdafx.h"
#include "Menu.h"


namespace PM3D
{
    void Menu::Init(CDispositifD3D11* pDispositif, std::unique_ptr<CAfficheurSprite>& spriteDisplay)
    {
        const Gdiplus::FontFamily oFamily(L"Arial", nullptr);

        pPolice = std::make_unique<Gdiplus::Font>(&oFamily, 25.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        text = std::make_unique<CAfficheurTexte>(pDispositif, 150, 30, pPolice.get(), Gdiplus::Color(255, 255, 0, 0)); //black
        auto textSize = text->getSize();
        spriteDisplay->AjouterSpriteTexte(
            text->GetTextureView(),
            { pDispositif->GetLargeur() * 0.5f - static_cast<float>(textSize.x) * 0.5f, pDispositif->GetHauteur() * 0.5f - static_cast<float>(textSize.y) * 0.5f }); //middle

        spriteDisplay->AjouterSprite(L"../ressources/MainMenu.dds", 0, pDispositif->GetHauteur(), pDispositif->GetLargeur(), pDispositif->GetHauteur());
    }

    void Menu::Update(const float tempsEcoule)
    {
        accumulatedTime += tempsEcoule;

        if (accumulatedTime > .5f) {
            toggleText = !toggleText;
            accumulatedTime = 0;
        }

        if(toggleText)
            text->Ecrire(L"Press Start");
        else
            text->Ecrire(L"");
    }
 

} // namespace PM3D
