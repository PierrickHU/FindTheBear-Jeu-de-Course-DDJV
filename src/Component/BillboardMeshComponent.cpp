
#include "stdafx.h"
#include "BillboardMeshComponent.h"
#include "util.h"
#include "Resource.h"
#include "MoteurWindows.h"

namespace PM3D
{

    CSommet BillboardMeshComponent::sommets[6] =
    {
        {XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)},
        {XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)},
        {XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)},
        {XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT2(0.0f, 1.0f)},
        {XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT2(1.0f, 0.0f)},
        {XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)}
    };

    BillboardMeshComponent::BillboardMeshComponent(CDispositifD3D11* _pDispositif, const std::wstring& filename)
        :pDispositif{_pDispositif}
    {
        bottomVector = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&sommets[5].m_Position), DirectX::XMLoadFloat3(&sommets[3].m_Position));
        material = { CMoteurWindows::GetInstance().GetRessourcesManager().GetTexture(filename) };
        InitBuffer();
        InitEffet();
    }

    void BillboardMeshComponent::InitEffet()
    {
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

        pEffet = CMoteurWindows::GetInstance().GetShaderManager().GetEffet(L"../ressources/shaders/MiniPhong.fx");

        // Creer l'organisation des sommets pour le VS de notre effet
        D3DX11_PASS_SHADER_DESC passeVSDesc; // contient la description de la passe
        D3DX11_EFFECT_SHADER_DESC effectVSDesc; // contient la description de l'effet
        const void *vsCodePtr; // pointeur vers le code compile
        unsigned vsCodeLen; // taille du le code compile

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

    void BillboardMeshComponent::InitBuffer()
    {
        auto pD3DDevice = pDispositif->GetD3DDevice();

        // VERTEX BUFFER
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth = static_cast<UINT>(sizeof(CSommet)*6);
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory(&InitData, sizeof(InitData));
        InitData.pSysMem = sommets;
        DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);
    }

    void BillboardMeshComponent::Draw(const DirectX::XMMATRIX& matWorld, ID3D11ShaderResourceView* pDepthShaderResourceView)
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
        sp.matWorldViewProjLight = sp.matWorldViewProj; //RANDOM STUFF
        sp.matWorld = matWorld;

        sp.dirLumiere = -DirectX::XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f);
        sp.vCamera = CMoteurWindows::GetInstance().GetCamera().getOrientation().getPosition();
        sp.vAEcl = DirectX::XMVectorSet(0.7f, 0.7f, 0.7f, 1.0f);
        sp.vAMat = DirectX::XMVectorSet(0.7f, 0.7f, 0.7f, 1.0f);
        sp.vDEcl = DirectX::XMVectorSet(0.7f, 0.7f, 0.7f, 1.0f);
        sp.vDMat = DirectX::XMVectorSet(0.7f, 0.7f, 0.7f, 1.0f);
        sp.vSEcl = DirectX::XMVectorSet(0.7f, 0.7f, 0.7f, 1.0f);
        sp.vSMat = DirectX::XMVectorSet(0.7f, 0.7f, 0.7f, 1.0f);
        sp.bTunnel = 0;

        ID3DX11EffectConstantBuffer* pShaderConstBuffer = pEffet->GetConstantBufferByName("param");
        pShaderConstBuffer = pEffet->GetConstantBufferByName("param");  // Nous n'avons qu'un seul CBuffer
        pShaderConstBuffer->SetConstantBuffer(pConstantBuffer);
        pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

        // Activation de la texture
        ID3DX11EffectShaderResourceVariable* variableTexture;
        variableTexture = pEffet->GetVariableByName("texture0")->AsShaderResource();
        variableTexture->SetResource(material.texture->ressource);

        // Le sampler state
        ID3DX11EffectSamplerVariable* variableSampler;
        variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
        variableSampler->SetSampler(0, pSampleState);

        // **** Rendu de l'objet
        pPasse->Apply(0, pImmediateContext);

        pDispositif->ActiverMelangeAlpha();
        pImmediateContext->Draw(6, 0);
        pDispositif->DesactiverMelangeAlpha();
    }

}
