#ifndef CAMERA_THIRD_PERSON_INPUT_COMPONENT_H
#define CAMERA_THIRD_PERSON_INPUT_COMPONENT_H

#include "CamInputComponent.h"

namespace PM3D
{
    class CamThirdPersonInputComponent : public CamInputComponent
    {
        static constexpr float VITESSE_ZOOM = 0.3f;

        static constexpr float DISTANCE_ANCRAGE_MAX = 8.f;
        static constexpr float DISTANCE_ANCRAGE_MIN = 5.f;
        static constexpr float DISTANCE_ANCRAGE_SOL_MIN = 1.f;
        static constexpr float PRECISION_ANGULAIRE = DirectX::XM_PIDIV4;

        Orientation* playerOrientation;
        DirectX::XMVECTOR oldPlayerPosition;
        float angle = DirectX::XM_PIDIV4 / 2.5f;
    public:	 
        CamThirdPersonInputComponent();
        ~CamThirdPersonInputComponent() = default;

        virtual void Handle(const float tempsEcoule) override;
    };
	 
}
#endif // !CAMERA_H