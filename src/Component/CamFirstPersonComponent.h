#ifndef CAMERA_FIRST_PERSON_COMPONENT_H
#define CAMERA_FIRST_PERSON_COMPONENT_H

#include "CamInputComponent.h"

namespace PM3D
{
	class CamFirstPersonComponent : public CamInputComponent
	{
		static constexpr float VITESSE_ZOOM = 0.3f;

		static constexpr float DISTANCE_ANCRAGE_MAX = 8.f;
		static constexpr float DISTANCE_ANCRAGE_MIN = 3.f;
		static constexpr float DISTANCE_ANCRAGE_SOL_MIN = 1.f;
		static constexpr float PRECISION_ANGULAIRE = DirectX::XM_PIDIV4;

		Orientation* playerOrientation;
		DirectX::XMVECTOR oldPlayerPosition;
		float angle = DirectX::XM_PIDIV4 / 2;
	public:
		CamFirstPersonComponent();
		~CamFirstPersonComponent() = default;

		virtual void Handle(const float tempsEcoule) override;
	};

}
#endif // !CAMERA_H