#include "stdafx.h"
#include "FrustrumView.h"

#include <cmath>

FrustrumView::FrustrumView(const float champDeVision, const float ratioDAspect, const float planRapproche, const float planEloigne, const bool orthographique)
{
   if (orthographique) {
      float nbPortionMax = 3.f;
      float nbPortion = 2.f;
      float planMedian = (nbPortion*planEloigne + (nbPortionMax - nbPortion)*planRapproche) / nbPortionMax;
      float hauteurPlanEloigne = 2 * planMedian * tan(champDeVision / 2.0f);
      float largeurPlanEloigne = ratioDAspect * hauteurPlanEloigne;
      m_MatProj = DirectX::XMMatrixOrthographicLH(largeurPlanEloigne, hauteurPlanEloigne, planRapproche, planEloigne);
   }
   else {
      m_MatProj = DirectX::XMMatrixPerspectiveFovLH(champDeVision, ratioDAspect, planRapproche, planEloigne);
   }
}

void FrustrumView::Update(DirectX::XMMATRIX& matViewProj, const DirectX::XMMATRIX& matView)
{
   matViewProj = matView * m_MatProj;

   //LEFT
   m_planes[0] = DirectX::XMPlaneNormalize({
       matViewProj._14 + matViewProj._11,
       matViewProj._24 + matViewProj._21,
       matViewProj._34 + matViewProj._31,
       matViewProj._44 + matViewProj._41
      });

   //RIGHT
   m_planes[1] = DirectX::XMPlaneNormalize({
       matViewProj._14 - matViewProj._11,
       matViewProj._24 - matViewProj._21,
       matViewProj._34 - matViewProj._31,
       matViewProj._44 - matViewProj._41
      });

   //TOP
   m_planes[2] = DirectX::XMPlaneNormalize({
       matViewProj._14 - matViewProj._12,
       matViewProj._24 - matViewProj._22,
       matViewProj._34 - matViewProj._32,
       matViewProj._44 - matViewProj._42
      });

   //BOT
   m_planes[3] = DirectX::XMPlaneNormalize({
       matViewProj._14 + matViewProj._12,
       matViewProj._24 + matViewProj._22,
       matViewProj._34 + matViewProj._32,
       matViewProj._44 + matViewProj._42
      });

   //NEAR
   m_planes[4] = DirectX::XMPlaneNormalize({
       matViewProj._13,
       matViewProj._23,
       matViewProj._33,
       matViewProj._43
      });

   //FAR
   m_planes[5] = DirectX::XMPlaneNormalize({
       matViewProj._14 - matViewProj._13,
       matViewProj._24 - matViewProj._23,
       matViewProj._34 - matViewProj._33,
       matViewProj._44 - matViewProj._43
      });
}

bool FrustrumView::IsInRectangle(const Zone& zone) const {
   for (auto i = 0; i < NB_MAX_PLANES; ++i)
   {
      DirectX::XMVECTOR normalPlane1 = m_planes[i];
      normalPlane1.vector4_f32[3] = 0.f;

      for (auto corner : zone.corners)
      {
         if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(normalPlane1, corner)) + XMVectorGetW(m_planes[i]) > 0.f)
            return true;
      }
   }

   return false;
}

const DirectX::XMMATRIX& FrustrumView::GetMatProj() const
{
   return m_MatProj;
}