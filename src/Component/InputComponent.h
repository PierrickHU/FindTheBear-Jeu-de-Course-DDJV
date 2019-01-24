#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

#include "PhysicComponent.h"

namespace PM3D
{
    struct InputComponent
    {
        virtual void Handle(const float) = 0;

    protected:
        InputComponent() = default;
        virtual ~InputComponent() = default;
    };

    struct InputPhysicComponent
    {
        virtual void Handle(const float, CGameObject*) = 0;

    protected:
        InputPhysicComponent() = default;
        virtual ~InputPhysicComponent() = default;
    };
}
#endif