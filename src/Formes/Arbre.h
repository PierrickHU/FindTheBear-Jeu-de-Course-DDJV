#ifndef Arbre_H
#define Arbre_H

#include "Item.h"

namespace PM3D
{
	class Arbre : public Item {
	public:
		Arbre(const Orientation& _orientation, const float _dx, const float _dy, const float _dz, CDispositifD3D11* _pDispositif, ScenePhysique& scenePhysique);

		éévirtual void OnTrigger() override; // On décrémente le nombre de parties d'ours
	};

} // namespace PM3D

#endif // !PARTIE_OURS_H
