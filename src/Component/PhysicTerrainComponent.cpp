#include "stdafx.h"
#include "PhysicTerrainComponent.h"
#include "Formes/CTerrain.h"
using namespace physx;

namespace PM3D
{
    PhysicTerrainComponent::PhysicTerrainComponent(CGameObject* obj, ScenePhysique& scenePhysique) {
        auto terrain = dynamic_cast<CTerrain*>(obj);
        physx::PxHeightFieldSample* samples = (physx::PxHeightFieldSample*)malloc(sizeof(physx::PxHeightFieldSample) * (terrain->mesh.m_Sommets.size()));
        for (physx::PxU32 x = 0; x < terrain->m_Width; x++)
        {
            for (physx::PxU32 y = 0; y < terrain->m_Depth; y++) {
                auto index = x + y * terrain->m_Width;
                if (terrain->isInCentre(x, y)) {
                    samples[index].height = static_cast<physx::PxI16>(terrain->m_Hauteurs[y + x * terrain->m_Width]);
                    samples[index].clearTessFlag();
                    samples[index].materialIndex0 = 0;
                    samples[index].materialIndex1 = 0;

                // Si on est trop loin du centre, alors on est un trou !
                } else {
                    samples[index].height = 0;
                    samples[index].clearTessFlag();
                    samples[index].materialIndex0 = PxHeightFieldMaterial::eHOLE;
                    samples[index].materialIndex1 = PxHeightFieldMaterial::eHOLE;
                }
            }
        }

        physx::PxHeightFieldDesc hfDesc;
        hfDesc.format = physx::PxHeightFieldFormat::eS16_TM;
        hfDesc.nbColumns = terrain->m_Width;
        hfDesc.nbRows = terrain->m_Depth;
        hfDesc.samples.data = samples;
        hfDesc.samples.stride = sizeof(physx::PxHeightFieldSample);

        physx::PxHeightField* hf = scenePhysique.mCooking->createHeightField(hfDesc, scenePhysique.gPhysics->getPhysicsInsertionCallback());
        delete samples;

        physx::PxHeightFieldGeometry hfGeom(hf, physx::PxMeshGeometryFlags(), terrain->orientation.getScale().y, terrain->orientation.getScale().x, terrain->orientation.getScale().z);
        physx::PxTransform pose(physx::PxVec3(0.f, 0.f, 0.f), physx::PxQuat(physx::PxIdentity));
        physx::PxRigidActor* hfActor = scenePhysique.gPhysics->createRigidStatic(pose);
        assert(hf);//"PxPhysics::createRigidStatic returned NULL\n");

        // Puis on rajoute l'acteur dans physx !
        shape = physx::PxRigidActorExt::createExclusiveShape(*hfActor, hfGeom, *scenePhysique.gMaterial);
        assert(shape);//"PxRigidActor::createShape returned NULL\n");
        shape->setFlag(physx::PxShapeFlag::ePARTICLE_DRAIN, false);
        shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
        physx::PxFilterData filterData;
        filterData.word0 = ScenePhysique::Filter_Group::TERRAIN; // C'est le TerrainGroup !
        shape->setQueryFilterData(filterData);
        // rigidbody = hf; // On retient pas le rigidbody ... de toute facon il est pas cense bouger le terrain ! :D
        int nbActorsAvant = scenePhysique.gScene->getNbActors(physx::PxActorTypeFlag::eRIGID_STATIC);
        scenePhysique.gScene->addActor(*hfActor);
        int nbActorsApres = scenePhysique.gScene->getNbActors(physx::PxActorTypeFlag::eRIGID_STATIC);
        assert(nbActorsApres > nbActorsAvant);// "Notre acteur n'a pas ete ajoute !");

        //// on ajoute des murs virtuels pour ne pas sortir du terrain
        //physx::PxRigidStatic* planXPos = PxCreatePlane(*scenePhysique.gPhysics, physx::PxPlane(-1, 0, 0, terrain->orientation.getScale().x * (terrain->m_Width - 1)), *scenePhysique.gPhysics->createMaterial(0.f, 0.f, 0.5f));
        //physx::PxRigidStatic* planXNeg = PxCreatePlane(*scenePhysique.gPhysics, physx::PxPlane(1, 0, 0, 0), *scenePhysique.gPhysics->createMaterial(0.f, 0.f, 0.5f));
        //physx::PxRigidStatic* planZPos = PxCreatePlane(*scenePhysique.gPhysics, physx::PxPlane(0, 0, -1, terrain->orientation.getScale().z * (terrain->m_Depth - 1)), *scenePhysique.gPhysics->createMaterial(0.f, 0.f, 0.5f));
        //physx::PxRigidStatic* planZNeg = PxCreatePlane(*scenePhysique.gPhysics, physx::PxPlane(0, 0, 1, 0), *scenePhysique.gPhysics->createMaterial(0.f, 0.f, 0.5f));
        //scenePhysique.gScene->addActor(*planXPos);
        //scenePhysique.gScene->addActor(*planXNeg);
        //scenePhysique.gScene->addActor(*planZPos);
        //scenePhysique.gScene->addActor(*planZNeg);
    }
}