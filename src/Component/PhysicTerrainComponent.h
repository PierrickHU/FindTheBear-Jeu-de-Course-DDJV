#ifndef PHYSIC_TERRAIN__COMPONENT_H
#define PHYSIC_TERRAIN__COMPONENT_H

#include "PhysicComponent.h"
#include "ScenePhysique.h"
#include "Formes/CGameObject.h"

namespace PM3D
{
    struct PhysicTerrainComponent : PhysicComponent
    {
        PhysicTerrainComponent(CGameObject* obj, ScenePhysique& scenePhysique);
        ~PhysicTerrainComponent() = default;
    };
}
#endif