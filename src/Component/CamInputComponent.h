#ifndef CAM_INPUT_COMPONENT_H
#define CAM_INPUT_COMPONENT_H

#include "stdafx.h"
#include "Formes/Orientation.h"
#include "InputComponent.h"

namespace PM3D
{
    enum ControlInputType{ FREE, LEVEL,FPS };

    struct CamInputComponent : public InputComponent
    {
        DirectX::XMMATRIX m_MatView;
        static constexpr float VITESSE_DEPLACEMENT = 200.f;
        Orientation orientation;
        ControlInputType inputType;

        CamInputComponent(DirectX::XMVECTOR&& position, DirectX::XMVECTOR&& quaternion, ControlInputType controlType) :
            orientation{ std::move(position), std::move(quaternion) }, inputType{ controlType }
        {}
        CamInputComponent() = delete;
        virtual ~CamInputComponent() = default;
    };	 
}
#endif // !CAMERA_H