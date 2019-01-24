#include "stdafx.h"
#include "util.h"
#include "Cylindre.h"
#include "Resource.h"
#include "MoteurWindows.h"
#include "toolsMath.h"
#include "ScenePhysique.h"
#include "Orientation.h"

using namespace DirectX;

namespace PM3D
{
   //  FONCTION : Cylindre, constructeur paramètré 
   //  BUT :	Constructeur d'une classe d'un cylindre !
   //  PARAMÈTRES:
   //       positionCentre, le centre du cylindre
   //       rayon, le rayon de l'enveloppe
   //       demiHauteur, la distance du centre à une base
   //       echantillonnageBase, le nombre de points sur un disque
   //		pDispositif: pointeur sur notre objet dispositif
   Cylindre::Cylindre(DirectX::XMFLOAT3& positionCentre, const float rayon, const float demiHauteur, const int echantillonnageBase, CDispositifD3D11* pDispositif_, ScenePhysique& scenePhysique)
       : CObjetMesh(pDispositif_, Orientation{positionCentre}, DirectX::XMVECTOR{ rayon, demiHauteur, rayon })
         , matWorld(DirectX::XMMatrixIdentity())
         , rotation(0.0f)
         , demiHauteur(demiHauteur)
         , rayon(rayon)
   {
       // Les points
       vector<DirectX::XMFLOAT3> point;
       // On commence par on échantillonner circulairement les 2 bases
       float angle = static_cast<float>(2 * M_PI / static_cast<double>(echantillonnageBase));
       // Base haute
       for (int i = 0; i < echantillonnageBase; i++) {
           float theta = i * angle;
           float x = rayon * cos(theta);
           float z = rayon * sin(theta);
           point.emplace_back( x, demiHauteur, z);
       }
       // Base basse
       for (int i = 0; i < echantillonnageBase; i++) {
           float theta = i * angle;
           float x = rayon * cos(theta);
           float z = rayon * sin(theta);
           point.emplace_back(x, -demiHauteur, z);
       }
       // Puis on rajoute le point du centre de la base haute
       point.emplace_back(0.0f, +demiHauteur, 0.0f);
       // Puis on rajoute le point du centre de la base basse
       point.emplace_back(0.0f, -demiHauteur, 0.0f);

       // Les indices
       // On commence par rajouter l'enveloppe
       for (int i = 0; i < echantillonnageBase; i++) {
           // Triangle vers le bas
           m_Indices.emplace_back((i + 1) % echantillonnageBase);
           m_Indices.emplace_back(i + echantillonnageBase);
           m_Indices.emplace_back(i);
           // Triangle vers le haut
           m_Indices.emplace_back((i + 1) % echantillonnageBase + echantillonnageBase);
           m_Indices.emplace_back(i + echantillonnageBase);
           m_Indices.emplace_back((i + 1) % echantillonnageBase);
       }
       // Puis les disque
       for (int i = 0; i < echantillonnageBase; i++) {
           // Disque du haut
           m_Indices.emplace_back((i + 1) % echantillonnageBase);
           m_Indices.emplace_back(i);
           m_Indices.emplace_back(point.size() - 2);
       }
       for(int i = 0; i < echantillonnageBase; i++) {
           // Disque du bas
           m_Indices.emplace_back(i + echantillonnageBase);
           m_Indices.emplace_back((i + 1) % echantillonnageBase + echantillonnageBase);
           m_Indices.emplace_back(point.size() - 1);
       }

       // Les normales
       vector<DirectX::XMFLOAT3> normales;
       // On commence par rajouter l'enveloppe du cone
       DirectX::XMFLOAT3 centreHaut = point[point.size() - 2];
       for (int i = 0; i < echantillonnageBase; i++) {
           normales.emplace_back(point[i].x - centreHaut.x, point[i].y - centreHaut.y, point[i].z - centreHaut.z);
       }
       // On le fait 2 fois car il y a le double de triangles !
       for (int i = 0; i < echantillonnageBase; i++) {
           normales.emplace_back(point[i].x - centreHaut.x, point[i].y - centreHaut.y, point[i].z - centreHaut.z);
       }
       // Puis les disques
       for (int i = 0; i < echantillonnageBase; i++) {
           normales.emplace_back(0.f, 0.f, 1.f); // Vers le haut, ez
       }
       for (int i = 0; i < echantillonnageBase; i++) {
           normales.emplace_back(0.f, 0.f, -1.f); // Vers le bas, ez
       }

       // On peut enfin créer les sommets ! <3
       for (int i = 0; i < point.size(); i++) {
           m_Sommets.emplace_back(point[i], normales[i]);
       }

      for (auto& sommet : m_Sommets) {
          XMStoreFloat3(&sommet.m_Position, XMLoadFloat3(&sommet.m_Position));
      }

      InitBuffer();
      InitEffet();

      // On s'occupe de physx
      shape = scenePhysique.gPhysics->createShape(physx::PxCapsuleGeometry{rayon, demiHauteur}, *(scenePhysique.gMaterial), true);
      physx::PxTransform t{orientation.getPositionX(), orientation.getPositionY(), orientation.getPositionZ()};
      rigidbody = scenePhysique.gPhysics->createRigidDynamic(t);
      rigidbody->setAngularDamping(0.5f);
      rigidbody->attachShape(*shape);
      scenePhysique.gScene->addActor(*rigidbody);
   }

    void Cylindre::InitBuffer() {
        // Création du vertex buffer et copie des sommets // Placer les sommets dans le vertex buffer
        auto pD3DDevice = pDispositif->GetD3DDevice();

        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth = static_cast<UINT>(sizeof(CSommet)*m_Sommets.size());
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory(&InitData, sizeof(InitData));
        InitData.pSysMem = m_Sommets.data();
        pVertexBuffer = nullptr;
        DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);

        // Création de l'index buffer et copie des indices
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth = static_cast<UINT>(sizeof(uint16_t)*m_Indices.size());
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;

        ZeroMemory(&InitData, sizeof(InitData));
        InitData.pSysMem = m_Indices.data();
        pIndexBuffer = nullptr;
        // Placer les index dans l'index buffer  
        DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer), DXE_CREATIONINDEXBUFFER);
    }

   void Cylindre::AnimeSpecific(float tempsEcoule)
   {
      rotation = rotation + ((DirectX::XM_PI * 2.0f) / 3.0f * tempsEcoule);

      // modifier la matrice de l'objet bloc
      matWorld = DirectX::XMMatrixRotationX(rotation);
   }

   void Cylindre::Draw()
   {
       // Obtenir le contexte
       auto pImmediateContext = pDispositif->GetImmediateContext();

       // Choisir la topologie des primitives
       pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

       // Source des sommets
       const UINT stride = sizeof(CSommet);
       const UINT offset = 0;
       pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

       // Source des index
       pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

       // input layout des sommets
       pImmediateContext->IASetInputLayout(pVertexLayout);

       // Initialiser et sélectionner les «constantes» de l'effet
       XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
       sp.matWorldViewProj = XMMatrixTranspose(getMatriceWorld() * viewProj);
       sp.matWorld = XMMatrixTranspose(getMatriceWorld());

       sp.vLumiere = XMVectorSet(4.0f, 30.0f, 4.0f, 1.0f);
       sp.vCamera = CMoteurWindows::GetInstance().GetCamera().getOrientation().getPosition();
       sp.vAEcl = XMVectorSet(0.7f, 0.7f, 0.7f, 1.0f);
       sp.vAMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
       sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
       sp.vDMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
       pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

       auto pCB = pEffet->GetConstantBufferByName("param");  // Nous n'avons qu'un seul CBuffer
       pCB->SetConstantBuffer(pConstantBuffer);

       //// Activation de la texture
       //ID3DX11EffectShaderResourceVariable* variableTexture;
       //variableTexture = pEffet->GetVariableByName("textureEntree")->AsShaderResource();
       //variableTexture->SetResource(pTextureD3D);

       //// Le sampler state
       //ID3DX11EffectSamplerVariable* variableSampler;
       //variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
       //variableSampler->SetSampler(0, pSampleState);

       // **** Rendu de l'objet
       pPasse->Apply(0, pImmediateContext);

       pImmediateContext->DrawIndexed(static_cast<UINT>(m_Indices.size()), 0, 0);
   }

   Cylindre::~Cylindre()
   {
    //  DXRelacher(pPixelShader);
      DXRelacher(pConstantBuffer);
      DXRelacher(pVertexLayout);
    //  DXRelacher(pVertexShader);
      DXRelacher(pIndexBuffer);
      DXRelacher(pVertexBuffer);
      shape->release();
   }

   void Cylindre::InitEffet()
   {
       // Compilation et chargement du vertex shader
       auto pD3DDevice = pDispositif->GetD3DDevice();

       // Création d'un tampon pour les constantes du VS
       D3D11_BUFFER_DESC bd;
       ZeroMemory(&bd, sizeof(bd));
       bd.Usage = D3D11_USAGE_DEFAULT;
       bd.ByteWidth = sizeof(ShadersParams);
       bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
       bd.CPUAccessFlags = 0;
       pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

       // Pour l'effet
       ID3DBlob* pFXBlob = nullptr;
       DXEssayer(D3DCompileFromFile(L"../src/shaders/MiniPhong.fx", 0, 0, 0,
           "fx_5_0", 0, 0,
           &pFXBlob, nullptr),
           DXE_ERREURCREATION_FX);

       D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(), pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet);

       pFXBlob->Release();

       pTechnique = pEffet->GetTechniqueByIndex(0);
       pPasse = pTechnique->GetPassByIndex(0);

       // Créer l'organisation des sommets pour le VS de notre effet
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

       //// Initialisation des paramètres de sampling de la texture
       //D3D11_SAMPLER_DESC samplerDesc;

       //samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
       //samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
       //samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
       //samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
       //samplerDesc.MipLODBias = 0.0f;
       //samplerDesc.MaxAnisotropy = 4;
       //samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
       //samplerDesc.BorderColor[0] = 0;
       //samplerDesc.BorderColor[1] = 0;
       //samplerDesc.BorderColor[2] = 0;
       //samplerDesc.BorderColor[3] = 0;
       //samplerDesc.MinLOD = 0;
       //samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

       //// Création de l'état de sampling
       //pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);
   }

} // namespace PM3D
