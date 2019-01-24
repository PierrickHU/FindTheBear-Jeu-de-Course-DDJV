#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "dispositif.h"
#include "Texture.h"
#include "CAssimpLoader.h"
#include "dispositifD3D11.h"
#include "d3dx11effect.h"
#include "../Formes/ShadersParams.h"
#include "../Formes/CSommet.h"
#include "../Formes/CGameObject.h"
#include "../Material.h"

using DirectX::XMMATRIX;
using std::vector;

namespace PM3D
{
     class MeshComponent
     {
     public:
          std::vector<CSommet> m_Sommets;
          std::vector<uint16_t> m_Indices;
          vector<Material> materials;			// Vecteur des matériaux
          LPCWSTR shaderFilename;

          MeshComponent() = default;
          MeshComponent(IChargeur& chargeur);
          MeshComponent(vector<CSommet> sommets, vector<uint16_t> m_Indices, vector<Material> materials);
          void Init(CDispositifD3D11* pDispositif);

          void InitSkybox(CDispositifD3D11 * _pDispositif);

          virtual ~MeshComponent();

          //void InitDepthBuffer();
          void Draw(const DirectX::XMMATRIX& matriceWorld, ID3D11ShaderResourceView* pDepthShaderResourceView = nullptr);
          void DrawShadow(const DirectX::XMMATRIX& matriceWorld, const XMMATRIX matViewProjLight);
			 void DrawNormalMap(const DirectX::XMMATRIX & matWorld, const XMMATRIX matViewProjLight);
          void InitEffetSkybox();
          void DrawSkybox(const DirectX::XMMATRIX & matriceWorld);
          void InitBufferSkybox();
			 void setBTex(int _btex);
          void TransfertObjet(IChargeur& chargeur);
          void InitEffet();
          void InitBuffer();
          CDispositifD3D11* getDispositif();
     protected:
          // Pour le dessin
          CDispositifD3D11* pDispositif;		// On prend en note le dispositif
          ID3D11Buffer* pVertexBuffer = nullptr;
          ID3D11Buffer* pIndexBuffer = nullptr;
          ID3D11SamplerState* pSampleState = nullptr;
          ID3D11Buffer* pConstantBuffer = nullptr;
          ID3DX11Effect* pEffet = nullptr;
          ID3DX11EffectTechnique* pTechnique = nullptr;
          ID3DX11EffectPass* pPasse = nullptr;
          ID3D11InputLayout* pVertexLayout = nullptr;

          // Pour les ombres
          ID3D11InputLayout* pVertexLayoutShadow = nullptr;
          ID3D11Texture2D* pDepthTexture = nullptr;    // texture de profondeur
          ID3D11DepthStencilView* pDepthStencilView = nullptr;
          ID3D11ShaderResourceView* pDepthShaderResourceView = nullptr; 




          // Les sous-objets
          int NombreSubmesh;				    // Nombre de sous-objets dans le mesh
          vector<int> SubmeshMaterialIndex;	// Index des matériaux
          vector<int> SubmeshIndex;			// Index des sous-objets

          ShadersParams sp;                 // Les parametres des shaders utilisés dans draw
     };
}

#endif // !MESH_COMPONENT_H


