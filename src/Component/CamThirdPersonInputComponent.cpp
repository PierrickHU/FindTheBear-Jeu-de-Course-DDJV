#include "stdafx.h"
#include "MoteurWindows.h"
#include "toolsMath.h"

#include <cmath>

namespace PM3D
{
   CamThirdPersonInputComponent::CamThirdPersonInputComponent() :
       CamInputComponent{ {}, {}, LEVEL }
   {
       playerOrientation = &CMoteurWindows::GetInstance().getVehicule()->orientation;
       DirectX::XMVECTOR positionInit{
           playerOrientation->getPositionX() - cos(angle) * (DISTANCE_ANCRAGE_MAX + DISTANCE_ANCRAGE_MIN) / 2
         , playerOrientation->getPositionY() + sin(angle) * (DISTANCE_ANCRAGE_MAX + DISTANCE_ANCRAGE_MIN) / 2
         , playerOrientation->getPositionZ() };

      DirectX::XMVECTOR directionInit = normaliseVecteur(playerOrientation->getPosition() - positionInit);
      DirectX::XMVECTOR axe = produitVectoriel(directionInit, normaliseVecteur(produitVectoriel(produitVectoriel(directionInit, getAxeY()), directionInit)));
      orientation = { std::move(positionInit), getQuaternionRotation(axe, angle) };
   }

   void CamThirdPersonInputComponent::Handle(float tempsEcoule)
   {
      auto terrain = CMoteurWindows::GetInstance().getTerrain();
      auto terrainSize = terrain->getSize();

      playerOrientation = &CMoteurWindows::GetInstance().getVehicule()->orientation;
      auto playerPosition = playerOrientation->getPosition();
      auto playerDirection = playerOrientation->getDirection();

      // On remet la camera derriere l'ancrage
      DirectX::XMVECTOR axeRotation = produitVectoriel(playerDirection, playerOrientation->getUp());
      DirectX::XMVECTOR vecteurPositionCam = rotateVectorWithQuaternion(-playerDirection, getQuaternionRotation(axeRotation, angle));
      //vecteurPositionCam = normaliseVecteur(vecteurPositionCam) * distance;
      vecteurPositionCam = normaliseVecteur(vecteurPositionCam) * DISTANCE_ANCRAGE_MAX;
      orientation.setPosition(playerPosition + vecteurPositionCam);

      // On s'assure que la camera reste dans le terrain
      if (!terrain->isInTerrain(orientation.getPositionX(), orientation.getPositionZ())) {
          float distanceCameraTerrain = distanceBetweenTwoPoints(convertIntoVector(terrain->getMiddlePosition()), orientation.getPosition()) - terrain->getRayonCarte();
          DirectX::XMVECTOR vecteurCameraTerrain = normaliseVecteur(convertIntoVector(terrain->getMiddlePosition()) - orientation.getPosition());
          orientation.translate(vecteurCameraTerrain * (distanceCameraTerrain + 0.5f));
      }

      // On s'assure que la camera reste dans le terrain
      float hauteurterrain = (terrain->isInTerrain(orientation.getPositionX(), orientation.getPositionZ())) ?
          terrain->getHauteurRaycast(orientation.getPositionX(), orientation.getPositionZ()) : orientation.getPositionY();
      float dH = orientation.getPositionY() - hauteurterrain;
      DirectX::XMVECTOR vecDirVersAncrageCam = normaliseVecteur(playerOrientation->getPosition() - orientation.getPosition());
      float terraintScaleY = terrain->orientation.getScale().y;
      float hauteurMin = 0.0f;
      bool tropBas = dH < hauteurMin;

      DirectX::XMVECTOR quaternionRotAngleDecal = getQuaternionRotation(axeRotation, 0.01f);
      float distance = DISTANCE_ANCRAGE_MAX;
      while (tropBas) {
         vecteurPositionCam = rotateVectorWithQuaternion(vecteurPositionCam, quaternionRotAngleDecal);
         vecteurPositionCam = normaliseVecteur(vecteurPositionCam) * distance;
         distance -= 0.1f;
         orientation.setPosition(playerPosition + vecteurPositionCam);

         //hauteurterrain = terrain->getHauteurRaycast(orientation.getPositionX(), orientation.getPositionZ());
         dH = orientation.getPositionY() - hauteurterrain;
         tropBas = dH < hauteurMin;
      }

      // on memorise la nouvelle position de l'ancrage pour la prochaine frame
      oldPlayerPosition = playerOrientation->getPosition();

      // On reoriente la camera vers l'ancrage plus offset
      float offsetY = 2.f;
      DirectX::XMVECTOR vecteurAncrageCam = playerOrientation->getPosition() - orientation.getPosition();
      DirectX::XMVECTOR newUp = normaliseVecteur(produitVectoriel(produitVectoriel(vecteurAncrageCam, getAxeY()), vecteurAncrageCam));
      orientation.setDirection(normaliseVecteur(vecteurAncrageCam), newUp);
      orientation.translate(offsetY * (getAxeY()));
   }
}
