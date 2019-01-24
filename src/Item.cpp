#include "stdafx.h"
#include "Item.h"
#include "util.h"
#include "dispositifD3D11.h"
#include "toolsMath.h"
#include "Resource.h"
#include "Moteur.h"
#include "MoteurWindows.h"
#include "ScenePhysique.h"
#include "GameState.h"

namespace PM3D
{

    int Item::currentIndex = 0; // Commence a 0 et est decremente 

	Item::Item(const Orientation& _orientation, const float _dx, const float _dy, const float _dz, CDispositifD3D11* _pDispositif, ScenePhysique& scenePhysique, ModelType model)
		:CGameObject(_orientation)
	{
		orientation.setScale(DirectX::XMFLOAT3{ _dx, _dy,  _dz });
		mesh = CMoteurWindows::GetInstance().GetRessourcesManager().GetModelMesh(model);
		mesh->shaderFilename = L"../ressources/shaders/MiniPhongSM.fx";
        int index = Item::currentIndex++;
		indexTrigger = index;
		mesh->Init(_pDispositif);
		physic = std::make_unique<PhysicTriggerComponent>(this, scenePhysique, index);
		physic->boundingBox = DirectX::XMVECTOR{ _dx / 2.f, _dy / 2.f, _dz / 2.f };
	}

	void Item::Update(const float tempsEcoule, const CTerrain& terrain, ScenePhysique& sp)
	{
		//physic->UpdateFromSimulation(this);
		calculMatriceWorld();
	}

	void Item::Draw(ID3D11ShaderResourceView* pDepthShaderResourceView)
	{
        // Si on n'est pas attrapable, on devient un peu transparent ! :3
        if(!this->isCatchable())
            mesh->getDispositif()->ActiverMelangeAlpha();
		mesh->Draw(matWorld);
        if(!this->isCatchable())
            mesh->getDispositif()->DesactiverMelangeAlpha();
	}

    bool Item::isCatchable() const noexcept {
        return true; // Par défaut on peut toujours attraper un item
    }

    void Item::DrawShadow(const XMMATRIX matViewProjLight)
    {
       mesh->DrawShadow(matWorld, matViewProjLight);
    }
} // namespace PM3D
