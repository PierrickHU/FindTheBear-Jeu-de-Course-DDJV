#ifndef C_GAME_OBJECT_H
#define C_GAME_OBJECT_H

#include "Formes/Orientation.h"

namespace PM3D
{
   class CGameObject
   {
   public:
      DirectX::XMMATRIX matWorld;
      Orientation orientation;

      CGameObject() = default;
      CGameObject(const Orientation& _orientation) :
          orientation{ _orientation }, matWorld{ DirectX::XMMatrixScalingFromVector(convertIntoVector(_orientation.getScale())) }
      {}
      virtual ~CGameObject() = default;

      void calculMatriceWorld()
      {
          matWorld = orientation.calculMatriceWorld();
      }

      virtual void Draw(ID3D11ShaderResourceView* pDepthShaderResourceView = nullptr) = 0;
      virtual void DrawShadow(const DirectX::XMMATRIX matViewProjLight) {};
      virtual void Update(const float) {};
   };
}
#endif // !C_GAME_OBJECT_H


