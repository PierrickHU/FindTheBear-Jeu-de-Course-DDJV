#include "stdafx.h"
#include "util.h"
#include "Cone.h"
#include "Resource.h"
#include "MoteurWindows.h"
#include "toolsMath.h"
#include "ScenePhysique.h"

using namespace DirectX;

namespace PM3D
{
   //  FONCTION : Cone, constructeur paramètré 
   //  BUT :	Constructeur d'une classe d'un cone !
   //  PARAMÈTRES:
   //       positionCentreBase, le centre de l'objet est défini comme le centre du cercle servant de base au cone
   //       rayonBase, le rayon de la base
   //       hauteur, la hauteur du cone
   //       echantillonnageBase, la précision du cercle de la base
   //		pDispositif: pointeur sur notre objet dispositif
     Cone::Cone(const Orientation& orientation, const float _rayonBase, const float _hauteur, const int _echantillonnageBase, CDispositifD3D11* pDispositif_, ScenePhysique& scenePhysique)
         : CObjetMesh(pDispositif_, orientation, DirectX::XMVECTOR{_rayonBase, _hauteur / 2.f, _rayonBase})
         , matWorld(DirectX::XMMatrixIdentity())
         , rotation(0.0f)
         , hauteur(_hauteur)
         , rayonBase(_rayonBase)
   {
       // Les points
       vector<DirectX::XMFLOAT3> point;
       // On commence par on échantillonner circulairement la base
       float angle = static_cast<float>(2 * M_PI / static_cast<double>(_echantillonnageBase));
       for (int i = 0; i < _echantillonnageBase; i++) {
           float theta = i * angle;
           float x = _rayonBase * cos(theta);
           float z = _rayonBase * sin(theta);
           point.emplace_back(x, 0.0f, z);
       }
       // Puis on rajoute le dernier point, celui de l'extrémité !
       point.emplace_back(0.0f, _hauteur, 0.0f);
       // Et on rajoute le point au centre, car ce sera super super pratique !
       point.emplace_back(0.0f, 0.0f, 0.0f);

       // Les indices
       // On commence par rajouter l'enveloppe du cone
       int nbPointsNormaux = _echantillonnageBase;
       // On commence par rajouter l'enveloppe du cone
       for (int i = 0; i < _echantillonnageBase; i++) {
           m_Indices.emplace_back(i);
           m_Indices.emplace_back((i + 1) % _echantillonnageBase);
           m_Indices.emplace_back(_echantillonnageBase);
       }
       // Puis le disque
       for (int i = 0; i < _echantillonnageBase; i++) {
           m_Indices.emplace_back((i + 1) % _echantillonnageBase);
           m_Indices.emplace_back(i);
           m_Indices.emplace_back(_echantillonnageBase + 1);
       }

       // Les normales
       vector<DirectX::XMFLOAT3> normales;
       // On commence par rajouter l'enveloppe du cone
       for (int i = 0; i < _echantillonnageBase; i++) {
           // Faites un dessein ! Et utilisez pythagore ! =)
           float theta = (pow(_hauteur, 2) + pow(_rayonBase, 2)) / _rayonBase;
           DirectX::XMVECTOR apexExtremite{ point[_echantillonnageBase].x - point[i].x, point[_echantillonnageBase].y - point[i].y, point[_echantillonnageBase].z - point[i].z };
           DirectX::XMVECTOR centreExtremite{ point[_echantillonnageBase + 1].x - point[i].x, point[_echantillonnageBase + 1].y - point[i].y, point[_echantillonnageBase + 1].z - point[i].z };
           DirectX::XMVECTOR axe = XMVector3Cross(apexExtremite, centreExtremite);
           DirectX::XMVECTOR quaternion = getQuaternionRotation(axe, theta + DirectX::XM_PI / 2.f);
           DirectX::XMVECTOR direction = rotateVectorWithQuaternion(centreExtremite, quaternion, DirectX::XMVECTOR{point[i].x, point[i].y, point[i].z});
           float norme = sqrt(pow(direction.vector4_f32[0], 2) + pow(direction.vector4_f32[1], 2) + pow(direction.vector4_f32[2], 2));
           DirectX::XMFLOAT3 normale{ direction.vector4_f32[0] / norme, direction.vector4_f32[1] / norme, direction.vector4_f32[2] / norme };
           normales.push_back(normale);
           // Ok j'avoue ... c'est pas super clair, sry !
       }
       // Puis le disque
       for (int i = 0; i < _echantillonnageBase; i++) {
           normales.push_back(DirectX::XMFLOAT3{ 0.f, 0.f, -1.f }); // Vers le bas, ez
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
      // C'est compliqué car il n'y a pas PxConeGeometry !
      //shape = scenePhysique.gPhysics->createShape(??, *(scenePhysique.gMaterial), true);
      //PxTransform t{position.x, position.y, position.z};
      //PxRigidDynamic* body = scenePhysique.gPhysics->createRigidDynamic(t);
      //body->setAngularDamping(0.5f);
      //body->attachShape(*shape);
      //scenePhysique.gScene->addActor(*body);
   }

    void Cone::InitBuffer() {
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

   void Cone::AnimeSpecific(float tempsEcoule)
   {
      rotation = rotation + ((DirectX::XM_PI * 2.0f) / 3.0f * tempsEcoule);

      // modifier la matrice de l'objet bloc
      matWorld = DirectX::XMMatrixRotationX(rotation);
   }

   void Cone::Draw()
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

   Cone::~Cone()
   {
    //  DXRelacher(pPixelShader);
      DXRelacher(pConstantBuffer);
      DXRelacher(pVertexLayout);
    //  DXRelacher(pVertexShader);
      DXRelacher(pIndexBuffer);
      DXRelacher(pVertexBuffer);
      shape->release();
   }

   void Cone::InitEffet()
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
