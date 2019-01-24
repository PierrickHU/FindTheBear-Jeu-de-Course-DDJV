#include "stdafx.h"
#include "PhysicPlayerComponent.h"
#include "Formes/CTerrain.h"
#include "MoteurWindows.h"
#include "GameManager.h"
#include "toolsMath.h"
#include "Vehicule.h"
#include "InputPlayerComponent.h"
using namespace DirectX;
using namespace physx;
using namespace std::chrono;

namespace PM3D
{
    PhysicPlayerComponent::PhysicPlayerComponent(CGameObject* obj, ScenePhysique& scenePhysique) {
        XMFLOAT3 taille = obj->orientation.getScale();
        PxMaterial* vehiculeMaterial = scenePhysique.gPhysics->createMaterial(scenePhysique.gMaterial->getStaticFriction(), scenePhysique.gMaterial->getDynamicFriction(), scenePhysique.gMaterial->getRestitution());
        vehiculeMaterial->setRestitution(0.0f);
        //vehiculeMaterial->setStaticFriction(1.0f);
        //vehiculeMaterial->setDynamicFriction(1.0f);
        shape = scenePhysique.gPhysics->createShape(physx::PxBoxGeometry{ taille.x, taille.y, taille.z }, *vehiculeMaterial, true);
        //shape = scenePhysique.gPhysics->createShape(physx::PxBoxGeometry{0.5f, 0.5f, 0.5f}, *(scenePhysique.gMaterial), true);
        physx::PxTransform t{ obj->orientation.getPositionX(), obj->orientation.getPositionY(), obj->orientation.getPositionZ() };

        physx::PxFilterData filterData;
        filterData.word0 = ScenePhysique::Filter_Group::VEHICULE;
        shape->setQueryFilterData(filterData);
        shape->setSimulationFilterData(filterData);
        rigidbody = scenePhysique.gPhysics->createRigidDynamic(t);
        rigidbody->setAngularDamping(0.5f);
        rigidbody->attachShape(*shape);
        rigidbody->setName("Bloc");
        rigidbody->setRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X | physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z);

        physx::PxRigidBodyExt::updateMassAndInertia(*rigidbody, 3000.f / 8.f);
        scenePhysique.gScene->addActor(*rigidbody);

        oldVitesseY = 0.f;
    }

    void PhysicPlayerComponent::ajouterOscillation(CGameObject* obj, const CTerrain& terrain, const float tempsEcoule) {
        // On va appliquer les forces du petits nuage pour le faire leviter ! :3
        // On calcul la distance moyenne du petit nuage au sol, pour ca on fait 4 raycasts, un par angles du bas, vers le sol !
        ScenePhysique sp = CMoteurWindows::GetInstance().GetScenePhysique();
        vector<float> hitDistances;
        auto nuagePosX = obj->orientation.getPositionX();
        auto nuagePosY = obj->orientation.getPositionY();
        auto nuagePosZ = obj->orientation.getPositionZ();
        auto boundingBoxX = DirectX::XMVectorGetX(boundingBox);
        auto boundingBoxZ = DirectX::XMVectorGetZ(boundingBox);

        physx::PxVec3 direction{ 0.0f, -1.0f, 0.0f };
        physx::PxReal distanceMax = 1000000.f;
        const physx::PxHitFlags outputFlags = physx::PxHitFlag::eDISTANCE | physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL;
        physx::PxQueryFilterData filterData = physx::PxQueryFilterData();
        filterData.data.word0 = ScenePhysique::Filter_Group::TERRAIN | ScenePhysique::Filter_Group::MUR;
        physx::PxRaycastBuffer hit;

        //Angle 1
        physx::PxVec3 depart{ nuagePosX - boundingBoxX, nuagePosY, nuagePosZ - boundingBoxZ };
        if (sp.gScene->raycast(depart, direction, distanceMax, hit, outputFlags, filterData)) {
            hitDistances.push_back(hit.block.distance);
        }
        // Angle 2
        depart = { nuagePosX + boundingBoxX, nuagePosY, nuagePosZ - boundingBoxZ };
        if (sp.gScene->raycast(depart, direction, distanceMax, hit, outputFlags, filterData)) {
            hitDistances.push_back(hit.block.distance);
        }
        // Angle 3
        depart = { nuagePosX - boundingBoxX, nuagePosY, nuagePosZ + boundingBoxZ };
        if (sp.gScene->raycast(depart, direction, distanceMax, hit, outputFlags, filterData)) {
            hitDistances.push_back(hit.block.distance);
        }
        // Angle 4
        depart = { nuagePosX + boundingBoxX, nuagePosY, nuagePosZ + boundingBoxZ };
        if (sp.gScene->raycast(depart, direction, distanceMax, hit, outputFlags, filterData)) {
            hitDistances.push_back(hit.block.distance);
        }
        // Si on a au moins 1 raycast qui a touche !
        // Si on a au moins 1 raycast qui a touche !
        if (!hitDistances.empty()) {

            float distanceMoyenne = moyenne(hitDistances);
            // Puis on calcul notre force
            float hauteurMinPetitNuage = 5.f;
            if (distanceMoyenne < hauteurMinPetitNuage) { // en dessous de la hauteur moyenne
                float coefForceUp = 800.f;
                rigidbody->addForce({ 0.0f, exp(hauteurMinPetitNuage - distanceMoyenne) * coefForceUp, 0.0f }, physx::PxForceMode::eIMPULSE); // On pousse vers le haut !

                // Si on est vraiment trop bas, on met un seuil sur la vitesse vers le bas
                float hauteurSecuritePetitNuage = 2.f;
                if (distanceMoyenne < hauteurSecuritePetitNuage) {
                    float avancement = distanceMoyenne / hauteurSecuritePetitNuage;
#undef max
                    if (vitesseLineaire.vector4_f32[1] < 0.f)
                        vitesseLineaire.vector4_f32[1] *= avancement;
                }
            }
            else { // au dessus de la hauteur moyenne
                float coefForceDown = 600.f;
                rigidbody->addForce({ 0.0f, (hauteurMinPetitNuage - distanceMoyenne) * coefForceDown, 0.0f }, physx::PxForceMode::eIMPULSE); // On pousse vers le haut !
            }
        }
        else {
            // Si aucun raycast n'a touche, ca veut dire qu'on est hors du terrain ! (ou au dessus de celui-ci de genre beaucoup !)
            // Donc on va lentement faire remonter notre petit nuage !
            if (terrain.isInTerrain(nuagePosX, nuagePosZ)) {
                rigidbody->addForce({ 0.0f, 10000.f, 0.0f }, physx::PxForceMode::eIMPULSE); // On pousse vers le haut !
            }
            else {
                // Si on est trop loin du terrain, on téléporte à l'extrémité du terrain
                CTerrain* terrain = CMoteurWindows::GetInstance().getTerrain();
                float distanceJoueurTerrain = distanceBetweenTwoPoints(convertIntoVector(terrain->getMiddlePosition()), obj->orientation.getPosition()) - terrain->getRayonCarte();
                if (distanceJoueurTerrain > 50.f) {
                    DirectX::XMVECTOR vecteurJoueurTerrain = normaliseVecteur(convertIntoVector(terrain->getMiddlePosition()) - obj->orientation.getPosition());
                    obj->orientation.translate(vecteurJoueurTerrain * (distanceJoueurTerrain + 0.5f));
                }
            }
        }
    }

    void PhysicPlayerComponent::ajouterForceRenvoyerCentreTerrain(CGameObject* obj, const CTerrain& terrain) {
        // On vérifie qu'on est pas dans le terrain !
        if (!terrain.isInTerrain(obj->orientation.getPositionX(), obj->orientation.getPositionZ())) {
            lastTimeOutOfTerrain = high_resolution_clock::now();

            // Si on est dans le monde du cauchemar ... On perd !!
            GameManager& gm = CMoteurWindows::GetInstance().getGameManager();
            if (gm.getMondeActuel() == GameManager::Monde::CAUCHEMAR) {
                gm.perdre(L"Chute Cauchemardesque !");
            }
        }
        if (aDepasseTerrainRecemment()) {
            float forceProjectionHorizontale = 1000000.f * CMoteurWindows::GetInstance().getVehicule()->input.get()->getCoefVitesse();
            float forceProjectionVerticale = 0.0f;// 25000.f;
            XMVECTOR centre = convertIntoVector(terrain.getMiddlePosition()) + getAxeY() * terrain.size.y * 2.f;
            XMVECTOR direction = centre - obj->orientation.getPosition();
            direction = normaliseVecteur(direction);
            float tempsEcoule = static_cast<float>(duration_cast<milliseconds>(high_resolution_clock::now() - lastTimeOutOfTerrain).count());
            float tempsTotal = static_cast<float>(TEMPS_INACTION_OUT_OF_TERRAIN.count());
            float avancement = 1.f - tempsEcoule / tempsTotal;
            PxVec3 force = toPxVec3(direction);
            force.x *= forceProjectionHorizontale * avancement;
            force.y = forceProjectionVerticale * avancement;
            force.z *= forceProjectionHorizontale * avancement;
            rigidbody->addForce(force, PxForceMode::eIMPULSE);
            rigidbody->addForce(toPxVec3(getAxeY() * 9.81f), PxForceMode::eACCELERATION);
        }
    }

    bool PhysicPlayerComponent::aDepasseTerrainRecemment() const noexcept {
        float coef = CMoteurWindows::GetInstance().getVehicule()->input.get()->getCoefVitesse();
        return duration_cast<milliseconds>(high_resolution_clock::now() - lastTimeOutOfTerrain).count() < static_cast<float>(TEMPS_INACTION_OUT_OF_TERRAIN.count()) / coef;
    }

    float PhysicPlayerComponent::getDescelerationFreinage(const float vitesseInit) const {
        return -vitesseInit / TEMPS_FREINAGE;
    }
    float PhysicPlayerComponent::getAccelerationRelance(const float vitesseVoulue) const {
        return vitesseVoulue / TEMPS_FREINAGE;
    }
}