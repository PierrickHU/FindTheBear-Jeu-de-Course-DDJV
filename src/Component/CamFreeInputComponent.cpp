#include "stdafx.h"
#include "CamFreeInputComponent.h"
#include "MoteurWindows.h"
#include "toolsMath.h"

#include <cmath>

namespace PM3D
{
   CamFreeInputComponent::CamFreeInputComponent(DirectX::XMVECTOR&& position, DirectX::XMVECTOR&& quaternion) :
      CamInputComponent{ std::move(position), std::move(quaternion), FREE }
   {}

   void CamFreeInputComponent::Handle(float tempsEcoule)
   {
      auto& GestionnaireDeSaisie = CMoteurWindows::GetInstance().GetGestionnaireDeSaisie();

      //rotation
      auto vertical = (GestionnaireDeSaisie.ToucheAppuyee(DIK_LEFT) - GestionnaireDeSaisie.ToucheAppuyee(DIK_RIGHT));
      auto horizontal = (GestionnaireDeSaisie.ToucheAppuyee(DIK_DOWN) - GestionnaireDeSaisie.ToucheAppuyee(DIK_UP));
      if (vertical != 0 || horizontal != 0) {
          orientation.rotateWithQuaternion(getQuaternionRotation(getAxeY(), vertical * tempsEcoule));
          DirectX::XMVECTOR cross = XMVector3Cross(orientation.getDirection(), orientation.getUp());
          orientation.rotateWithQuaternion(getQuaternionRotation(cross, horizontal * tempsEcoule ));
      }

      // mouvement
      auto avantArriere = static_cast<float>(GestionnaireDeSaisie.ToucheAppuyee(DIK_W) - GestionnaireDeSaisie.ToucheAppuyee(DIK_S));
      auto gaucheDroite = static_cast<float>(GestionnaireDeSaisie.ToucheAppuyee(DIK_A) - GestionnaireDeSaisie.ToucheAppuyee(DIK_D));
      auto hautBas = static_cast<float>(GestionnaireDeSaisie.ToucheAppuyee(DIK_LSHIFT) - GestionnaireDeSaisie.ToucheAppuyee(DIK_LCONTROL));
      if (avantArriere != 0 || gaucheDroite != 0 || hautBas != 0) {
         orientation.translate(
            (orientation.getDirection() * avantArriere * tempsEcoule * VITESSE_DEPLACEMENT) +
            (XMVector3Cross(orientation.getDirection(), orientation.getUp()) * gaucheDroite * tempsEcoule * VITESSE_DEPLACEMENT) +
            (orientation.getUp() * hautBas * tempsEcoule * VITESSE_DEPLACEMENT * 3)
         );
      }
   }
}
