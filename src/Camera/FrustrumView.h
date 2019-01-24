#ifndef FRUSTRUM_VIEW_H
#define FRUSTRUM_VIEW_H

#include "stdafx.h"
#include "Zone.h"

class FrustrumView {
private:
    static constexpr int NB_MAX_PLANES = 6;
    DirectX::XMMATRIX m_MatProj;
    DirectX::XMVECTOR m_planes[NB_MAX_PLANES];

public:
    FrustrumView() = default;
    FrustrumView(const float champDeVision, const float ratioDAspect, const float planRapproche, const float planEloigne, const bool orthographique = false);

    void Update(DirectX::XMMATRIX& matViewProj, const DirectX::XMMATRIX& matView);
    bool IsInRectangle(const Zone& zone) const;

    const DirectX::XMMATRIX& GetMatProj() const;
};

#endif