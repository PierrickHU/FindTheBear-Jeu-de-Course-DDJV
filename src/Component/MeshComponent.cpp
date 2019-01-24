#include "stdafx.h"
#include "Component/MeshComponent.h"
#include "MoteurWindows.h"
#include "util.h"
#include "Resource.h"
#include "toolsMath.h"
#include "RandomGenerateur.h"
#include "../Formes/ShadersParams.h"
#include "../Formes/Orientation.h"
#include "ShaderManager.h"
#include "../Camera/LumiereManager.h"
#include "GameManager.h"
#include "../CScene.h"

#include <string>
#include <numeric>

using namespace UtilitairesDX;
using namespace DirectX;
using namespace std;

namespace PM3D
{
   MeshComponent::MeshComponent(IChargeur& _chargeur)
   {
      TransfertObjet(_chargeur);
   }

   MeshComponent::MeshComponent(vector<CSommet> sommets, vector<uint16_t> indices, vector<Material> materiels) {
      // 1. SOMMETS a) Créations des sommets dans un tableau temporaire
      for (int i = 0; i < sommets.size(); ++i)
      {
         m_Sommets.emplace_back(sommets[i].m_Position, sommets[i].m_Normal, sommets[i].m_TexCoord);
      }

      //recentrer les points au centre parce que certains fichiers dds sont pas centré
      DirectX::XMVECTOR moyenne = std::accumulate(m_Sommets.begin(), m_Sommets.end(), DirectX::XMVECTOR{ 0.f,0.f,0.f }, [](DirectX::XMVECTOR acc, const CSommet& s) {
         return DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&s.m_Position), acc);
      }) / m_Sommets.size();

      std::for_each(m_Sommets.begin(), m_Sommets.end(), [&moyenne](CSommet& s) {
         DirectX::XMStoreFloat3(&s.m_Position, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&s.m_Position), moyenne));
      });

      // Normaliser la taille du mesh pour qu'il ait une taille de 1 au maximum sur chacune de ses longueurs
#undef max
      float distanceMax = std::accumulate(m_Sommets.begin(), m_Sommets.end(), 0.f, [](float acc, const CSommet& s) {
         XMFLOAT3 a = s.m_Position;
         return std::max(std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z), acc);
      });
      std::for_each(m_Sommets.begin(), m_Sommets.end(), [&distanceMax](CSommet& s) {
         DirectX::XMStoreFloat3(&s.m_Position, DirectX::XMVectorDivide(DirectX::XMLoadFloat3(&s.m_Position), { distanceMax, distanceMax, distanceMax }));
      });

      for (int i = 0; i < m_Indices.size(); i += 3) {
         m_Indices.push_back(indices[i]);
         m_Indices.push_back(indices[i + 1]);
         m_Indices.push_back(indices[i + 2]);
      }

      // 3. Les sous-objets
      NombreSubmesh = 1;
      SubmeshIndex.push_back(0);

      //    Début de chaque sous-objet et un pour la fin
      //chargeur.CopieSubsetIndex(SubmeshIndex);

      // 4. MATERIAU
      // 4b) Copie des matériaux dans la version locale
      for (int i = 1; i < materiels.size(); ++i)
      {
         materials.push_back(materiels[i]);
      }

      //// 4c) Trouver l'index du materiau pour chaque sous-ensemble
      SubmeshMaterialIndex.push_back(0);

      // 4d) Chargement des textures
      RessourcesManager& TexturesManager = CMoteurWindows::GetInstance().GetRessourcesManager();

      for (unsigned int i = 0; i < materials.size(); ++i)
      {
         if (materials[i].NomFichierTexture != "")
         {
            std::wstring ws;
            ws.assign(materials[i].NomFichierTexture.begin(), materials[i].NomFichierTexture.end());

            materials[i].texture = TexturesManager.GetTexture(ws.c_str());
            sp.bTex = 1;
         }
         else
         {
            sp.bTex = 0;
         }
      }
   }

   void MeshComponent::Init(CDispositifD3D11* _pDispositif)
   {
      pDispositif = _pDispositif;
      InitBuffer();
      InitEffet();
   }

    void MeshComponent::InitSkybox(CDispositifD3D11* _pDispositif)
    {
        pDispositif = _pDispositif;
        InitBuffer();
        InitEffetSkybox();
    }

   MeshComponent::~MeshComponent()
   {
      SubmeshMaterialIndex.clear();
      SubmeshIndex.clear();
      materials.clear();

      DXRelacher(pConstantBuffer);
      DXRelacher(pSampleState);

      //DXRelacher(pEffet);
      DXRelacher(pVertexLayout);
      DXRelacher(pIndexBuffer);
      DXRelacher(pVertexBuffer);
   }

   void MeshComponent::TransfertObjet(IChargeur& chargeur)
   {
      // 1. SOMMETS a) Créations des sommets dans un tableau temporaire
      vector<unsigned int> SommetDansMesh;

      for (int iSubmesh = 0; iSubmesh < chargeur.GetNombreSubmesh(); ++iSubmesh)
      {
         SommetDansMesh.push_back(m_Sommets.size());
         for (int i = 0; i < chargeur.GetNombreSommetsSubmesh(iSubmesh); ++i)
         {
            m_Sommets.emplace_back(chargeur.GetPosition(iSubmesh, i),
               chargeur.GetNormale(iSubmesh, i),
               chargeur.GetCoordTex(iSubmesh, i));
         }
      }

      //recentrer les points au centre parce que certains fichiers dds sont pas centré
      DirectX::XMVECTOR moyenne = std::accumulate(m_Sommets.begin(), m_Sommets.end(), DirectX::XMVECTOR{ 0.f,0.f,0.f }, [](DirectX::XMVECTOR acc, const CSommet& s) {
         return DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&s.m_Position), acc);
      }) / m_Sommets.size();

      std::for_each(m_Sommets.begin(), m_Sommets.end(), [&moyenne](CSommet& s) {
         DirectX::XMStoreFloat3(&s.m_Position, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&s.m_Position), moyenne));
      });

      // Normaliser la taille du mesh pour qu'il ait une taille de 1 au maximum sur chacune de ses longueurs
#undef max
      float distanceMax = std::accumulate(m_Sommets.begin(), m_Sommets.end(), 0.f, [](float acc, const CSommet& s) {
         XMFLOAT3 a = s.m_Position;
         return std::max(std::max(a.x, std::max(a.y, a.z)), acc); // avant je contractait dans la sphère unité, maintenant c'est dans le cube unité !
      });
      std::for_each(m_Sommets.begin(), m_Sommets.end(), [&distanceMax](CSommet& s) {
         DirectX::XMStoreFloat3(&s.m_Position, DirectX::XMVectorDivide(DirectX::XMLoadFloat3(&s.m_Position), { distanceMax, distanceMax, distanceMax }));
      });

      for (int iSubmesh = 0; iSubmesh < chargeur.GetNombreSubmesh(); ++iSubmesh)
      {
         SubmeshIndex.push_back(static_cast<uint16_t>(m_Indices.size()));
         for (int i = 0; i < chargeur.GetNombrePolygonesSubmesh(iSubmesh); ++i)
         {
            for (int j = 0; j < 3; ++j)
            {
               m_Indices.push_back(static_cast<uint16_t>(SommetDansMesh[iSubmesh] + chargeur.GetIndice(iSubmesh, i, j)));
            }
         }
      }
      SubmeshIndex.push_back(static_cast<uint16_t>(m_Indices.size()));

      // 3. Les sous-objets
      NombreSubmesh = chargeur.GetNombreSubmesh();

      //    Début de chaque sous-objet et un pour la fin
      //chargeur.CopieSubsetIndex(SubmeshIndex);

      // 4. MATERIAU
      // 4b) Copie des matériaux dans la version locale
      for (int i = 1; i < chargeur.GetNombreMaterial(); ++i)
      {
         Material mat;
         chargeur.GetMaterial(i, mat.NomFichierTexture,
            mat.NomMateriau,
            mat.Ambient,
            mat.Diffuse,
            mat.Specular,
            mat.Puissance);

         materials.push_back(mat);
      }


      //// 4c) Trouver l'index du materiau pour chaque sous-ensemble
      for (int i = 0; i < chargeur.GetNombreSubmesh(); ++i)
      {
         SubmeshMaterialIndex.push_back(chargeur.GetMaterialIndex(i) - 1);
      }

      // 4d) Chargement des textures
      RessourcesManager& TexturesManager = CMoteurWindows::GetInstance().GetRessourcesManager();

      for (unsigned int i = 0; i < materials.size(); ++i)
      {
         if (materials[i].NomFichierTexture != "")
         {
            std::wstring ws;
            ws.assign(materials[i].NomFichierTexture.begin(),
               materials[i].NomFichierTexture.end());

            materials[i].texture = TexturesManager.GetTexture(ws.c_str());
            sp.bTex = 1;
         }
         else
         {
            sp.bTex = 0;
         }
      }
   }

   void MeshComponent::InitBuffer() {
      auto pD3DDevice = pDispositif->GetD3DDevice();

      // VERTEX BUFFER
      D3D11_BUFFER_DESC bd;
      ZeroMemory(&bd, sizeof(bd));
      bd.Usage = D3D11_USAGE_IMMUTABLE;
      bd.ByteWidth = static_cast<UINT>(sizeof(CSommet)*m_Sommets.size());
      bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      bd.CPUAccessFlags = 0;
      D3D11_SUBRESOURCE_DATA InitData;
      ZeroMemory(&InitData, sizeof(InitData));
      InitData.pSysMem = m_Sommets.data();
      DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);

      // INDEX BUFFER
      ZeroMemory(&bd, sizeof(bd));
      bd.Usage = D3D11_USAGE_IMMUTABLE;
      bd.ByteWidth = static_cast<UINT>(sizeof(uint16_t)*m_Indices.size());
      bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
      bd.CPUAccessFlags = 0;
      ZeroMemory(&InitData, sizeof(InitData));
      InitData.pSysMem = m_Indices.data();
      DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer), DXE_CREATIONINDEXBUFFER);

   }

   void MeshComponent::InitEffet() {
      // Compilation et chargement du vertex shader
      auto pD3DDevice = pDispositif->GetD3DDevice();

      // Creation d'un tampon pour les constantes du VS
      D3D11_BUFFER_DESC bd;
      ZeroMemory(&bd, sizeof(bd));
      bd.Usage = D3D11_USAGE_DEFAULT;
      bd.ByteWidth = sizeof(ShadersParams);
      bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
      bd.CPUAccessFlags = 0;
      pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

      pEffet = CMoteurWindows::GetInstance().GetShaderManager().GetEffet(shaderFilename);

        // Creer l'organisation des sommets pour le VS de notre effet
        D3DX11_PASS_SHADER_DESC passeVSDesc; // contient la description de la passe
        D3DX11_EFFECT_SHADER_DESC effectVSDesc; // contient la description de l'effet
        const void *vsCodePtr; // pointeur vers le code compile
        unsigned vsCodeLen; // taille du le code compile
      // 1 pour shadowmap
      // on recupere la technique
        pTechnique = pEffet->GetTechniqueByName("ShadowMap");
        // on recupere la passe
        pPasse = pTechnique->GetPassByIndex(0);
        // on recupere la description du vertex shader
        pPasse->GetVertexShaderDesc(&passeVSDesc);
        // on recupere la description de l'effet
        passeVSDesc.pShaderVariable->GetShaderDesc(passeVSDesc.ShaderIndex, &effectVSDesc);
        // on recupere les infos pour le code compile
        vsCodePtr = effectVSDesc.pBytecode;
        vsCodeLen = effectVSDesc.BytecodeLength;
        // on valorise l'input layout
        pVertexLayoutShadow = nullptr;
        DXEssayer(pD3DDevice->CreateInputLayout(CSommet::layout,
            CSommet::numElements,
            vsCodePtr,
            vsCodeLen,
            &pVertexLayoutShadow),
            DXE_CREATIONLAYOUT);


      // 2 pour miniphong
      // on recupere la technique
      pTechnique = pEffet->GetTechniqueByName("MiniPhong");
      // on recupere la passe
      pPasse = pTechnique->GetPassByIndex(0);
      // on recupere la description du vertex shader
      pPasse->GetVertexShaderDesc(&passeVSDesc);
      // on recupere la description de l'effet
      passeVSDesc.pShaderVariable->GetShaderDesc(passeVSDesc.ShaderIndex, &effectVSDesc);
      // on recupere les infos pour le code compile
      vsCodePtr = effectVSDesc.pBytecode;
      vsCodeLen = effectVSDesc.BytecodeLength;
      // on valorise l'input layout
      pVertexLayout = nullptr;
      DXEssayer(pD3DDevice->CreateInputLayout(CSommet::layout,
         CSommet::numElements,
         vsCodePtr,
         vsCodeLen,
         &pVertexLayout),
         DXE_CREATIONLAYOUT);

      // Initialisation des parametres de sampling de la texture
      D3D11_SAMPLER_DESC samplerDesc;

      samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
      samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
      samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
      samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
      samplerDesc.MipLODBias = 0.0f;
      samplerDesc.MaxAnisotropy = 4;
      samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
      samplerDesc.BorderColor[0] = 0;
      samplerDesc.BorderColor[1] = 0;
      samplerDesc.BorderColor[2] = 0;
      samplerDesc.BorderColor[3] = 0;
      samplerDesc.MinLOD = 0;
      samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

      // Creation de l'etat de sampling
      pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);
   }

   void MeshComponent::Draw(const DirectX::XMMATRIX& matWorld, ID3D11ShaderResourceView* pDepthShaderResourceView)
   {
      auto pImmediateContext = pDispositif->GetImmediateContext();

      // Choisir la topologie des primitives
      pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // Index buffer
      pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

      // Vertex buffer
      const UINT stride = sizeof(CSommet);
      const UINT offset = 0;
      pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

      // Choix de la technique
      pTechnique = pEffet->GetTechniqueByName("MiniPhong");
      pPasse = pTechnique->GetPassByIndex(0);

      pImmediateContext->IASetInputLayout(pVertexLayout);

      DirectX::XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
      sp.matWorldViewProj = DirectX::XMMatrixTranspose(matWorld * viewProj);
      sp.matWorld = matWorld;

      auto m = materials[0];
      auto monde = CMoteurWindows().GetInstance().getGameManager().getMondeActuel();
      auto typeLumiere = (monde == GameManager::Monde::CAUCHEMAR) ? LumiereManager::LightType::MINIPHONG_CAUCHEMARD : LumiereManager::LightType::MINIPHONG_REVE;
      //Tunnel
      CMoteurWindows::GetInstance().GetScene().getVehicule()->orientation;
      Chateau chateau = CMoteurWindows::GetInstance().GetScene().getChateau();
      auto posVehicule = CMoteurWindows::GetInstance().getVehicule()->orientation;
      bool isInTunnel = (monde == GameManager::Monde::CAUCHEMAR) ? true : chateau.isInTunnel(posVehicule.getPositionX(), posVehicule.getPositionZ());
      if (isInTunnel) {
         typeLumiere = (monde == GameManager::Monde::CAUCHEMAR) ? LumiereManager::LightType::TUNNEL_CAUCHEMARD : LumiereManager::LightType::TUNNEL_REVE;
      }

      auto lumiere = CMoteurWindows().GetInstance().GetScene().getLumiereManager().getLumiere(typeLumiere);
      auto couleurLumiere = lumiere.getCouleur();
      sp.posLumiere = lumiere.orientation.getPosition();
      sp.dirLumiere = -lumiere.orientation.getDirection();
      sp.vCamera = CMoteurWindows::GetInstance().GetCamera().getOrientation().getPosition();
      sp.vAEcl = couleurLumiere.ambient;
      sp.vAMat = DirectX::XMLoadFloat4(&m.Ambient);
      sp.vDEcl = couleurLumiere.diffus;
      sp.vDMat = DirectX::XMLoadFloat4(&m.Diffuse);
      sp.vSEcl = couleurLumiere.speculaire;
      sp.vSMat = DirectX::XMLoadFloat4(&m.Specular);
      CTerrain* terrain = CMoteurWindows::GetInstance().getTerrain();
      const XMFLOAT2* centreTerrain = new XMFLOAT2{ terrain->getMiddlePosition().x, terrain->getMiddlePosition().z };
      sp.centreTerrain = DirectX::XMLoadFloat2(centreTerrain);
      sp.puissance = m.Puissance;
      sp.bTunnel = (isInTunnel) ? 1 : 0;
      //sp.bTunnel = 5;

      ID3DX11EffectConstantBuffer* pShaderConstBuffer = pEffet->GetConstantBufferByName("param");
      pShaderConstBuffer = pEffet->GetConstantBufferByName("param");  // Nous n'avons qu'un seul CBuffer
      pShaderConstBuffer->SetConstantBuffer(pConstantBuffer);
      pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);
      //TODO - séparer mode texture on/off
      if (sp.bTex)
      {
         // Activation de la texture
         ID3DX11EffectShaderResourceVariable* variableTexture;
         variableTexture = pEffet->GetVariableByName("texture0")->AsShaderResource();
         variableTexture->SetResource(m.texture->ressource);
         if (materials.size() > 1) {
            variableTexture = pEffet->GetVariableByName("texture1")->AsShaderResource();
            variableTexture->SetResource(materials[1].texture->ressource);
            if (materials.size() > 2) {
               variableTexture = pEffet->GetVariableByName("texture2")->AsShaderResource();
               variableTexture->SetResource(materials[2].texture->ressource);
               if (materials.size() > 3) {
                  variableTexture = pEffet->GetVariableByName("normalTexture0")->AsShaderResource();
                  variableTexture->SetResource(materials[3].texture->ressource);
                  if (materials.size() > 4) {
                     variableTexture = pEffet->GetVariableByName("normalTexture1")->AsShaderResource();
                     variableTexture->SetResource(materials[4].texture->ressource);
                     if (materials.size() > 5) {
                        variableTexture = pEffet->GetVariableByName("normalTexture2")->AsShaderResource();
                        variableTexture->SetResource(materials[5].texture->ressource);
                     }
                  }
               }
            }
         }
      }

      // Le sampler state
      ID3DX11EffectSamplerVariable* variableSampler;
      variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
      variableSampler->SetSampler(0, pSampleState);

      if (pDepthShaderResourceView) {
         // la shadow texture
         ID3DX11EffectShaderResourceVariable* pShadowMap;
         pShadowMap = pEffet->GetVariableByName("ShadowTexture")->AsShaderResource();
         pShadowMap->SetResource(pDepthShaderResourceView);
      }

      // **** Rendu de l'objet
      pPasse->Apply(0, pImmediateContext);

      pImmediateContext->DrawIndexed(static_cast<UINT>(m_Indices.size()), 0, 0);
   }

   void MeshComponent::DrawShadow(const DirectX::XMMATRIX& matWorld, const XMMATRIX matViewProjLight)
   {
      auto pImmediateContext = pDispositif->GetImmediateContext();

      // Choisir la topologie des primitives
      pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // Index buffer
      pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

      // Vertex buffer
      const UINT stride = sizeof(CSommet);
      const UINT offset = 0;
      pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

      // Choix de la technique
      pTechnique = pEffet->GetTechniqueByName("ShadowMap");
      pPasse = pTechnique->GetPassByIndex(0);

      // input layout des sommets
      pImmediateContext->IASetInputLayout(pVertexLayoutShadow);

      // Initialiser et sélectionner les «constantes» de l'effet
      sp.matWorldViewProjLight = getMatriceTransposition(matWorld * matViewProjLight);

      ID3DX11EffectConstantBuffer* pShaderConstBuffer = pEffet->GetConstantBufferByName("param");
      pShaderConstBuffer->SetConstantBuffer(pConstantBuffer);
      pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

      // IMPORTANT pour ajuster les param.
      pPasse->Apply(0, pImmediateContext);

      pImmediateContext->DrawIndexed(static_cast<UINT>(m_Indices.size()), 0, 0);
   }
   void MeshComponent::DrawNormalMap(const DirectX::XMMATRIX& matWorld, const XMMATRIX matViewProjLight)
   {
      auto pImmediateContext = pDispositif->GetImmediateContext();

      // Choisir la topologie des primitives
      pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // Index buffer
      pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

      // Vertex buffer
      const UINT stride = sizeof(CSommet);
      const UINT offset = 0;
      pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

      // Choix de la technique
      pTechnique = pEffet->GetTechniqueByName("BumpMapping");
      pPasse = pTechnique->GetPassByIndex(0);

      // input layout des sommets
      pImmediateContext->IASetInputLayout(pVertexLayoutShadow);

      // Initialiser et sélectionner les «constantes» de l'effet
      sp.matWorldViewProjLight = getMatriceTransposition(matWorld * matViewProjLight);

      ID3DX11EffectConstantBuffer* pShaderConstBuffer = pEffet->GetConstantBufferByName("param");
      pShaderConstBuffer->SetConstantBuffer(pConstantBuffer);
      pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

      // IMPORTANT pour ajuster les param.
      pPasse->Apply(0, pImmediateContext);

      pImmediateContext->DrawIndexed(static_cast<UINT>(m_Indices.size()), 0, 0);
   }

    void MeshComponent::InitEffetSkybox() {
        // Compilation et chargement du vertex shader
        auto pD3DDevice = pDispositif->GetD3DDevice();

        // Creation d'un tampon pour les constantes du VS
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(ShadersParams);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);


        pEffet = CMoteurWindows::GetInstance().GetShaderManager().GetEffet(shaderFilename);

        pTechnique = pEffet->GetTechniqueByIndex(0);
        pPasse = pTechnique->GetPassByIndex(0);

        // Creer l'organisation des sommets pour le VS de notre effet
        D3DX11_PASS_SHADER_DESC effectVSDesc;
        pPasse->GetVertexShaderDesc(&effectVSDesc);

        D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
        effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex, &effectVSDesc2);

        const void *vsCodePtr = effectVSDesc2.pBytecode;
        const unsigned vsCodeLen = effectVSDesc2.BytecodeLength;

        pVertexLayout = nullptr;
        DXEssayer(pD3DDevice->CreateInputLayout(CSommet::layout,
            CSommet::numElements,
            vsCodePtr,
            vsCodeLen,
            &pVertexLayout),
            DXE_CREATIONLAYOUT);

        // Initialisation des param�tres de sampling de la texture
        D3D11_SAMPLER_DESC samplerDesc;

        samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 4;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        // Cr�ation de l'�tat de sampling
        pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);
    }
    void MeshComponent::DrawSkybox(const DirectX::XMMATRIX& matriceWorld)
    {
        auto pImmediateContext = pDispositif->GetImmediateContext();

        pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        const UINT stride = sizeof(CSommet);
        const UINT offset = 0;
        pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
        pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        pImmediateContext->IASetInputLayout(pVertexLayout);

        DirectX::XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
        sp.matWorldViewProj = DirectX::XMMatrixTranspose(matriceWorld * viewProj);
        sp.matWorld = matriceWorld;

        auto m = materials[0];
        sp.dirLumiere = DirectX::XMVectorSet(4.0f, 30.0f, 4.0f, 1.0f);
        sp.vCamera = CMoteurWindows::GetInstance().GetCamera().getOrientation().getPosition();
        sp.vAEcl = DirectX::XMVectorSet(0.7f, 0.7f, 0.7f, 1.0f);
        sp.vAMat = DirectX::XMLoadFloat4(&m.Ambient);
        sp.vDEcl = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
        sp.vDMat = DirectX::XMLoadFloat4(&m.Diffuse);
        sp.vSEcl = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
        sp.vSMat = DirectX::XMLoadFloat4(&m.Specular);
        pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

        auto pCB = pEffet->GetConstantBufferByName("param");  // Nous n'avons qu'un seul CBuffer
        pCB->SetConstantBuffer(pConstantBuffer);

        //TODO - séparer mode texture on/off
        if (sp.bTex)
        {
            // Activation de la texture
            ID3DX11EffectShaderResourceVariable* variableTexture;
            variableTexture = pEffet->GetVariableByName("texture0")->AsShaderResource();
            variableTexture->SetResource(m.texture->ressource);

        }

        // Le sampler state
        ID3DX11EffectSamplerVariable* variableSampler;
        variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
        variableSampler->SetSampler(0, pSampleState);
         

        auto variableCauchemar = pEffet->GetVariableByName("cauchemar")->AsScalar();
        variableCauchemar->SetInt(CMoteurWindows::GetInstance().getGameManager().getMondeActuel());
        // **** Rendu de l'objet
        pPasse->Apply(0, pImmediateContext);

        pImmediateContext->DrawIndexed(static_cast<UINT>(m_Indices.size()), 0, 0);
    }

    void MeshComponent::setBTex(int _btex) {
        sp.bTex = _btex;
    }

    CDispositifD3D11* MeshComponent::getDispositif() {
        return pDispositif;
    }
}





