#ifndef SCENE_PHYSIQUE_H
#define SCENE_PHYSIQUE_H
#include "../src/Component/PhysicTriggerComponent.h"

namespace PM3D {

   class ScenePhysique {

   public:

		enum Filter_Group {
			TERRAIN = (1 << 0),
			MUR = (1 << 1),
			VEHICULE = (1 << 2),
			ITEM = (1 << 3),
			OURS = (1 << 4),
			TIME = (1 << 5),
			BONUS = (1 << 6),
			ARBRE = (1 <<7 )
		};

		class PhysicTriggerComponent;
      physx::PxDefaultAllocator gAllocator{};
      physx::PxDefaultErrorCallback gErrorCallback{};
		
      physx::PxFoundation* gFoundation = NULL;
      physx::PxPvd* gPvd = NULL;
      physx::PxPhysics* gPhysics = NULL;
      physx::PxDefaultCpuDispatcher*	gDispatcher = NULL;
      physx::PxScene* gScene = NULL;
      physx::PxMaterial* gMaterial = NULL;

      physx::PxCooking* mCooking = NULL;
		vector<int> deleteTrigger ;
      ScenePhysique() = default;
		
		

		void Init(); // Permet d'initialiser toutes les variables de la scène physique

   };

} // namespace PM3D


#endif