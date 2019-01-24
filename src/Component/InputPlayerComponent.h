#ifndef PLAYER_INPUT_COMPONENT_H
#define PLAYER_INPUT_COMPONENT_H

#include "InputComponent.h"

namespace PM3D
{
    class InputPlayerComponent : public InputPhysicComponent
    {
    public:
        enum ETAT_VEHICULE { ARRET, AVANCE, FREINE, RECULE };

        float currentVitesse = 0.f;
        ETAT_VEHICULE etatVehicule = ARRET;
    
    private:
        static constexpr float VITESSE_LINEAIRE_MAX = 100.f;
        static constexpr float TEMPS_ACCES_VITESSE_LINEAIRE_MAX = 1.f;
        static constexpr float ACCELERATION_LINEAIRE_MAX = VITESSE_LINEAIRE_MAX / TEMPS_ACCES_VITESSE_LINEAIRE_MAX;
        static constexpr float DECELERATION_LINEAIRE_MAX = 2 * ACCELERATION_LINEAIRE_MAX;

        static constexpr float VITESSE_ANGULAIRE_MAX = DirectX::XM_PI;
        static constexpr float TEMPS_ACCES_VITESSE_ANGULAIRE_MAX = 0.1f;
        static constexpr float ACCELERATION_ANGULAIRE_MAX = VITESSE_ANGULAIRE_MAX / TEMPS_ACCES_VITESSE_ANGULAIRE_MAX;
        static constexpr float DECELERATION_ANGULAIRE_MAX = 4 * ACCELERATION_ANGULAIRE_MAX;

        ETAT_VEHICULE oldEtat = etatVehicule;
        float coefVitesse = 1.f; // La valeur par laquelle on multiplie la vitesse linéaire maximum !
    public:
        virtual void Handle(const float tempsEcoule, CGameObject* obj) override;
        void setVitesseMaxCoef(float newCoef);
        float getVitesseMaxCoef() const;
        float getVitesseLineaireMax() const;
        float getCoefVitesse() const;

        InputPlayerComponent() = default;
        ~InputPlayerComponent() = default;
    };
}
#endif