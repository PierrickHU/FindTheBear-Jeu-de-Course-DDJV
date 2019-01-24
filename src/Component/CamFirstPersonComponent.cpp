#include "stdafx.h"
#include "MoteurWindows.h"
#include "toolsMath.h"

#include <cmath>

namespace PM3D
{
	CamFirstPersonComponent::CamFirstPersonComponent() :
		CamInputComponent{ {}, {}, FPS }
	{
		playerOrientation = &CMoteurWindows::GetInstance().getVehicule()->orientation;
		DirectX::XMVECTOR positionInit{
			 playerOrientation->getPositionX() /*- cos(angle) * (DISTANCE_ANCRAGE_MAX + DISTANCE_ANCRAGE_MIN) / 2*/
		  , playerOrientation->getPositionY() /*+ sin(angle) * (DISTANCE_ANCRAGE_MAX + DISTANCE_ANCRAGE_MIN) / 2*/
		  , playerOrientation->getPositionZ() };

		//DirectX::XMVECTOR directionInit = normaliseVecteur(playerOrientation->getPosition() - positionInit);
		//DirectX::XMVECTOR axe = produitVectoriel(directionInit, normaliseVecteur(produitVectoriel(produitVectoriel(directionInit, getAxeY()), directionInit)));
		orientation = { std::move(positionInit), playerOrientation->getDirection() };
	}

	void CamFirstPersonComponent::Handle(float tempsEcoule)
	{

		//TODO : 
		// A CHAQUE FRAME DONNER LA POSITION DE LOBJET A LA CAMERA 
		// DONNER LA DIRECTION DE LOBJET A LA CAMERA
		auto terrainSize = CMoteurWindows::GetInstance().getTerrain()->getSize();

		playerOrientation = &CMoteurWindows::GetInstance().getVehicule()->orientation;
		auto playerPosition = playerOrientation->getPosition();
		auto playerDirection = playerOrientation->getDirection();
		float distanceHorizontal = distanceBetweenTwoPoints(getProjectionPlanaire(orientation.getPosition(), getAxeY(), getOrigineMonde() + DirectX::XMVECTOR{ 0.f,terrainSize.y,0.f,0.f }), playerPosition);
		float distance = distanceBetweenTwoPoints(orientation.getPosition(), playerPosition);

		//// On se rapproche de l'ancrage s'il est trop loin (ou on s'eloigne s'il est trop pres)
		//if (distance < DISTANCE_ANCRAGE_MIN || distance >= DISTANCE_ANCRAGE_MAX || distanceHorizontal <= DISTANCE_ANCRAGE_SOL_MIN)
		//{
		//	DirectX::XMVECTOR mouvement = playerPosition - oldPlayerPosition;
		//	orientation.translate(mouvement);
		//	distance = distanceBetweenTwoPoints(orientation.getPosition(), playerPosition);
		//}

		//// On remet la camera derriere l'ancrage
		//DirectX::XMVECTOR axeRotation = produitVectoriel(playerDirection, playerOrientation->getUp());
		//DirectX::XMVECTOR vecteurPositionCam = rotateVectorWithQuaternion(-playerDirection, getQuaternionRotation(axeRotation, angle));
		//vecteurPositionCam = normaliseVecteur(vecteurPositionCam) * distance;
		orientation.setPosition(playerPosition);

		// on memorise la nouvelle position de l'ancrage pour la prochaine frame
		oldPlayerPosition = playerOrientation->getPosition();

		//// On reoriente la camera vers l'ancrage
		//DirectX::XMVECTOR vecteurAncrageCam = playerOrientation->getPosition() - orientation.getPosition();
		//DirectX::XMVECTOR newUp = normaliseVecteur(produitVectoriel(produitVectoriel(vecteurAncrageCam, getAxeY()), vecteurAncrageCam));
		orientation.setDirection(playerOrientation->getDirection(), playerOrientation->getUp());
		//	normaliseVecteur(vecteurAncrageCam), newUp);
	}
}
