#include "stdafx.h"
#include "ScenePhysique.h"
#include "../src/MoteurWindows.h"
#include "../src/CScene.h"
#include "../src/Vehicule.h"
#include "GameManager.h"
#include "CScene.h"
#include <chrono>
using namespace physx;
using namespace std;
using namespace std::chrono;
namespace PM3D {

	PxFilterFlags FilterShader(
		PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize) {
		
		const bool isPlayer = filterData0.word0 == ScenePhysique::Filter_Group::VEHICULE || filterData1.word0 == ScenePhysique::Filter_Group::VEHICULE;
		const bool isOurs = (filterData0.word1 == ScenePhysique::Filter_Group::OURS || filterData1.word1 == ScenePhysique::Filter_Group::OURS);
		const bool isTime = (filterData0.word1 == ScenePhysique::Filter_Group::TIME || filterData1.word1 == ScenePhysique::Filter_Group::TIME);
		const bool isBonus = (filterData0.word1 == ScenePhysique::Filter_Group::BONUS || filterData1.word1 == ScenePhysique::Filter_Group::BONUS);
		const bool isTerrain = filterData0.word0 == ScenePhysique::Filter_Group::TERRAIN || filterData1.word0 == ScenePhysique::Filter_Group::TERRAIN;
		const bool isItem = filterData0.word0 == ScenePhysique::Filter_Group::ITEM || filterData1.word0 == ScenePhysique::Filter_Group::ITEM;
		const bool isWall = filterData0.word0 == ScenePhysique::Filter_Group::MUR || filterData1.word0 == ScenePhysique::Filter_Group::MUR;
		const bool isArbre = filterData0.word0 == ScenePhysique::Filter_Group::ARBRE || filterData1.word0 == ScenePhysique::Filter_Group::ARBRE;

		//const bool isVehicule = filterData1.word0 == ScenePhysique::Filter_Group::VEHICULE ;
		//const bool isContact = isWall0 || isWall1;
		GameManager& gm = CMoteurWindows::GetInstance().getGameManager();
		bool inDream= (gm.getMondeActuel()== gm.Monde::REVE);
		const int IndexItem = isItem ? filterData0.word3 : filterData1.word3;

		if (isArbre) {

			if (isTerrain)
			{
				return PxFilterFlag::eKILL;
			}
			
		}

		if (isItem)
		{
			if (isTerrain)
			{
				return PxFilterFlag::eKILL;
			}
			if (!inDream)
			{
				if (isOurs) {
					return PxFilterFlag::eKILL;
				}
				else if (isTime) {
					ScenePhysique &sp = CMoteurWindows::GetInstance().GetScenePhysique();
					sp.deleteTrigger.push_back(IndexItem);
					//CMoteurWindows::GetInstance().SetTriggerInScene(sp.deleteTrigger);
					return PxFilterFlag::eKILL;
				}
				else if (isBonus) {
					ScenePhysique &sp = CMoteurWindows::GetInstance().GetScenePhysique();
					sp.deleteTrigger.push_back(IndexItem);
					//CMoteurWindows::GetInstance().SetTriggerInScene(sp.deleteTrigger);
					return PxFilterFlag::eKILL;
				}
			}
			else if (inDream) {
				if (isOurs) {
					ScenePhysique &sp = CMoteurWindows::GetInstance().GetScenePhysique();
					sp.deleteTrigger.push_back(IndexItem);
					//CMoteurWindows::GetInstance().SetTriggerInScene(sp.deleteTrigger);
					return PxFilterFlag::eKILL;
				}
				else if (isTime) {
					return PxFilterFlag::eKILL;
				}
				else if (isBonus) {
					ScenePhysique &sp = CMoteurWindows::GetInstance().GetScenePhysique();
					sp.deleteTrigger.push_back(IndexItem);
					//CMoteurWindows::GetInstance().SetTriggerInScene(sp.deleteTrigger);
					return PxFilterFlag::eKILL;
				}
			}
		}
		// Vérification de la collision entre le véhicule et un arbre
		if (isPlayer && isArbre) {
			// Si on est dans le monde du cauchemar ... On perd !!
			GameManager& gm = CMoteurWindows::GetInstance().getGameManager();
			if (gm.getMondeActuel() == GameManager::Monde::CAUCHEMAR) {
				gm.perdre(L"           Pas les arbres !");
			}
			// Sinon on désactive le mouvement pendant un temps
			CMoteurWindows::GetInstance().GetScene().getVehicule()->disableMoveFor(250ms);
			pairFlags.set(PxPairFlag::eCONTACT_DEFAULT);
			return PxFilterFlag::eDEFAULT;
		}


		 // Vérification de la collision entre le véhicule et un mur
		if (isPlayer && isWall) {
            // Si on est dans le monde du cauchemar ... On perd !!
            GameManager& gm = CMoteurWindows::GetInstance().getGameManager();
            if (gm.getMondeActuel() == GameManager::Monde::CAUCHEMAR) {
                gm.perdre(L"Collision Fantasmagorique !");
            }

            // Sinon on désactive le mouvement pendant un temps
			CMoteurWindows::GetInstance().GetScene().getVehicule()->disableMoveFor(750ms);
			pairFlags.set(PxPairFlag::eCONTACT_DEFAULT);
			return PxFilterFlag::eDEFAULT;
		}
       pairFlags.set(PxPairFlag::eCONTACT_DEFAULT);
		return PxFilterFlag::eDEFAULT;
	}


    void ScenePhysique::Init() {
        gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

        gPvd = PxCreatePvd(*gFoundation);
        physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
        gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

        gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, gPvd);

        physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());

        // On change le filter shader !!!
        // myFilterShader.customizeSceneDesc(sceneDesc);

        sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
        gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
        sceneDesc.cpuDispatcher = gDispatcher;
        sceneDesc.filterShader = FilterShader;
        //sceneDesc.filterShader = PxDefaultSimulationFilterShader;
        gScene = gPhysics->createScene(sceneDesc);
        gMaterial = gPhysics->createMaterial(0.1f, 0.1f, 0.6f);

        // Bon, si la scène est null, c'est pas normal :/
        if (gScene != NULL) {
            physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
            if (pvdClient)
            {
                pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
                pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
                pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
            }
        }


        // Le cooking !
        physx::PxTolerancesScale scale{};
        physx::PxCookingParams params(scale);
        params.meshWeldTolerance = 0.001f;
        params.meshPreprocessParams = physx::PxMeshPreprocessingFlags(physx::PxMeshPreprocessingFlag::eWELD_VERTICES);
        params.buildGPUData = true; //Enable GRB data being produced in cooking.
        mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, params);
        if (!mCooking) {
            // Il faut crasher !
            exit(0);
        }
    }
}