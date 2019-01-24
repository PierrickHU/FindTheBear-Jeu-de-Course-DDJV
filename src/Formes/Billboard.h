#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "Formes/CGameObject.h"
#include "Formes/Orientation.h"
#include "dispositifD3D11.h"
#include "Component/BillboardMeshComponent.h"

#include <memory>

namespace PM3D
{
   class Billboard: public CGameObject
   {
       const float neededAngle = DirectX::XM_PIDIV4;
   public:
       
      std::unique_ptr<BillboardMeshComponent> mesh;
      DirectX::XMVECTOR test;
      DirectX::XMVECTOR offset;

      Billboard(CDispositifD3D11* _pDispositif, const Orientation& _orientation, const DirectX::XMVECTOR& offset, const std::wstring& filename);
      virtual ~Billboard() = default;

      virtual void Draw(ID3D11ShaderResourceView* pDepthShaderResourceView = nullptr);
      virtual void Update(const float);
   };
}
#endif // !C_GAME_OBJECT_H


