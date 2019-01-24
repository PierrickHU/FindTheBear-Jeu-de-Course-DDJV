#include "stdafx.h"
#include "Formes/Chateau.h"
#include "Bloc.h"
#include "Orientation.h"
#include "ScenePhysique.h"
#include "MoteurWindows.h"
#include "CTerrain.h"
#include "toolsMath.h"
#include <string>

using namespace DirectX;
using namespace std;

namespace PM3D
{
    Chateau::Chateau(CDispositifD3D11* pDispositif, ScenePhysique& scenePhysique) {
        // La texture du chateau
        wstring textureFilename{ L"../ressources/textures/stone.dds" };

        // Initialiser les différents blocs aux bons endroits ! :)
        CTerrain* terrain = CMoteurWindows::GetInstance().GetScene().getTerrain();
        XMVECTOR centreTerrain = convertIntoVector(terrain->getMiddlePosition());
        float rayon = terrain->getRayonChateau();
        // Ne pas oublier qu'il s'agit d'half-extents
        float ratioMurOuverture = 0.4f; // 0.4 de mur, puis 0.2 d'ouverture, puis 0.4 de mur à nouveau
        float proportionPorte = 1.f - 2 * ratioMurOuverture;
        float longueur = sqrt(2) * rayon / 2.f * ratioMurOuverture * 1.767f;
        float base = 0.f;

        // Variables tunnel
        float largeurTunel = sqrt(2) * rayon / 2.f * proportionPorte * 1.767f / 2.f;

        // Murs haut
        Orientation orientation{};
        orientation.setPosition(centreTerrain);
        orientation.translate({rayon, 0.f, longueur * (1.f + proportionPorte)});
        orientation.rotateWithQuaternion(getQuaternionRotation(getAxeY(), XM_PIDIV2));
        orientation.setScale(XMFLOAT3{longueur, hauteur, largeur});
        Bloc mur{orientation, textureFilename, pDispositif, scenePhysique};
        murs.push_back(mur);

        orientation.setPosition(centreTerrain);
        orientation.translate({rayon, 0.f, - longueur * (1.f + proportionPorte)});
        orientation.setScale(XMFLOAT3{longueur, hauteur, largeur});
        mur = Bloc{orientation, textureFilename, pDispositif, scenePhysique};
        murs.push_back(mur);

        // Murs bas
        orientation.setPosition(centreTerrain);
        orientation.translate({-rayon, 0.f, longueur * (1.f + proportionPorte)});
        orientation.setScale(XMFLOAT3{longueur, hauteur, largeur});
        mur = Bloc{orientation, textureFilename, pDispositif, scenePhysique};
        murs.push_back(mur);

        orientation.setPosition(centreTerrain);
        orientation.translate({-rayon, 0.f, - longueur * (1.f + proportionPorte)});
        orientation.setScale(XMFLOAT3{longueur, hauteur, largeur});
        mur = Bloc{orientation, textureFilename, pDispositif, scenePhysique};
        murs.push_back(mur);

        // Murs gauche
        orientation.setPosition(centreTerrain);
        orientation.setQuaternion(XMQuaternionIdentity());
        orientation.translate({longueur * (1.f + proportionPorte), 0.f, rayon});
        orientation.setScale(XMFLOAT3{longueur, hauteur, largeur});
        mur = Bloc{orientation, textureFilename, pDispositif, scenePhysique};
        murs.push_back(mur);

        orientation.setPosition(centreTerrain);
        orientation.translate({longueur * (1.f + proportionPorte), 0.f, - rayon});
        orientation.setScale(XMFLOAT3{longueur, hauteur, largeur});
        mur = Bloc{orientation, textureFilename, pDispositif, scenePhysique};
        murs.push_back(mur);

        // Murs droit
        orientation.setPosition(centreTerrain);
        orientation.translate({-longueur * (1.f + proportionPorte), 0.f, rayon});
        orientation.setScale(XMFLOAT3{longueur, hauteur, largeur});
        mur = Bloc{orientation, textureFilename, pDispositif, scenePhysique};
        murs.push_back(mur);

        orientation.setPosition(centreTerrain);
        orientation.translate({-longueur * (1.f + proportionPorte), 0.f, - rayon});
        orientation.setScale(XMFLOAT3{longueur, hauteur, largeur});
        mur = Bloc{orientation, textureFilename, pDispositif, scenePhysique};
        murs.push_back(mur);

        // Tunnel 1
        orientation.setPosition(centreTerrain);
        orientation.translate({-longueur * (1.f + proportionPorte), 0.f, largeurTunel});
        orientation.setScale(XMFLOAT3{longueur, hauteur, largeur});
        mur = Bloc{orientation, textureFilename, pDispositif, scenePhysique};
        murs.push_back(mur);

        orientation.setPosition(centreTerrain);
        orientation.translate({-longueur * (1.f + proportionPorte), 0.f, - largeurTunel});
        orientation.setScale(XMFLOAT3{longueur, hauteur, largeur});
        mur = Bloc{orientation, textureFilename, pDispositif, scenePhysique};
        murs.push_back(mur);

        orientation.setPosition(centreTerrain);
        orientation.translate({-longueur * (1.f + proportionPorte), hauteur - largeur / 2.f, 0.f});
        orientation.setScale(XMFLOAT3{longueur, largeur / 2.f, longueur * proportionPorte});
        mur = Bloc{orientation, textureFilename, pDispositif, scenePhysique};
      //  murs.push_back(mur);
        toitsTunnel.push_back(mur);

        // Tunnel 2
        orientation.setPosition(centreTerrain);
        orientation.translate({+longueur * (1.f + proportionPorte), 0.f, largeurTunel});
        orientation.setScale(XMFLOAT3{longueur, hauteur, largeur});
        mur = Bloc{orientation, textureFilename, pDispositif, scenePhysique};
        murs.push_back(mur);

        orientation.setPosition(centreTerrain);
        orientation.translate({+longueur * (1.f + proportionPorte), 0.f, - largeurTunel});
        orientation.setScale(XMFLOAT3{longueur, hauteur, largeur});
        mur = Bloc{orientation, textureFilename, pDispositif, scenePhysique};
        murs.push_back(mur);

        orientation.setPosition(centreTerrain);
        orientation.translate({+longueur * (1.f + proportionPorte), hauteur - largeur / 2.f, 0.f});
        orientation.setScale(XMFLOAT3{longueur, largeur / 2.f, longueur * proportionPorte});
        mur = Bloc{orientation, textureFilename, pDispositif, scenePhysique};
       // murs.push_back(mur);
        toitsTunnel.push_back(mur);

    }

    void Chateau::Draw() {
        for (Bloc mur : murs)
            mur.Draw();

        for (Bloc toit : toitsTunnel)
            toit.Draw();
    }

    bool Chateau::isInTunnel(float x, float z) const {
        for (Bloc b : toitsTunnel) {
            // On définit les 4 points du bloc
            float x0 = b.getOrientation().getPositionX() - b.getOrientation().getScale().x;
            float x1 = b.getOrientation().getPositionX() + b.getOrientation().getScale().x;
            float z0 = b.getOrientation().getPositionZ() - b.getOrientation().getScale().z;
            float z1 = b.getOrientation().getPositionZ() + b.getOrientation().getScale().z;

            // Attention ! Le code qui suit ne fonctionne QUE si le tunnel est dans les axes ! Ce qui est le cas actuellement !
            if (x0 <= x && x <= x1 && z0 <= z && z <= z1)
                return true;
        }
        return false;
    }

    bool Chateau::isInChateau(float x, float z, float dx, float dz) const {
        // On va lancer un raycast vers le bas depuis (x, hauteurMax, z) et voir la distance qu'il parcours !
        ScenePhysique sp = CMoteurWindows::GetInstance().GetScenePhysique();
        physx::PxVec3 direction{ 0.0f, -1.0f, 0.0f };
        physx::PxReal hauteurMax = hauteur - largeur / 2.f - largeur / 4.f - 10.f;
      //  physx::PxReal hauteurMax = 1000.f;
        physx::PxReal distanceMax = hauteurMax;
        const physx::PxHitFlags outputFlags = physx::PxHitFlag::eDISTANCE;
        physx::PxQueryFilterData filterData = physx::PxQueryFilterData();
        filterData.data.word0 = ScenePhysique::Filter_Group::MUR;
        physx::PxRaycastBuffer hit;

        physx::PxVec3 depart{ x, hauteurMax, z };
        return sp.gScene->raycast(depart, direction, distanceMax, hit, outputFlags, filterData);
    }

    void Chateau::DrawShadow(const XMMATRIX matViewProjLight)
    {
       for (auto mur : murs) {
          mur.getMesh().DrawShadow(mur.getOrientation().calculMatriceWorld(), matViewProjLight);
       }

       for (auto toit : toitsTunnel) {
          toit.getMesh().DrawShadow(toit.getOrientation().calculMatriceWorld(), matViewProjLight);
       }
    }

} // namespace PM3D
