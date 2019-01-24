#ifndef PHYSIC_DYNAMIC_COMPONENT_H
#define PHYSIC_DYNAMIC_COMPONENT_H

#include "Formes/CGameObject.h"
#include "Formes/Orientation.h"
#include "PhysicComponent.h"

#include <cmath>

namespace PM3D
{
    class ScenePhysique;
    class CTerrain;
    struct PhysicDynamicComponent : PhysicComponent
    {
        DirectX::XMVECTOR vitesseLineaire{ 0.0f, 0.0f, 0.0f };
        DirectX::XMVECTOR vitesseAngulaire{ 0.0f, 0.0f, 0.0f };

        void UpdateSimulation(Orientation orientation, const CTerrain& terrain, ScenePhysique& sp);
        void UpdateFromSimulation(CGameObject* obj);
        int GetVitesse();

        PhysicDynamicComponent() = default;
        ~PhysicDynamicComponent() = default;
    };
}
#endif