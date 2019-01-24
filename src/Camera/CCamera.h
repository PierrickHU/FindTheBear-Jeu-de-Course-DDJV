#ifndef CAMERA_H
#define CAMERA_H

#include "Component/CamInputComponent.h"
#include "Component/CamFreeInputComponent.h"
#include "Component/CamThirdPersonInputComponent.h"
#include "Component/CamFirstPersonComponent.h"

#include "Formes/Orientation.h"
#include "FrustrumView.h"
#include "dispositifD3D11.h"

namespace PM3D
{
   class CCamera
   {
   private:
      FrustrumView frustrum;
      std::unique_ptr<CamFreeInputComponent> freeControl;
      std::unique_ptr<CamThirdPersonInputComponent> levelControl;
      std::unique_ptr<CamFirstPersonComponent> firstPersonControl;
      CamInputComponent* actualControl;

      void HandleInput(const float);
   public:
      CCamera() = default;
      ~CCamera() = default;

      void Init(CDispositifD3D11* pDispositif);
      void Update(DirectX::XMMATRIX& matViewProj, const float tempsEcoule);

      const FrustrumView& getFrustrumView() const;
      Orientation& getOrientation();
      const DirectX::XMMATRIX& GetMatView() const;

      bool isLevel() const;
      bool isFirstPerson() const;
   };
}
#endif // !CAMERA_H