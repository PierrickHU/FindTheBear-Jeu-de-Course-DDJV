#include "stdafx.h"
#include "Bloc.h"
#include "util.h"
#include "dispositifD3D11.h"
#include "toolsMath.h"
#include "Resource.h"
#include "Moteur.h"
#include "MoteurWindows.h"
#include "ScenePhysique.h"
#include "Orientation.h"
#include "Orientation.h"

using DirectX::XMVectorSet;
using DirectX::XMLoadFloat3;
using DirectX::XMFLOAT3;
using DirectX::XMVECTOR;
using namespace physx;
using namespace std;

namespace PM3D
{
    Bloc::Bloc(const Orientation& orientation, wstring textureFilename, CDispositifD3D11* pDispositif, ScenePhysique& scenePhysique) :
        orientation{ orientation }
    {
        float dx = orientation.getScale().x;
        float dy = orientation.getScale().y;
        float dz = orientation.getScale().z;

        // On construit le mesh
        meshComponent = CMoteurWindows::GetInstance().GetRessourcesManager().GetModelMesh(ModelType::CUBE_TRI);
        //meshComponent->shaderFilename = L"../ressources/shaders/MiniPhongSM.fx";
        meshComponent->shaderFilename = L"../ressources/shaders/TerrainNM.fx";
		 //meshComponent->shaderFilename = L"../ressources/shaders/ChateauNM.fx";
			meshComponent->materials[0] = Material{ CMoteurWindows::GetInstance().GetRessourcesManager().GetTexture(textureFilename) };
			meshComponent->setBTex(1);
        // On rajoute les textures ! x)
			 meshComponent->materials.emplace_back(CMoteurWindows::GetInstance().GetRessourcesManager().GetTexture(L"../ressources/textures/stone.dds"));
          meshComponent->materials.emplace_back(CMoteurWindows::GetInstance().GetRessourcesManager().GetTexture(L"../ressources/textures/grass.dds"));
          meshComponent->materials.emplace_back(CMoteurWindows::GetInstance().GetRessourcesManager().GetTexture(L"../ressources/textures/brick.dds"));
          meshComponent->materials.emplace_back(CMoteurWindows::GetInstance().GetRessourcesManager().GetTexture(L"../ressources/textures/stoneNM.dds"));
          meshComponent->materials.emplace_back(CMoteurWindows::GetInstance().GetRessourcesManager().GetTexture(L"../ressources/textures/grassNM.dds"));
          meshComponent->materials.emplace_back(CMoteurWindows::GetInstance().GetRessourcesManager().GetTexture(L"../ressources/textures/brickNM.dds"));
        meshComponent->m_Sommets[0].m_TexCoord = XMFLOAT2{1.0f, 1.0f};
        meshComponent->m_Sommets[1].m_TexCoord = XMFLOAT2{0.0f, 0.0f};
        meshComponent->m_Sommets[2].m_TexCoord = XMFLOAT2{1.0f, 0.0f};
        meshComponent->m_Sommets[3].m_TexCoord = XMFLOAT2{1.0f, 1.0f};
        meshComponent->m_Sommets[4].m_TexCoord = XMFLOAT2{0.0f, 0.0f};
        meshComponent->m_Sommets[5].m_TexCoord = XMFLOAT2{1.0f, 0.0f};
        meshComponent->m_Sommets[6].m_TexCoord = XMFLOAT2{0.0f, 0.0f};
        meshComponent->m_Sommets[7].m_TexCoord = XMFLOAT2{1.0f, 1.0f};
        meshComponent->m_Sommets[8].m_TexCoord = XMFLOAT2{0.0f, 1.0f};
        meshComponent->m_Sommets[9].m_TexCoord = XMFLOAT2{0.0f, 0.0f};
        meshComponent->m_Sommets[10].m_TexCoord = XMFLOAT2{1.0f, 1.0f};
        meshComponent->m_Sommets[11].m_TexCoord = XMFLOAT2{0.0f, 1.0f};
        meshComponent->m_Sommets[12].m_TexCoord = XMFLOAT2{0.0f, 0.0f};
        meshComponent->m_Sommets[13].m_TexCoord = XMFLOAT2{1.0f, 1.0f};
        meshComponent->m_Sommets[14].m_TexCoord = XMFLOAT2{0.0f, 1.0f};
        meshComponent->m_Sommets[15].m_TexCoord = XMFLOAT2{1.0f, 1.0f};
        meshComponent->m_Sommets[16].m_TexCoord = XMFLOAT2{0.0f, 0.0f};
        meshComponent->m_Sommets[17].m_TexCoord = XMFLOAT2{1.0f, 0.0f};
        meshComponent->m_Sommets[18].m_TexCoord = XMFLOAT2{1.0f, 1.0f};
        meshComponent->m_Sommets[19].m_TexCoord = XMFLOAT2{0.0f, 1.0f};
        meshComponent->m_Sommets[20].m_TexCoord = XMFLOAT2{1.0f, 0.0f};
        meshComponent->m_Sommets[21].m_TexCoord = XMFLOAT2{1.0f, 0.0f};
        meshComponent->m_Sommets[22].m_TexCoord = XMFLOAT2{1.0f, 0.0f};
        meshComponent->m_Sommets[23].m_TexCoord = XMFLOAT2{0.0f, 1.0f};

        // Puis on l'initialise !
        meshComponent->Init(pDispositif);

        // On s'occupe de physx
        physicComponent = {};
        PxMaterial* blocMaterial = scenePhysique.gPhysics->createMaterial(scenePhysique.gMaterial->getStaticFriction(), scenePhysique.gMaterial->getDynamicFriction(), scenePhysique.gMaterial->getRestitution());
        blocMaterial->setRestitution(0.2f); // a remettre à 0.0 !
        physicComponent.shape = scenePhysique.gPhysics->createShape(physx::PxBoxGeometry{ dx, dy, dz }, *blocMaterial, true);
        physx::PxTransform t{ orientation.getPositionX(), orientation.getPositionY(), orientation.getPositionZ() };
        physicComponent.rigidbody = scenePhysique.gPhysics->createRigidDynamic(t);
        physicComponent.rigidbody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
        physicComponent.rigidbody->setAngularDamping(0.5f);
        physicComponent.rigidbody->attachShape(*physicComponent.shape);
        physicComponent.rigidbody->setName("Bloc");
        physx::PxRigidBodyExt::updateMassAndInertia(*physicComponent.rigidbody, 10000.f);
        scenePhysique.gScene->addActor(*physicComponent.rigidbody);

        // On applique la rotation s'il y en a déjà une !
        t = physicComponent.rigidbody->getGlobalPose();
        t.q = toPxQuat(orientation.getQuaternion());
        physicComponent.rigidbody->setGlobalPose(t);

        // Un bloc est un mur, en tout cas pour le moment
        PxFilterData filterData;
        filterData.word0 = ScenePhysique::Filter_Group::MUR;
        physicComponent.shape->setQueryFilterData(filterData);
        physicComponent.shape->setSimulationFilterData(filterData);


        //// Le materiel
        //vector<Material> materials{};
        //Material materiel{ CMoteurWindows::GetInstance().GetRessourcesManager().GetTexture(L"../ressources/textures/stone.dds") };
        //materials.push_back(materiel);

        //// Les points
        //XMFLOAT3 point[8] =
        //{
        //   XMFLOAT3(-dx / 2, dy / 2, -dz / 2),
        //   XMFLOAT3(dx / 2, dy / 2, -dz / 2),
        //   XMFLOAT3(dx / 2, -dy / 2, -dz / 2),
        //   XMFLOAT3(-dx / 2, -dy / 2, -dz / 2),
        //   XMFLOAT3(-dx / 2, dy / 2, dz / 2),
        //   XMFLOAT3(-dx / 2, -dy / 2, dz / 2),
        //   XMFLOAT3(dx / 2, -dy / 2, dz / 2),
        //   XMFLOAT3(dx / 2, dy / 2, dz / 2)
        //};

        //vector<int> m_Indices = {
        //   0,1,2,			// devant
        //   0,2,3,			// devant
        //   5,6,7,			// arriere
        //   5,7,4,			// arriere
        //   8,9,10,        // dessous
        //   8,10,11,		// dessous
        //   13,14,15,		// dessus
        //   13,15,12,		// dessus
        //   19,16,17,		// gauche
        //   19,17,18,		// gauche
        //   20,21,22,		// droite
        //   20,22,23		// droite
        //};

        //// Calculer les normales
        //const XMFLOAT3 n0(0.0f, 0.0f, -1.0f); // devant
        //const XMFLOAT3 n1(0.0f, 0.0f, 1.0f); // arriere
        //const XMFLOAT3 n2(0.0f, -1.0f, 0.0f); // dessous
        //const XMFLOAT3 n3(0.0f, 1.0f, 0.0f); // dessus
        //const XMFLOAT3 n4(-1.0f, 0.0f, 0.0f); // face gauche
        //const XMFLOAT3 n5(1.0f, 0.0f, 0.0f); // face droite

        //vector<CSommet> m_Sommets =
        //{
        //    // Le devant du bloc
        //    CSommet(point[0], n0),
        //    CSommet(point[1], n0),
        //    CSommet(point[2], n0),
        //    CSommet(point[3], n0),

        //    // L'arriere du bloc
        //    CSommet(point[4], n1),
        //    CSommet(point[5], n1),
        //    CSommet(point[6], n1),
        //    CSommet(point[7], n1),

        //    // Le dessous du bloc
        //    CSommet(point[3], n2),
        //    CSommet(point[2], n2),
        //    CSommet(point[6], n2),
        //    CSommet(point[5], n2),

        //    // Le dessus du bloc
        //    CSommet(point[0], n3),
        //    CSommet(point[4], n3),
        //    CSommet(point[7], n3),
        //    CSommet(point[1], n3),

        //    // La face gauche
        //    CSommet(point[0], n4),
        //    CSommet(point[3], n4),
        //    CSommet(point[5], n4),
        //    CSommet(point[4], n4),

        //    // La face droite
        //    CSommet(point[1], n5),
        //    CSommet(point[7], n5),
        //    CSommet(point[6], n5),
        //    CSommet(point[2], n5)
        //};

        ////for (auto& sommet : m_Sommets) {
        ////    XMStoreFloat3(&sommet.m_Position, XMLoadFloat3(&sommet.m_Position));
        ////}
    }

    void Bloc::Draw() {
        meshComponent->Draw(orientation.calculMatriceWorld());
    }

    //void CBloc::InitBuffer() {
    //   // Creation du vertex buffer et copie des sommets // Placer les sommets dans le vertex buffer
    //   auto pD3DDevice = pDispositif->GetD3DDevice();

    //   D3D11_BUFFER_DESC bd;
    //   ZeroMemory(&bd, sizeof(bd));
    //   bd.Usage = D3D11_USAGE_IMMUTABLE;
    //   bd.ByteWidth = static_cast<UINT>(sizeof(CSommet)*m_Sommets.size());
    //   bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    //   bd.CPUAccessFlags = 0;

    //   D3D11_SUBRESOURCE_DATA InitData;
    //   ZeroMemory(&InitData, sizeof(InitData));
    //   InitData.pSysMem = m_Sommets.data();
    //   pVertexBuffer = nullptr;
    //   DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);

    //   // Creation de l'index buffer et copie des indices
    //   ZeroMemory(&bd, sizeof(bd));
    //   bd.Usage = D3D11_USAGE_IMMUTABLE;
    //   bd.ByteWidth = static_cast<UINT>(sizeof(int)*m_Indices.size());
    //   bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    //   bd.CPUAccessFlags = 0;

    //   ZeroMemory(&InitData, sizeof(InitData));
    //   InitData.pSysMem = m_Indices.data();
    //   pIndexBuffer = nullptr;
    //   // Placer les index dans l'index buffer  
    //   DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer), DXE_CREATIONINDEXBUFFER);
    //}

    //void CBloc::AnimeSpecific(float tempsEcoule) {
    //}


    //void CBloc::Draw()
    //{
    //   // Obtenir le contexte
    //   auto pImmediateContext = pDispositif->GetImmediateContext();

    //   // Choisir la topologie des primitives
    //   pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //   // Source des sommets
    //   const UINT stride = sizeof(CSommet);
    //   const UINT offset = 0;
    //   pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

    //   // Source des index
    //   pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    //   // input layout des sommets
    //   pImmediateContext->IASetInputLayout(pVertexLayout);

    //   // Initialiser et selectionner les constantes de l'effet
    //   DirectX::XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj();

    //   sp.matWorldViewProj = getMatriceTransposition(getMatriceWorld()*viewProj);
    //   sp.matWorld = getMatriceTransposition(getMatriceWorld());

    //   sp.vLumiere = XMVectorSet(4.0f, 30.0f, 4.0f, 1.0f);
    //   sp.vCamera = CMoteurWindows::GetInstance().GetCamera().getOrientation().getPosition();
    //   sp.vAEcl = XMVectorSet(0.7f, 0.7f, 0.7f, 1.0f);
    //   sp.vAMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
    //   sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
    //   sp.vDMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
    //   pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

    //   auto pCB = pEffet->GetConstantBufferByName("param");  // Nous n'avons qu'un seul CBuffer
    //   pCB->SetConstantBuffer(pConstantBuffer);

    //   //// Activation de la texture
    //   //ID3DX11EffectShaderResourceVariable* variableTexture;
    //   //variableTexture = pEffet->GetVariableByName("textureEntree")->AsShaderResource();
    //   //variableTexture->SetResource(pTextureD3D);

    //   //// Le sampler state
    //   //ID3DX11EffectSamplerVariable* variableSampler;
    //   //variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
    //   //variableSampler->SetSampler(0, pSampleState);

    //   // **** Rendu de l'objet
    //   pPasse->Apply(0, pImmediateContext);

    //   pImmediateContext->DrawIndexed(static_cast<UINT>(m_Indices.size()), 0, 0);
    //}

    //void CBloc::InitEffet()
    //{
    //   // Compilation et chargement du vertex shader
    //   auto pD3DDevice = pDispositif->GetD3DDevice();

    //   // Creation d'un tampon pour les constantes du VS
    //   D3D11_BUFFER_DESC bd;
    //   ZeroMemory(&bd, sizeof(bd));
    //   bd.Usage = D3D11_USAGE_DEFAULT;
    //   bd.ByteWidth = sizeof(ShadersParams);
    //   bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //   bd.CPUAccessFlags = 0;
    //   pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

    //   // Pour l'effet
    //   ID3DBlob* pFXBlob = nullptr;
    //   DXEssayer(D3DCompileFromFile(L"../src/shaders/MiniPhong.fx", 0, 0, 0,
    //      "fx_5_0", 0, 0,
    //      &pFXBlob, nullptr),
    //      DXE_ERREURCREATION_FX);

    //   D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(), pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet);

    //   pFXBlob->Release();

    //   pTechnique = pEffet->GetTechniqueByIndex(0);
    //   pPasse = pTechnique->GetPassByIndex(0);

    //   // Creer l'organisation des sommets pour le VS de notre effet
    //   D3DX11_PASS_SHADER_DESC effectVSDesc;
    //   pPasse->GetVertexShaderDesc(&effectVSDesc);

    //   D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
    //   effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex, &effectVSDesc2);

    //   const void *vsCodePtr = effectVSDesc2.pBytecode;
    //   const unsigned vsCodeLen = effectVSDesc2.BytecodeLength;

    //   pVertexLayout = nullptr;
    //   DXEssayer(pD3DDevice->CreateInputLayout(CSommet::layout,
    //      CSommet::numElements,
    //      vsCodePtr,
    //      vsCodeLen,
    //      &pVertexLayout),
    //      DXE_CREATIONLAYOUT);

    //   //// Initialisation des parametres de sampling de la texture
    //   //D3D11_SAMPLER_DESC samplerDesc;

    //   //samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    //   //samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    //   //samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    //   //samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    //   //samplerDesc.MipLODBias = 0.0f;
    //   //samplerDesc.MaxAnisotropy = 4;
    //   //samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    //   //samplerDesc.BorderColor[0] = 0;
    //   //samplerDesc.BorderColor[1] = 0;
    //   //samplerDesc.BorderColor[2] = 0;
    //   //samplerDesc.BorderColor[3] = 0;
    //   //samplerDesc.MinLOD = 0;
    //   //samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    //   //// Creation de l'�tat de sampling
    //   //pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);
    //}

    MeshComponent& Bloc::getMesh() {
        return *meshComponent;
    }
    Orientation& Bloc::getOrientation() {
        return orientation;
    }


} // namespace PM3D
