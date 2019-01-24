#ifndef LUMIERE_H
#define LUMIERE_H

#include "stdafx.h"
#include "FrustrumView.h"
#include "../Formes/CGameObject.h"


namespace PM3D {

   struct CouleurLumiere {

      CouleurLumiere(const DirectX::XMVECTOR& _ambient, 
         const DirectX::XMVECTOR& _diffus, 
         const DirectX::XMVECTOR& _speculaire) : ambient{ _ambient }, diffus{ _diffus }, speculaire{ _speculaire }{}
      CouleurLumiere() : CouleurLumiere({}, {}, {}) {}
      ~CouleurLumiere() = default;

      DirectX::XMVECTOR ambient;
      DirectX::XMVECTOR diffus;
      DirectX::XMVECTOR speculaire;
   };

   class Lumiere : public CGameObject {
   private:
      

      FrustrumView frustrumView;
      CouleurLumiere couleur;

   public:

      Lumiere() = default;
      ~Lumiere() = default;
      Lumiere(const FrustrumView& _frustrumView) : frustrumView{ _frustrumView } {}
      Lumiere(const Orientation& _orientation, const CouleurLumiere& _couleur = CouleurLumiere(), const FrustrumView& _frustrumView = FrustrumView()) : CGameObject(_orientation), couleur{ _couleur }, frustrumView{ _frustrumView } {}

      DirectX::XMMATRIX getMatProj() const {
         return frustrumView.GetMatProj();
      }

      CouleurLumiere getCouleur() {
         return couleur;
      }

      virtual void Draw(ID3D11ShaderResourceView* pDepthShaderResourceView = nullptr) override {};
      virtual void Update(const DirectX::XMVECTOR& position) { orientation.setPosition(position); };
   };

} // namespace PM3D
#endif //!LUMIERE_H