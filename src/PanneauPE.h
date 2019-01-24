#ifndef  PANNEAU_POST_EFFECTS
#define PANNEAU_POST_EFFECTS

#include "Formes/CGameObject.h"

namespace PM3D
{
   class CDispositifD3D11;

   class CSommetPanneauPE
   {
   public:
      CSommetPanneauPE() = default;
      CSommetPanneauPE(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& coordTex)
         : m_position(position)
         , m_coordTex(coordTex)
      { }

   public:
      static UINT numElements;
      static D3D11_INPUT_ELEMENT_DESC layout[];

      DirectX::XMFLOAT3 m_position;
      DirectX::XMFLOAT2 m_coordTex;
   };

   //   Classe : CPanneauPE
   //   BUT : 	Classe pour les post-effects
   class CPanneauPE : public CGameObject
   {
   public:
      explicit CPanneauPE(CDispositifD3D11* pDispositif_in);
      virtual ~CPanneauPE();

      virtual void Draw(ID3D11ShaderResourceView* pDepthShaderResourceView = nullptr) override;

      void DebutPostEffect();
      void FinPostEffect();
      void setVitesseJoueur(float);
      void setVitesseMaxJoueur(float);

   private:

      void InitEffet();

      static CSommetPanneauPE sommets[6];
      ID3D11Buffer* pVertexBuffer;
      CDispositifD3D11* pDispositif;

      // Pour les effets
      ID3DX11Effect* pEffet;
      ID3DX11EffectTechnique* pTechnique;
      ID3DX11EffectPass* pPasse;

      static const int NOMBRE_TECHNIQUES = 3;
      ID3D11InputLayout* pVertexLayout[NOMBRE_TECHNIQUES];

      ID3D11SamplerState* pSampleState;

      // Texture de rendu pour effets
      ID3D11Texture2D* pTextureScene;
      ID3D11RenderTargetView* pRenderTargetView;
      ID3D11ShaderResourceView* pResourceView;
      ID3D11Texture2D* pDepthTexture;
      ID3D11DepthStencilView* pDepthStencilView;

      ID3D11RenderTargetView* pOldRenderTargetView;
      ID3D11DepthStencilView* pOldDepthStencilView;

      float vitesseJoueur;
      float vitesseMaxJoueur;

      float pourcentageVitesse() {
         return vitesseJoueur / vitesseMaxJoueur;
      }

      using CObjet3DPtr = std::unique_ptr<CGameObject>;
   };

} // namespace PM3D

#endif // ! PANNEAU_POST_EFFECTS

