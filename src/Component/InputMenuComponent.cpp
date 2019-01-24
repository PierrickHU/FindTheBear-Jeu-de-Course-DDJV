
#include "stdafx.h"
#include "InputMenuComponent.h"
#include "MoteurWindows.h"

namespace PM3D
{
    InputMenuComponent::InputMenuComponent(std::vector<DirectX::XMFLOAT2>&& position)
        :position{position}, currentIndex{0}
    {
        assert(position.size());
    }


    void InputMenuComponent::Handle(const float elapsedTime, CAfficheurSprite* display)
    {
        auto& GestionnaireDeSaisie = CMoteurWindows::GetInstance().GetGestionnaireDeSaisie();

        auto move = GestionnaireDeSaisie.ToucheAppuyee(DIK_D) - GestionnaireDeSaisie.ToucheAppuyee(DIK_A);

        if (move != 0)
        {
            if (!currentPressedState)
            {
                if (!(currentIndex == 0 & move < 0))
                {
                    currentIndex = (currentIndex + move) % position.size();
                }
                else
                {
                    currentIndex = position.size() - 1;
                }
                display->MoveSprite(0, 50, 15, position[currentIndex]);
                currentPressedState = true;
            }
        }
        else
        {
            currentPressedState = false;
        }
    }
}