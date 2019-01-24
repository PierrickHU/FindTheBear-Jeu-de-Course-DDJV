#include "stdafx.h"
#include "CSimpleGameObject.h"
#include "MoteurWindows.h"

namespace PM3D
{
    CSimpleGameObject::CSimpleGameObject(Orientation& orientation, ModelType type, CDispositifD3D11* _pDispositif, ScenePhysique& scenePhysique)
        :CGameObject(orientation)
    {
        mesh = CMoteurWindows::GetInstance().GetRessourcesManager().GetModelMesh(type);
        mesh->shaderFilename = L"../ressources/shaders/MiniPhongSM.fx";
        mesh->Init(_pDispositif);
        //auto varMin = -0.3f;
        //auto varMax = 0.3f;
        //mesh->materials[0].Ambient = { mesh->materials[0].Ambient.x + RandomGenerateur::get().random(varMin, varMax) ,mesh->materials[0].Ambient.y + RandomGenerateur::get().random(varMin, varMax),mesh->materials[0].Ambient.z + RandomGenerateur::get().random(varMin, varMax), mesh->materials[0].Ambient.w };
        //mesh->materials[0].Diffuse = { mesh->materials[0].Diffuse.x + RandomGenerateur::get().random(varMin, varMax) ,mesh->materials[0].Diffuse.y + RandomGenerateur::get().random(varMin, varMax),mesh->materials[0].Diffuse.z + RandomGenerateur::get().random(varMin, varMax), mesh->materials[0].Diffuse.w };
        //mesh->materials[0].Specular = { mesh->materials[0].Specular.x + RandomGenerateur::get().random(varMin, varMax) ,mesh->materials[0].Specular.y + RandomGenerateur::get().random(varMin, varMax),mesh->materials[0].Specular.z + RandomGenerateur::get().random(varMin, varMax), mesh->materials[0].Specular.w };
        calculMatriceWorld();

		  XMFLOAT3 tailleBox = orientation.getScale();
		  shape = scenePhysique.gPhysics->createShape(physx::PxBoxGeometry{ tailleBox.x/7, tailleBox.y/3.5f, tailleBox.z/7 }, *(scenePhysique.gMaterial), true);
		  physx::PxTransform t{ orientation.getPositionX(), orientation.getPositionY()-30, orientation.getPositionZ() };
		  physx::PxFilterData filterData;
		  filterData.word0 = ScenePhysique::Filter_Group::ARBRE;
		  //filterData.word3 = index;
		  shape->setQueryFilterData(filterData);
		  shape->setSimulationFilterData(filterData);
		  rigidbody = scenePhysique.gPhysics->createRigidDynamic(t);
		  //rigidbody->setAngularDamping(0.5f);
		  //rigidbody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
		  rigidbody->attachShape(*shape);
		  //rigidbody->setName("Item");
		  rigidbody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
		  //rigidbody->setRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X | physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z);
		  physx::PxRigidBodyExt::updateMassAndInertia(*rigidbody, 10.f);
		  scenePhysique.gScene->addActor(*rigidbody);
    }


    void CSimpleGameObject::Draw(ID3D11ShaderResourceView* pDepthShaderResourceView) {
        mesh->Draw(matWorld, pDepthShaderResourceView);
    }

    void CSimpleGameObject::DrawShadow(const XMMATRIX matViewProjLight)
    {
        mesh->DrawShadow(matWorld, matViewProjLight);
    }

} // namespace PM3D
