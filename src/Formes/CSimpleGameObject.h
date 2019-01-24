#ifndef C_SIMPLE_GAME_OBJECT_H
#define C_SIMPLE_GAME_OBJECT_H

#include "CGameObject.h"
#include "../Component/MeshComponent.h"
#include "../Formes/Orientation.h"
#include "../RessourcesManager.h"
#include "../dispositifD3D11.h"
#include "../ScenePhysique.h"


namespace PM3D
{
     class CSimpleGameObject : public CGameObject
     {
     public:
          CSimpleGameObject(Orientation& orientation, ModelType type, CDispositifD3D11* _pDispositif, ScenePhysique& scenePhysique);
          MeshComponent* mesh;
			 physx::PxShape* shape = NULL;
			 physx::PxRigidDynamic* rigidbody = NULL;
          virtual void Draw(ID3D11ShaderResourceView* pDepthShaderResourceView) override;
          void DrawShadow(const XMMATRIX matViewProjLight);
     };

} // namespace PM3D
#endif // C_SOMMET_H