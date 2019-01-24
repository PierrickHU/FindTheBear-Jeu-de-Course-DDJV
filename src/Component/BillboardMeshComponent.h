#ifndef BILLBOARD_MESH_COMPONENT_H
#define BILLBOARD_MESH_COMPONENT_H

#include "AfficheurSprite.h"
#include "../Formes/ShadersParams.h"
#include "../Formes/CGameObject.h"
#include "../dispositifD3D11.h"
#include "../Texture.h"
#include "../Material.h"
#include "../Formes/CSommet.h"

#include <vector>

namespace PM3D
{
    class BillboardMeshComponent
    {
    public:
        static CSommet sommets[6];
        LPCWSTR shaderFilename;

        void Draw(const DirectX::XMMATRIX& matriceWorld, ID3D11ShaderResourceView* pDepthShaderResourceView = nullptr);
        void InitEffet();
        void InitBuffer();

        BillboardMeshComponent(CDispositifD3D11* _pDispositif, const std::wstring& filename);
        virtual ~BillboardMeshComponent() = default;

        DirectX::XMVECTOR bottomVector;

    protected:
        Material material;
        CDispositifD3D11* pDispositif;		// On prend en note le dispositif
        ID3D11Buffer* pVertexBuffer = nullptr;
        ID3D11Buffer* pIndexBuffer = nullptr;
        ID3D11SamplerState* pSampleState = nullptr;
        ID3D11Buffer* pConstantBuffer = nullptr;
        ID3DX11Effect* pEffet = nullptr;
        ID3DX11EffectTechnique* pTechnique = nullptr;
        ID3DX11EffectPass* pPasse = nullptr;
        ID3D11InputLayout* pVertexLayout = nullptr;
        ShadersParams sp;                 // Les parametres des shaders utilisés dans draw
    };
}
#endif