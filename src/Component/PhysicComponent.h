#ifndef PHYSIC_COMPONENT_H
#define PHYSIC_COMPONENT_H

#include "Formes/CGameObject.h"

namespace PM3D
{
    class CTerrain;
    class ScenePhysique;
    struct PhysicComponent
    {
        physx::PxShape* shape = NULL;
        physx::PxRigidDynamic* rigidbody = NULL;
        DirectX::XMVECTOR boundingBox;

        PhysicComponent() = default;
        ~PhysicComponent() = default;
    };
}
#endif