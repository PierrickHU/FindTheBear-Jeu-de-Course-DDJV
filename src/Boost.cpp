#include "stdafx.h"
#include "Boost.h"
#include "PartieOurs.h"
#include "MoteurWindows.h"
#include "CScene.h"
#include "GameManager.h"
#include "Vehicule.h"

namespace PM3D
{
    Boost::Boost(const Orientation& _orientation, const float _dx, const float _dy, const float _dz, CDispositifD3D11* _pDispositif, ScenePhysique& scenePhysique)
        : Item(_orientation, _dx, _dy, _dz, _pDispositif, scenePhysique, ModelType::MYSTERYBOX) {
		 physx::PxFilterData filterData;
		 filterData.word0 = ScenePhysique::Filter_Group::ITEM;
		 filterData.word1 = ScenePhysique::Filter_Group::BONUS;
		 filterData.word3 = this->physic->shape->getSimulationFilterData().word3;
		 this->physic->shape->setSimulationFilterData(filterData);
    }

    void Boost::OnTrigger() {
        Vehicule* vehicule = CMoteurWindows::GetInstance().GetScene().getVehicule();
        float coefActuel = vehicule->input->getVitesseMaxCoef();
        float acceleration = 0.5f;
        vehicule->input->setVitesseMaxCoef(coefActuel + acceleration);
    }

} // namespace PM3D
