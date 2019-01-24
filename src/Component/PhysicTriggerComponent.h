#ifndef PHYSIC_TRIGGER_COMPONENT_H
#define PHYSIC_TRIGGER_COMPONENT_H

#include "Formes/CGameObject.h"
#include "PhysicComponent.h"

#include "ScenePhysique.h"


namespace PM3D
{
	class ScenePhysique;
	class CTerrain;
	struct PhysicTriggerComponent : PhysicComponent
	{
		
		PhysicTriggerComponent(CGameObject * obj, ScenePhysique & scenePhysique, int index);
		
		

		~PhysicTriggerComponent() = default;
	};

}
#endif