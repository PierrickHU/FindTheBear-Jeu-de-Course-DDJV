#ifndef PHYSIC_PLAYER_COMPONENT_H
#define PHYSIC_PLAYER_COMPONENT_H

#include "PhysicDynamicComponent.h"
#include "ScenePhysique.h"
#include "Formes/CGameObject.h"
#include <chrono>
#include <cmath>

using std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::milliseconds;

namespace PM3D
{
   class CTerrain;
   struct PhysicPlayerComponent : PhysicDynamicComponent
   {
      enum EtatFrein {
         FREINE,
         RELANCE,
         ATTENTE,
         INACTIF
      };

      const milliseconds TEMPS_INACTION_OUT_OF_TERRAIN{ 500 };

      static constexpr float VITESSE_LINEAIRE_MAX = 60.f;
      static constexpr float TEMPS_ACCES_VITESSE_LINEAIRE_MAX = 1.f;
      static constexpr float ACCELERATION_LINEAIRE_MAX = VITESSE_LINEAIRE_MAX / TEMPS_ACCES_VITESSE_LINEAIRE_MAX;

      static constexpr float HAUTEUR_OSCILLATION = 1.f;
      static constexpr float HAUTEUR_MIN = 20.f;
      static constexpr float HAUTEUR_MAX = HAUTEUR_MIN + HAUTEUR_OSCILLATION;

      static constexpr float CONSTANTE_GRAVITATION = 9.81f;
      static constexpr float VITESSE_PICHENETTE_CARRE = HAUTEUR_OSCILLATION*2.f* CONSTANTE_GRAVITATION*CONSTANTE_GRAVITATION/(2.f* CONSTANTE_GRAVITATION-1.f);
      static const float VITESSE_PICHENETTE() { return std::sqrt(VITESSE_PICHENETTE_CARRE); }

      static constexpr float HAUTEUR_FREINAGE = 2.f;
      static constexpr float TEMPS_FREINAGE = 0.5f;
      static constexpr float TEMPS_RELANCE = 2.f/3.f*TEMPS_FREINAGE;

      float oldVitesseY;
      float accelerationFreinY;
      float accelerationRelanceY;
      float forcePichenette;
      EtatFrein etatFrein = INACTIF;

      PhysicPlayerComponent(CGameObject* obj, ScenePhysique& scenePhysique);
      ~PhysicPlayerComponent() = default;

      void ajouterOscillation(CGameObject* obj, const CTerrain& terrain, const float tempsEcoule);
      void ajouterForceRenvoyerCentreTerrain(CGameObject* obj, const CTerrain& terrain);
      bool aDepasseTerrainRecemment() const noexcept;
   private:
      bool rayCastTouche(const CGameObject* obj, vector<float>& hitDistances) const noexcept;
      time_point<steady_clock> lastTimeOutOfTerrain{};
      float getDescelerationFreinage(const float vitesseInit) const;
      float getAccelerationRelance(const float vitesseVoulue) const;

   };
}
#endif