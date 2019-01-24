#ifndef CAM_FREE_INPUT_COMPONENT_H
#define CAM_FREE_INPUT_COMPONENT_H

#include "CamInputComponent.h"

namespace PM3D
{
    class CamFreeInputComponent : public CamInputComponent
    {
        static constexpr float ANGLE_DIRECTION = DirectX::XM_PI / 2.f;
    public:
        CamFreeInputComponent(DirectX::XMVECTOR&& position, DirectX::XMVECTOR&& quaternion);
        CamFreeInputComponent() = delete;
        ~CamFreeInputComponent() = default;

        virtual void Handle(const float tempsEcoule) override;
    };
}
#endif // !CAMERA_FREE_INPUT_COMPONENT_H