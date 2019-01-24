#ifndef SKYBOX_H
#define SKYBOX_H

#include "Formes/Orientation.h"
#include "Component/MeshComponent.h"
#include "Formes/Orientation.h"
#include "Formes/CGameObject.h"
#include "dispositifD3D11.h"

#include <vector>
#include <memory>

namespace PM3D {

   class Skybox : public CGameObject
   {
   public:
      MeshComponent * skyboxMesh;
      Skybox(const Orientation & _orientation, CDispositifD3D11 * _pDispositif);

      ~Skybox()
      {
      }

      virtual void Draw(ID3D11ShaderResourceView* pDepthShaderResourceView = nullptr) {
         skyboxMesh->DrawSkybox(matWorld);

      }
      void Init(CDispositifD3D11 * _pDispositif);
      void Update(const float tempsEcoule) override;
      // virtual void Update(const float tempsEcoule) override;
   private:

   };


}
#endif // SKYBOX_H