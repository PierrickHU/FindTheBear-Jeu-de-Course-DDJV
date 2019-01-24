#include "stdafx.h"
#include "PartieOurs.h"
#include "MoteurWindows.h"
#include "CScene.h"
#include "GameManager.h"

namespace PM3D {

    PartieOurs::PartieOurs(const Orientation& _orientation, const float _dx, const float _dy, const float _dz, CDispositifD3D11* _pDispositif, ScenePhysique& scenePhysiquex)
        : Item(_orientation, _dx, _dy, _dz, _pDispositif, scenePhysiquex, ModelType::BEAR) {
		 physx::PxFilterData filterData;
		 filterData.word0 = ScenePhysique::Filter_Group::ITEM;
		 filterData.word1 = ScenePhysique::Filter_Group::OURS;
		 filterData.word3 = this->physic->shape->getSimulationFilterData().word3;
		 this->physic->shape->setSimulationFilterData(filterData);
    }

    void PartieOurs::OnTrigger() {
        GameManager& gameManager = CMoteurWindows::GetInstance().getGameManager();
        gameManager.decrementPartiesOurs();
    }

    bool PartieOurs::isCatchable() const noexcept {
        GameManager& gameManager = CMoteurWindows::GetInstance().getGameManager();
        return gameManager.getMondeActuel() == GameManager::Monde::REVE;
    }

} // namespace PM3D
