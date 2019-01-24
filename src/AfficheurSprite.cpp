#include "stdafx.h"
#include "AfficheurSprite.h"
#include "Resource.h"
#include "MoteurWindows.h"
#include "util.h"
#include "dispositifD3D11.h"

namespace PM3D
{
   struct ShadersParams
   {
      XMMATRIX matWVP;	// la matrice totale 
   };

   // Definir l'organisation de notre sommet
   D3D11_INPUT_ELEMENT_DESC CSommetSprite::layout[] =
   {
       { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
       { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
   };

   UINT CSommetSprite::numElements = ARRAYSIZE(layout);

   CSommetSprite CAfficheurSprite::sommets[6] =
   {
       CSommetSprite(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
       CSommetSprite(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)),
       CSommetSprite(XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)),
       CSommetSprite(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
       CSommetSprite(XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)),
       CSommetSprite(XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f))
   };

   CAfficheurSprite::CAfficheurSprite(CDispositifD3D11* _pDispositif)
      : pDispositif(_pDispositif)
      , pVertexBuffer(nullptr)
      , pConstantBuffer(nullptr)
      , pEffet(nullptr)
      , pTechnique(nullptr)
      , pPasse(nullptr)
      , pVertexLayout(nullptr)
      , pSampleState(nullptr)
   {
      // Création du vertex buffer et copie des sommets
      ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

      D3D11_BUFFER_DESC bd;
      ZeroMemory(&bd, sizeof(bd));

      bd.Usage = D3D11_USAGE_IMMUTABLE;
      bd.ByteWidth = sizeof(sommets);
      bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      bd.CPUAccessFlags = 0;

      D3D11_SUBRESOURCE_DATA InitData;
      ZeroMemory(&InitData, sizeof(InitData));
      InitData.pSysMem = sommets;

      DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer),
         DXE_CREATIONVERTEXBUFFER);

      // Initialisation de l'effet
      InitEffet();
   }

   CAfficheurSprite ::~CAfficheurSprite()
   {
      DXRelacher(pConstantBuffer);
      DXRelacher(pSampleState);

      //DXRelacher(pEffet);
      DXRelacher(pVertexLayout);
      DXRelacher(pVertexBuffer);

      tabSprites.clear();
   }

   void CAfficheurSprite::InitEffet()
   {
      // Compilation et chargement du vertex shader
      ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

      // Création d'un tampon pour les constantes de l'effet
      D3D11_BUFFER_DESC bd;
      ZeroMemory(&bd, sizeof(bd));

      bd.Usage = D3D11_USAGE_DEFAULT;
      bd.ByteWidth = sizeof(ShadersParams);
      bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
      bd.CPUAccessFlags = 0;
      pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

      // Pour l'effet
      pEffet = CMoteurWindows::GetInstance().GetShaderManager().GetEffet(L"../ressources/shaders/Sprite1.fx");

      pTechnique = pEffet->GetTechniqueByName("AfficheSprite");
      pPasse = pTechnique->GetPassByIndex(0);

      // Créer l'organisation des sommets pour le VS de notre effet
      D3DX11_PASS_SHADER_DESC effectVSDesc;
      pPasse->GetVertexShaderDesc(&effectVSDesc);

      D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
      effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex,
         &effectVSDesc2);

      const void *vsCodePtr = effectVSDesc2.pBytecode;
      const unsigned vsCodeLen = effectVSDesc2.BytecodeLength;

      DXEssayer(pD3DDevice->CreateInputLayout(
         CSommetSprite::layout,
         CSommetSprite::numElements,
         vsCodePtr,
         vsCodeLen,
         &pVertexLayout),
         DXE_CREATIONLAYOUT);

      // Initialisation des paramètres de sampling de la texture
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

      // Création de l'état de sampling
      pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);
   }

   void CAfficheurSprite::Draw(ID3D11ShaderResourceView* pDepthShaderResourceView)
   {
      // Obtenir le contexte
      ID3D11DeviceContext* pImmediateContext =
         pDispositif->GetImmediateContext();

      // Choisir la topologie des primitives
      pImmediateContext->IASetPrimitiveTopology(
         D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // Source des sommets
      const UINT stride = sizeof(CSommetSprite);
      const UINT offset = 0;
      pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride,
         &offset);

      // input layout des sommets
      pImmediateContext->IASetInputLayout(pVertexLayout);

      // Le sampler state
      ID3DX11EffectSamplerVariable* variableSampler;
      variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
      variableSampler->SetSampler(0, pSampleState);

      ID3DX11EffectConstantBuffer* pCB =
         pEffet->GetConstantBufferByName("param");
      ID3DX11EffectShaderResourceVariable* variableTexture;
      variableTexture =
         pEffet->GetVariableByName("texture0")->AsShaderResource();

      pDispositif->ActiverMelangeAlpha();

      // Faire le rendu de tous nos sprites
      for (auto& sprite : tabSprites)
      {
         // Initialiser et sélectionner les «constantes» de l'effet
         ShadersParams sp;
         sp.matWVP = XMMatrixTranspose(sprite->matPosDim);
         pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

         pCB->SetConstantBuffer(pConstantBuffer);

         // Activation de la texture
         variableTexture->SetResource(sprite->pTextureD3D);

         pPasse->Apply(0, pImmediateContext);

         // **** Rendu de l'objet
         pImmediateContext->Draw(6, 0);
      }

      pDispositif->DesactiverMelangeAlpha();
   }

   void CAfficheurSprite::AjouterSprite(const std::wstring& textureFilename,
      int _x, int _y,
      int _dx, int _dy)
   {
      float x, y, dx, dy;
      float posX, posY;
      float facteurX, facteurY;

      std::unique_ptr<CSprite> pSprite = std::make_unique<CSprite>();;
      pSprite->pTextureD3D = CMoteurWindows::GetInstance().GetRessourcesManager().GetTexture(textureFilename)->ressource;

      // Obtenir les dimensions de la texture si _dx et _dy sont à 0;
      if (_dx == 0 && _dy == 0)
      {
         ID3D11Resource* pResource;
         ID3D11Texture2D *pTextureInterface = 0;
         pSprite->pTextureD3D->GetResource(&pResource);
         pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
         D3D11_TEXTURE2D_DESC desc;
         pTextureInterface->GetDesc(&desc);

         DXRelacher(pResource);
         DXRelacher(pTextureInterface);

         dx = float(desc.Width);
         dy = float(desc.Height);
      }
      else
      {
         dx = float(_dx);
         dy = float(_dy);
      }

      // Dimension en facteur
      facteurX = dx * 2.0f / pDispositif->GetLargeur();
      facteurY = dy * 2.0f / pDispositif->GetHauteur();

      // Position en coordonnées logiques
      // 0,0 pixel = -1,1   
      x = float(_x);
      y = float(_y);

      posX = x * 2.0f / pDispositif->GetLargeur() - 1.0f;
      posY = 1.0f - y * 2.0f / pDispositif->GetHauteur();

      pSprite->matPosDim = DirectX::XMMatrixScaling(facteurX, facteurY, 1.0f) *
         DirectX::XMMatrixTranslation(posX, posY, 0.0f);

      // On l'ajoute à notre vecteur
      tabSprites.push_back(std::move(pSprite));
   }

   void CAfficheurSprite::AjouterPanneau(const std::string& NomTexture,
      const XMFLOAT3& _position,
      float _dx, float _dy)
   {
      // Initialisation de la texture
      RessourcesManager& TexturesManager =
         CMoteurWindows::GetInstance().GetRessourcesManager();

      std::wstring ws(NomTexture.begin(), NomTexture.end());

      std::unique_ptr<CPanneau> pPanneau = std::make_unique<CPanneau>();
      pPanneau->pTextureD3D =
         TexturesManager.GetTexture(ws.c_str())->ressource;

      // Obtenir la dimension de la texture si _dx et _dy sont à 0;
      if (_dx == 0.0f && _dy == 0.0f)
      {
         ID3D11Resource* pResource;
         ID3D11Texture2D *pTextureInterface = 0;
         pPanneau->pTextureD3D->GetResource(&pResource);
         pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
         D3D11_TEXTURE2D_DESC desc;
         pTextureInterface->GetDesc(&desc);

         DXRelacher(pResource);
         DXRelacher(pTextureInterface);

         pPanneau->dimension.x = float(desc.Width);
         pPanneau->dimension.y = float(desc.Height);

         // Dimension en facteur
         pPanneau->dimension.x = pPanneau->dimension.x * 2.0f / pDispositif->GetLargeur();
         pPanneau->dimension.y = pPanneau->dimension.y * 2.0f / pDispositif->GetHauteur();
      }
      else
      {
         pPanneau->dimension.x = float(_dx);
         pPanneau->dimension.y = float(_dy);
      }

      // Position en coordonnées du monde
      const XMMATRIX& viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
      pPanneau->position = _position;

      pPanneau->matPosDim = DirectX::XMMatrixScaling(pPanneau->dimension.x,
         pPanneau->dimension.y, 1.0f) *
         DirectX::XMMatrixTranslation(pPanneau->position.x,
            pPanneau->position.y, pPanneau->position.z) *
         viewProj;

      // On l'ajoute à notre vecteur
      tabSprites.push_back(std::move(pPanneau));
   }

   void CAfficheurSprite::AjouterSpriteTexte(ID3D11ShaderResourceView* pTexture, const DirectX::XMFLOAT2& position)
   {
      std::unique_ptr<CSprite> pSprite = std::make_unique<CSprite>();
      pSprite->pTextureD3D = pTexture;

      // Obtenir la dimension de la texture;
      ID3D11Resource* pResource;
      ID3D11Texture2D *pTextureInterface = 0;
      pSprite->pTextureD3D->GetResource(&pResource);
      pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
      D3D11_TEXTURE2D_DESC desc;
      pTextureInterface->GetDesc(&desc);

      DXRelacher(pResource);
      DXRelacher(pTextureInterface);

      const float dx = float(desc.Width);
      const float dy = float(desc.Height);

      // Dimension en facteur
      const float facteurX = dx * 2.0f / pDispositif->GetLargeur();
      const float facteurY = dy * 2.0f / pDispositif->GetHauteur();

      // Position en coordonnées logiques
      // 0,0 pixel = -1,1   

      const float posX = static_cast<float>(position.x) * 2.0f / pDispositif->GetLargeur() - 1.0f;
      const float posY = 1.0f - static_cast<float>(position.y) * 2.0f / pDispositif->GetHauteur();

      pSprite->matPosDim = DirectX::XMMatrixScaling(facteurX, facteurY, 1.0f) *
         DirectX::XMMatrixTranslation(posX, posY, 0.0f);

      // On l'ajoute à notre vecteur
      tabSprites.push_back(std::move(pSprite));
   }

   void CAfficheurSprite::MoveSprite(size_t index, int dx, int dy, const DirectX::XMFLOAT2& pos)
   {
       const float posX = static_cast<float>(pos.x) * 2.0f / pDispositif->GetLargeur() - 1.0f;
       const float posY = 1.0f - static_cast<float>(pos.y) * 2.0f / pDispositif->GetHauteur();

       tabSprites[index]->matPosDim = DirectX::XMMatrixScaling(dx * 2.0f / pDispositif->GetLargeur(), dy * 2.0f / pDispositif->GetHauteur(), 1.0f)
           * DirectX::XMMatrixTranslation(posX, posY, 0.0f);
   }

} // namespace PM3D
