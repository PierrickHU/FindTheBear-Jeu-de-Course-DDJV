#include "stdafx.h"
#include "Vehicule.h"
#include "util.h"
#include "dispositifD3D11.h"
#include "toolsMath.h"
#include "Resource.h"
#include "Moteur.h"
#include "MoteurWindows.h"
#include "ScenePhysique.h"
#include "GameState.h"

using namespace std;
using namespace std::chrono;

namespace PM3D
{
    Vehicule::Vehicule(const Orientation& _orientation, const float _dx, const float _dy, const float _dz, CDispositifD3D11* _pDispositif, ScenePhysique& scenePhysique)
        :CGameObject(_orientation)
    {
        orientation.setScale(DirectX::XMFLOAT3{ _dx, _dy, _dz });

        mesh = CMoteurWindows::GetInstance().GetRessourcesManager().GetModelMesh(ModelType::CLOUD);
        mesh->shaderFilename = L"../ressources/shaders/MiniPhongSM.fx";
        mesh->Init(_pDispositif);

        physic = std::make_unique<PhysicPlayerComponent>(this, scenePhysique);
        physic->boundingBox = DirectX::XMVECTOR{ _dx / 2.f, _dy / 2.f, _dz / 2.f };

        input = std::make_unique<InputPlayerComponent>();
        stars.push_back(std::make_unique<Billboard>(_pDispositif, orientation.getPosition(), DirectX::XMVECTOR{ 1, -1, 0 }, L"../ressources/textures/Star.dds"));
        stars.push_back(std::make_unique<Billboard>(_pDispositif, orientation.getPosition(), DirectX::XMVECTOR{ -1, -1, 0 }, L"../ressources/textures/Star.dds"));
    }

    void Vehicule::Update(const float tempsEcoule, const CTerrain& terrain, ScenePhysique& sp)
    {
      physic->UpdateFromSimulation(this);

        if (canMove()) {
            physic->rigidbody->setRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X | physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z);
        } else {
            physic->rigidbody->setRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X | physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z | physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y);
            physic->vitesseAngulaire = {};
            physic->vitesseLineaire = {};
        }

        if (CMoteurWindows::GetInstance().GetCamera().isLevel()) {
                input->Handle(tempsEcoule, this);
        }
        else if (CMoteurWindows::GetInstance().GetCamera().isFirstPerson()) {
                input->Handle(tempsEcoule, this);
        }
        else {
            physic->vitesseAngulaire *= 0.5f;
            physic->vitesseLineaire *= 0.5f;
        }

        physic->ajouterOscillation(this, terrain, tempsEcoule);
        physic->ajouterForceRenvoyerCentreTerrain(this, terrain);
        if(canMove()) physic->UpdateSimulation(orientation, terrain, sp); // On ne met à jour la simulation QUE si on a pu bouger !! // Sinon on la laisse faire ! :)
        calculMatriceWorld();

        stars[0]->test = DirectX::XMVectorAdd(orientation.getPosition(), { 1, -1, 0 });
        stars[1]->test = DirectX::XMVectorAdd(orientation.getPosition(), { -1, -1, 0 });

        for (auto& star : stars) {
            star->Update(tempsEcoule);
        }

    }
    void Vehicule::Draw(ID3D11ShaderResourceView* pDepthShaderResourceView)
    {
        mesh->Draw(matWorld, pDepthShaderResourceView);
        for (auto& star : stars) {
            star->Draw(pDepthShaderResourceView);
        }
    }

    void Vehicule::DrawShadow(const XMMATRIX matViewProjLight) {
       mesh->DrawShadow(matWorld, matViewProjLight);
    }

    bool Vehicule::canMove() const noexcept {
        return duration_cast<milliseconds>(high_resolution_clock::now() - lastTimeDisableMove) > dureeDisableMove;
    }

    void Vehicule::disableMoveFor(milliseconds duree) {
        lastTimeDisableMove = high_resolution_clock::now();
        dureeDisableMove = duree;
    }

} // namespace PM3D
