#include "stdafx.h"
#include "InputPlayerComponent.h"
#include "Vehicule.h"
#include "MoteurWindows.h"

namespace PM3D
{
    void InputPlayerComponent::Handle(const float tempsEcoule, CGameObject* obj) {
        float vitesseLineaireMax = VITESSE_LINEAIRE_MAX * coefVitesse;
        float tempsAccesVitesseLineaireMax = TEMPS_ACCES_VITESSE_LINEAIRE_MAX * coefVitesse;
        float accelerationLineaireMax = vitesseLineaireMax / tempsAccesVitesseLineaireMax;
        float decelerationLineaireMax = 4 * accelerationLineaireMax;

        auto vehicule = dynamic_cast<Vehicule*>(obj);
        auto& GestionnaireDeSaisie = CMoteurWindows::GetInstance().GetGestionnaireDeSaisie();

        //rotation
        auto virageInput = (GestionnaireDeSaisie.ToucheAppuyee(DIK_D) || GestionnaireDeSaisie.ToucheAppuyee(DIK_RIGHT)) - (GestionnaireDeSaisie.ToucheAppuyee(DIK_A) || GestionnaireDeSaisie.ToucheAppuyee(DIK_LEFT));
        auto accelerationInput = (GestionnaireDeSaisie.ToucheAppuyee(DIK_W) || GestionnaireDeSaisie.ToucheAppuyee(DIK_UP)) - (GestionnaireDeSaisie.ToucheAppuyee(DIK_S) || GestionnaireDeSaisie.ToucheAppuyee(DIK_DOWN));

        float oldVitesse = currentVitesse;
        bool changementSens = false;

        if (accelerationInput != 0) {
            // on detecte le changement de sens
            etatVehicule = (accelerationInput < 0) ? RECULE : etatVehicule;
            etatVehicule = (accelerationInput > 0) ? AVANCE : etatVehicule;

            changementSens = oldEtat != etatVehicule
                && (oldEtat == RECULE || oldEtat == AVANCE)
                && (etatVehicule == RECULE || etatVehicule == AVANCE);
        }
        // on freine
        etatVehicule = (accelerationInput == 0 || changementSens) ? FREINE : etatVehicule;

        // Norme de la vitesse
        float acceleration = (etatVehicule == FREINE || etatVehicule == ARRET) ? -decelerationLineaireMax : accelerationLineaireMax;

        // mouvement
        float nouvelleVitesse = oldVitesse + acceleration * tempsEcoule;

        // On borne la vitesse
        currentVitesse = (nouvelleVitesse <= vitesseLineaireMax) ? nouvelleVitesse : vitesseLineaireMax;
        currentVitesse = (nouvelleVitesse >= 0.f) ? currentVitesse : 0.f;

        //on passe a l'arret
        etatVehicule = (currentVitesse == 0.f) ? ARRET : etatVehicule;

        // On determine le sens du mouvement
        float sensMouvement = (etatVehicule == RECULE || (etatVehicule == FREINE && oldEtat == RECULE)) ? -1.f : 1.f;

        // rotation
        if (virageInput != 0) {
            float rotation;
            if (etatVehicule == RECULE || (etatVehicule == FREINE && oldEtat == RECULE)) {
                rotation = sensMouvement * virageInput * VITESSE_ANGULAIRE_MAX * -1;
            } else {
                rotation = sensMouvement * virageInput * VITESSE_ANGULAIRE_MAX * 1;
            }
#undef min
            vehicule->physic->vitesseAngulaire = getQuaternionRotation(vehicule->orientation.getUp(), rotation) * std::min(coefVitesse, 2.0f);
        }
        else {
            vehicule->physic->vitesseAngulaire = getQuaternionRotation(vehicule->orientation.getUp(), 0);
        }

        vehicule->physic->vitesseLineaire = vehicule->orientation.getDirection() * sensMouvement * currentVitesse;
        oldEtat = (etatVehicule != FREINE) ? etatVehicule : oldEtat;
    }

    void InputPlayerComponent::setVitesseMaxCoef(float newCoef) {
        coefVitesse = newCoef;
    }
    float InputPlayerComponent::getVitesseMaxCoef() const {
        return coefVitesse;
    }
    
    float InputPlayerComponent::getVitesseLineaireMax() const {
       return VITESSE_LINEAIRE_MAX;
    }

    float InputPlayerComponent::getCoefVitesse() const {
        return coefVitesse;
    }
}