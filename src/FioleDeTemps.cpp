#include "stdafx.h"
#include "FioleDeTemps.h"
#include "Item.h"
#include "util.h"
#include "dispositifD3D11.h"
#include "toolsMath.h"
#include "Resource.h"
#include "Moteur.h"
#include "MoteurWindows.h"
#include "ScenePhysique.h"
#include <chrono>
using namespace std;
using namespace std::chrono;

namespace PM3D
{
    FioleDeTemps::FioleDeTemps(const Orientation& _orientation, const float _dx, const float _dy, const float _dz, CDispositifD3D11* _pDispositif, ScenePhysique& scenePhysique)
        : Item(_orientation, _dx, _dy, _dz, _pDispositif, scenePhysique, ModelType::SANDWATCH) {
		 physx::PxFilterData filterData;
		 filterData.word0 = ScenePhysique::Filter_Group::ITEM;
		 filterData.word1 = ScenePhysique::Filter_Group::TIME;
		 filterData.word3 = this->physic->shape->getSimulationFilterData().word3;
		 this->physic->shape->setSimulationFilterData(filterData);
    }

    void FioleDeTemps::OnTrigger() {
        GameManager& gameManager = CMoteurWindows::GetInstance().getGameManager();
        gameManager.addTempsDeJeu(20'000ms);
    }

    bool FioleDeTemps::isCatchable() const noexcept {
        GameManager& gameManager = CMoteurWindows::GetInstance().getGameManager();
        return gameManager.getMondeActuel() == GameManager::Monde::CAUCHEMAR;
    }
} // namespace PM3D
