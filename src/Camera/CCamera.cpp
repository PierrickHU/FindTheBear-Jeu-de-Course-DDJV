#include "stdafx.h"
#include "CCamera.h"
#include "MoteurWindows.h"
#include "toolsMath.h"
#include <math.h> 

namespace PM3D {

   void CCamera::Init(CDispositifD3D11* pDispositif)
   {
      frustrum = { DirectX::XM_PI / 4,  static_cast<float>(pDispositif->GetLargeur()) / static_cast<float>(pDispositif->GetHauteur()), 1.f, 20000.f };
      freeControl = std::make_unique<CamFreeInputComponent>( DirectX::XMVECTOR{ 0.0f, 10.0f, -10.0f, 1.0f }, DirectX::XMQuaternionIdentity() );
      levelControl = std::make_unique<CamThirdPersonInputComponent>();
		firstPersonControl= std::make_unique<CamFirstPersonComponent>();
      actualControl = levelControl.get();
      // Init matView
      actualControl->m_MatView = DirectX::XMMatrixLookAtLH(
         actualControl->orientation.getPosition(),
         actualControl->orientation.getPosition() + actualControl->orientation.getDirection(),
         actualControl->orientation.getUp());
   }

   void CCamera::Update(DirectX::XMMATRIX& matViewProj, const float tempsEcoule) {
       HandleInput(tempsEcoule);
		 
       actualControl->m_MatView = DirectX::XMMatrixLookAtLH(
           actualControl->orientation.getPosition(),
           actualControl->orientation.getPosition() + actualControl->orientation.getDirection(),
           actualControl->orientation.getUp());

       frustrum.Update(matViewProj, actualControl->m_MatView);
   }

   void CCamera::HandleInput(const float tempsEcoule) {
      auto& GestionnaireDeSaisie = CMoteurWindows::GetInstance().GetGestionnaireDeSaisie();

      if (GestionnaireDeSaisie.ToucheAppuyee(DIK_1))
      {
          actualControl = levelControl.get();
      }
      else if (GestionnaireDeSaisie.ToucheAppuyee(DIK_2))
      {
          actualControl = freeControl.get();
      }
		else if (GestionnaireDeSaisie.ToucheAppuyee(DIK_3))
		{
			actualControl = firstPersonControl.get();
		}

      actualControl->Handle(tempsEcoule);	
   }

   const FrustrumView& CCamera::getFrustrumView() const {
       return frustrum;
   }

   Orientation& CCamera::getOrientation() {
       return actualControl->orientation;
   }

   const DirectX::XMMATRIX& CCamera::GetMatView() const {
       return actualControl->m_MatView;
   }

   bool CCamera::isLevel() const {
       return actualControl->inputType == LEVEL;
   }

	bool CCamera::isFirstPerson() const {
		return actualControl->inputType == FPS;
	}
}