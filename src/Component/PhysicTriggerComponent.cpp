#include "stdafx.h"
#include "PhysicTriggerComponent.h"
#include "PxPhysicsAPI.h"
#include <DirectXMath.h>

using namespace physx;
using namespace DirectX;

#undef min
#undef max

namespace PM3D
{
	
	PhysicTriggerComponent::PhysicTriggerComponent(CGameObject* obj, ScenePhysique& scenePhysique, int index) {
        XMFLOAT3 tailleBox = obj->orientation.getScale();
		shape = scenePhysique.gPhysics->createShape(physx::PxBoxGeometry{ tailleBox.x, tailleBox.x, tailleBox.x }, *(scenePhysique.gMaterial), true);
		physx::PxTransform t{ obj->orientation.getPositionX(), obj->orientation.getPositionY(), obj->orientation.getPositionZ() };
		physx::PxFilterData filterData;
		filterData.word0 = ScenePhysique::Filter_Group::ITEM;
		//filterData.word1 = ScenePhysique::Filter_Group::1;
		filterData.word3 = index;
		shape->setQueryFilterData(filterData);
		shape->setSimulationFilterData(filterData);
		rigidbody = scenePhysique.gPhysics->createRigidDynamic(t);
		//rigidbody->setAngularDamping(0.5f);
		//rigidbody->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		rigidbody->attachShape(*shape);
		rigidbody->setName("Item");
        rigidbody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		//rigidbody->setRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X | physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z);
		
		physx::PxRigidBodyExt::updateMassAndInertia(*rigidbody, 10.f);
		scenePhysique.gScene->addActor(*rigidbody);
	}

	

	

}