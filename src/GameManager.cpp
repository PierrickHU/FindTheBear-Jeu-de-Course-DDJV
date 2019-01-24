#include "stdafx.h"
#include "toolsMath.h"
#include "GameManager.h"
#include "Formes/Orientation.h"
#include "Formes/CTerrain.h"
#include "MoteurWindows.h"
#include "CScene.h"
#include "RandomGenerateur.h"
#include "ScenePhysique.h"
#include "Item.h"
#include "Boost.h"
#include "FioleDeTemps.h"
#include "PartieOurs.h"

#include <memory>
#include <algorithm>
using namespace std;
using namespace std::chrono;
using namespace DirectX;
using namespace physx;

namespace PM3D
{
    GameManager::GameManager() {
    }

    void GameManager::Init(vector<unique_ptr<Item>>& items, CDispositifD3D11* dispositif) {
        ScenePhysique& sp = CMoteurWindows::GetInstance().GetScenePhysique();
        Chateau& c = CMoteurWindows::GetInstance().GetScene().getChateau();
        // Les parties de l'ours
        for (int i = 0; i != NB_PARTIES_OURS; ++i) {
            Orientation orientationItem = getRandomOrientation(10.f + 3.5f, 20.f, 20.f);
            items.push_back(std::make_unique<PartieOurs>(orientationItem, 20.f, 20.f, 20.f, dispositif, sp));
        }
        // Les boosts
        for (int i = 0; i != NB_BOOSTS; i++) {
            Orientation orientationItem = getRandomOrientation(10.f, 10.f, 10.f);
            items.push_back(std::make_unique<Boost>(orientationItem, 10.0f, 10.0f, 10.0f, dispositif, sp));
        }
        // Les fioles de temps !
        for (int i = 0; i != NB_FIOLE_DE_TEMPS; i++) {
            Orientation orientationItem = getRandomOrientation(10.f, 10.f, 10.f);
            items.push_back(std::make_unique<FioleDeTemps>(orientationItem, 10.0f, 10.0f, 10.0f, dispositif, sp));
        }
        tempsDebutJeu = high_resolution_clock::now();
        lastTimeChangementMonde = high_resolution_clock::now(); // Nous fera faire une transition au lancement du jeu !
    }

    void GameManager::Anime() {
        // On regarde si c'est la fin du compte a rebours
        if (getRemainingTime() < 0ms) {
            perdre(L"Reve ecoule ! zzzzzzzzzz ...");
        }

        // On regarde si on a appuye sur Escape
        if (CMoteurWindows::GetInstance().GetGestionnaireDeSaisie().ToucheAppuyee(DIK_ESCAPE)) {
            //gameResult = LOSE;
            exit(0);
        }

        // On regarde si on a appuy� sur Espace
        if (CMoteurWindows::GetInstance().GetGestionnaireDeSaisie().ToucheAppuyee(DIK_SPACE)) {
            // Si oui, on change de monde !
            changerMonde();
        }

        // Retourne automatiquement dans le monde du Reve
        if (getMondeActuel() == CAUCHEMAR) {
            if (tempsAvantRetourAuReve() <= 0ms) {
                changerMonde();
            }
        }
    }


    void GameManager::changerMonde() {
        if (mondeActuel == Monde::REVE) {
            if (duration_cast<milliseconds>(high_resolution_clock::now() - lastTimeChangementMonde) > dureeMinimaleAvantSwapMonde) {
                mondeActuel = Monde::CAUCHEMAR;
                lastTimeChangementMonde = high_resolution_clock::now();
            }
        }
        else {
            if (duration_cast<milliseconds>(high_resolution_clock::now() - lastTimeChangementMonde) > dureeMinimaleCauchemar) {
                mondeActuel = Monde::REVE;
                lastTimeChangementMonde = high_resolution_clock::now();
            }
        }
    }

    milliseconds GameManager::tempsAvantRetourAuReve() const {
        if (getMondeActuel() == Monde::REVE) return milliseconds{ 0 };
        return dureeMinimaleCauchemar - duration_cast<milliseconds>(high_resolution_clock::now() - lastTimeChangementMonde);
    }

    Orientation GameManager::getRandomOrientation(float offsetY, float dx, float dz) const {
        CTerrain* terrain = CMoteurWindows::GetInstance().getTerrain();
        Chateau& c = CMoteurWindows::GetInstance().GetScene().getChateau();
        float x, z;
        do
        {
            x = RandomGenerateur::get().random(0.0f, terrain->getSize().x);
            z = RandomGenerateur::get().random(0.0f, terrain->getSize().z);
        } while (!terrain->isInTerrain(x, z) || c.isInChateau(x, z, dx, dz));

        Orientation orientation{ DirectX::XMVECTOR{
            x,
            terrain->getHauteurRaycast(x,z) + offsetY,
            z} };

        XMVECTOR rotationAleatoire = getQuaternionRotation(getAxeY(), RandomGenerateur::get().random(0.0f, XM_2PI));
        orientation.rotateWithQuaternion(rotationAleatoire);

        return orientation;
    }


    Orientation GameManager::getRandomOrientationArbre(float offsetY, float dx, float dz) const {
        CTerrain* terrain = CMoteurWindows::GetInstance().getTerrain();
        Chateau& c = CMoteurWindows::GetInstance().GetScene().getChateau();
        float x, z;
        do
        {
            x = RandomGenerateur::get().random(0.0f, terrain->getSize().x);
            z = RandomGenerateur::get().random(0.0f, terrain->getSize().z);
        } while (!terrain->isInTerrain(x, z) || c.isInTunnel(x, z) || c.isInChateau(x, z, dx, dz));

        Orientation orientation{ DirectX::XMVECTOR{
            x,
            terrain->getHauteurRaycast(x,z) + offsetY,
            z } };

        XMVECTOR rotationAleatoire = getQuaternionRotation(getAxeY(), RandomGenerateur::get().random(0.0f, XM_2PI));
        orientation.rotateWithQuaternion(rotationAleatoire);

        return orientation;
    }

    void GameManager::decrementPartiesOurs() {
        nbPartiesOursActuel--;
        if (nbPartiesOursActuel <= 0) {
            gameResult = WIN;
        }
    }

    std::pair<unsigned int, unsigned int> GameManager::getNbOursResult() {
        return { NB_PARTIES_OURS - nbPartiesOursActuel, NB_PARTIES_OURS };
    }

    void GameManager::addTempsDeJeu(milliseconds tempsAdditionnel) {
        tempsDeJeu += tempsAdditionnel;
    }

    void GameManager::perdre(wstring message) {
        gameResult = GameResult::LOSE;
        messageDefaite = message;
    }
    void GameManager::gagner() {
        gameResult = GameResult::WIN;
    }

    wstring GameManager::getMessageDefaite() const noexcept {
        return messageDefaite;
    }

    milliseconds GameManager::getRemainingTime() const {
        return tempsDeJeu - duration_cast<milliseconds>(high_resolution_clock::now() - tempsDebutJeu);
    }

    milliseconds GameManager::getTimeSinceBeginning() const {
        return duration_cast<milliseconds>(high_resolution_clock::now() - tempsDebutJeu);
    }

    GameManager::Monde GameManager::getMondeActuel() const noexcept {
        return mondeActuel;
    }

    GameResult GameManager::getGameResult() const {
        return gameResult;
    }

    float GameManager::shader_getPuissanceAttenuation() const {
        milliseconds t{ duration_cast<milliseconds>(high_resolution_clock::now() - lastTimeChangementMonde) };
        float valMax = 2.5f;
        float valMin = 0.5f;
        if (t < tempsEffetTransitionCroissant) {
            float avancement = static_cast<float>(t.count()) / static_cast<float>(tempsEffetTransitionCroissant.count());
            return valMin + avancement * (valMax - valMin);

        } else if (t < tempsEffetTransitionCroissant + tempsEffetTransitionDecroissant) {
            float avancement = static_cast<float>(t.count() - tempsEffetTransitionCroissant.count()) / static_cast<float>(tempsEffetTransitionDecroissant.count());
            return valMax - avancement * (valMax - valMin);

        } else {
            return 0.f;
        }
    }
    float GameManager::shader_getPuissanceEclaircissement() const {
        milliseconds t{ duration_cast<milliseconds>(high_resolution_clock::now() - lastTimeChangementMonde) };
        float valMax = 3.f;
        float valMin = 1.f;
        if (t < tempsEffetTransitionCroissant) {
            float avancement = static_cast<float>(t.count()) / static_cast<float>(tempsEffetTransitionCroissant.count());
            return valMin + avancement * (valMax - valMin);

        }
        else if (t < tempsEffetTransitionCroissant + tempsEffetTransitionDecroissant) {
            float avancement = static_cast<float>(t.count() - tempsEffetTransitionCroissant.count()) / static_cast<float>(tempsEffetTransitionDecroissant.count());
            return valMax - avancement * (valMax - valMin);

        }
        else {
            return 0.f;
        }
    }
}
