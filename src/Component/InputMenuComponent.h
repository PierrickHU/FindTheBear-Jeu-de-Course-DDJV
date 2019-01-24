#ifndef INPUT_MENU_COMPONENT_H
#define INPUT_MENU_COMPONENT_H

#include "InputComponent.h"
#include "AfficheurSprite.h"



namespace PM3D
{
    struct InputMenuComponent
    {
        virtual void Handle(const float, CAfficheurSprite*);

        InputMenuComponent(std::vector<DirectX::XMFLOAT2>&&);
        virtual ~InputMenuComponent() = default;

    private:
        bool currentPressedState = false;

        unsigned int currentIndex;
        std::vector<DirectX::XMFLOAT2> position;
    };
}
#endif