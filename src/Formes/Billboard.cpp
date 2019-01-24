
#include "stdafx.h"
#include "Billboard.h"
#include "../MoteurWindows.h"
#include "../toolsMath.h"
#include <cmath>

namespace PM3D
{
    Billboard::Billboard(CDispositifD3D11* _pDispositif, const Orientation& _orientation, const DirectX::XMVECTOR& offset, const std::wstring& filename) :
        CGameObject{ _orientation }, offset{ offset }
    {
        orientation.setPosition(DirectX::XMVectorAdd(orientation.getPosition(), offset));
        orientation.setScale({ 1.f, 1.f, 1.f });
        mesh = std::make_unique<BillboardMeshComponent>(_pDispositif, filename);
    }

    void Billboard::Draw(ID3D11ShaderResourceView* pDepthShaderResourceView)
    {
        mesh->Draw(matWorld, pDepthShaderResourceView);
    }

    void Billboard::Update(const float elapsedTime)
    {
        orientation.setPosition(DirectX::XMVectorAdd(orientation.getPosition(), offset));
        orientation.setScale({ orientation.getScale().x * .99f, orientation.getScale().y * .99f, orientation.getScale().z * .99f });

        if (orientation.getScale().x < 0.2f)
            orientation.setScale({ 1.f, 1.f, 1.f });

        auto camPosition = CMoteurWindows::GetInstance().GetCamera().getOrientation().getPosition();
        auto middleBottomVector = test + DirectX::XMVectorMultiply(mesh->bottomVector * orientation.getScale().x, { .5f, 1.f, .5f });
        auto a = atan2(DirectX::XMVectorGetX(middleBottomVector) - DirectX::XMVectorGetX(camPosition), DirectX::XMVectorGetZ(middleBottomVector) - DirectX::XMVectorGetZ(camPosition));

        matWorld = DirectX::XMMatrixScalingFromVector(convertIntoVector(orientation.getScale())) * DirectX::XMMatrixRotationY(a - neededAngle) * DirectX::XMMatrixTranslation(DirectX::XMVectorGetX(test), DirectX::XMVectorGetY(test), DirectX::XMVectorGetZ(test));
    }
}
