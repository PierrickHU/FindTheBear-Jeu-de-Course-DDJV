#include "stdafx.h"
#include "util.h"
#include "Sphere.h"
#include "Resource.h"
#include "MoteurWindows.h"
#include "ScenePhysique.h"
#include "Orientation.h"

using namespace DirectX;

namespace PM3D
{
   //  FONCTION : Sphere, constructeur paramètré 
   //  BUT :	Constructeur d'une classe d'une sphère
   //  PARAMÈTRES:
   //       rayon est le rayon ! :D
   //		pDispositif: pointeur sur notre objet dispositif
     Sphere::Sphere(DirectX::XMFLOAT3& _position, const float rayon, const int echantillonnageVertical, const int echantillonnageHorizontal, CDispositifD3D11* pDispositif_, ScenePhysique& scenePhysique)
         : CObjetMesh(pDispositif_, Orientation{ _position }, DirectX::XMVECTOR{ 0.f, 0.f, 0.f })
         , matWorld(DirectX::XMMatrixIdentity())
         , rotation(0.0f)
         , rayon(rayon)
   {
       // Les points
       vector<DirectX::XMFLOAT3> point;
       // On commence par on échantillonner circulairement en descendant
       // Puis on échantillonne sur chaque anneaux
       float angleHorizontal = static_cast<float>(2 * M_PI / static_cast<double>(echantillonnageHorizontal));
       float angleVertical = static_cast<float>(2 * M_PI / static_cast<double>(echantillonnageVertical));
       for (int i = 0; i < echantillonnageHorizontal; i++) {
           for (int j = 0; j < echantillonnageVertical; j++) {
               float phi = i * angleHorizontal;
               float theta = j * angleVertical;
               float x = rayon * sin(phi) * cos(theta);
               float y = rayon * sin(phi) * sin(theta);
               float z = rayon * cos(phi);
               point.emplace_back(x, y, z);
           }
       }
       // Puis par rajouter les 2 points des extrémités
       point.emplace_back(0.f, rayon, 0.f); // Haut
       point.emplace_back(0.f, -rayon, 0.f); // Bas

       // Les indices
       // On commence par rajouter les cônes des extrémités
       int nbPointsNormaux = echantillonnageHorizontal * echantillonnageVertical;
       for (int i = 0; i < echantillonnageHorizontal; i++) {
           // Couronne du haut
           m_Indices.emplace_back(i);
           m_Indices.emplace_back((i + 1) % echantillonnageHorizontal);
           m_Indices.emplace_back(nbPointsNormaux);
           // Couronne du bas // Je sais pas pourquoi mais en fait il n'y en a pas besoin :o
           m_Indices.emplace_back(nbPointsNormaux - ((i + 1) % echantillonnageHorizontal));
           m_Indices.emplace_back(nbPointsNormaux - i);
           m_Indices.emplace_back(nbPointsNormaux + 1);
       }
       // Puis les inter-couronnes
       for (int i = 0; i < echantillonnageVertical - 1; i++) {
           for (int j = 0; j < echantillonnageHorizontal; j++) {
               // Triangle vers le bas
               m_Indices.emplace_back(i * echantillonnageVertical + ((j + 1) % echantillonnageHorizontal));
               m_Indices.emplace_back(i * echantillonnageVertical + j);
               m_Indices.emplace_back((i+1) * echantillonnageVertical + j);
               // Triangle vers le haut
               m_Indices.emplace_back((i+1) * echantillonnageVertical + j);
               m_Indices.emplace_back((i+1) * echantillonnageVertical + ((j + 1) % echantillonnageHorizontal));
               m_Indices.emplace_back(i * echantillonnageVertical + ((j + 1) % echantillonnageHorizontal));
           }
       }

       // Les normales
       // C'est facile ! Toutes les normales sont dans la direction (centre ==> point sur la périphérie) !
       vector<DirectX::XMFLOAT3> normales;
       // On fait toutes les normales d'un coup !
       for (int i = 0; i < point.size(); i++) {
           DirectX::XMFLOAT3 direction{point[i].x, point[i].y, point[i].z};
           float norme = sqrt(pow(direction.x, 2) + pow(direction.y, 2) + pow(direction.z, 2));
           DirectX::XMFLOAT3 normale{ direction.x / norme, direction.y / norme, direction.z / norme };
           normales.push_back(normale);
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

      // On s'occupe de physx !
      shape = scenePhysique.gPhysics->createShape(physx::PxSphereGeometry{rayon}, *(scenePhysique.gMaterial), true);
      physx::PxTransform t{orientation.getPositionX(), orientation.getPositionY(), orientation.getPositionZ()};
      rigidbody = scenePhysique.gPhysics->createRigidDynamic(t);
      rigidbody->setAngularDamping(0.5f);
      rigidbody->attachShape(*shape);
      scenePhysique.gScene->addActor(*rigidbody);
   }

    void Sphere::InitBuffer() {
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

   void Sphere::AnimeSpecific(float tempsEcoule)
   {

   }

   void Sphere::Draw()
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

   Sphere::~Sphere()
   {
    //  DXRelacher(pPixelShader);
      DXRelacher(pConstantBuffer);
      DXRelacher(pVertexLayout);
    //  DXRelacher(pVertexShader);
      DXRelacher(pIndexBuffer);
      DXRelacher(pVertexBuffer);
      shape->release();
   }

   void Sphere::InitEffet()
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
