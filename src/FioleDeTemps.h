#ifndef FIOLE_DE_TEMPS_H
#define FIOLE_DE_TEMPS_H

#include "Item.h"

namespace PM3D
{
	class FioleDeTemps : public Item {
    public:
		FioleDeTemps(const Orientation& _orientation, const float _dx, const float _dy, const float _dz, CDispositifD3D11* _pDispositif, ScenePhysique& scenePhysique);

        virtual void OnTrigger() override; // On accélère la vitesse du joueur

        virtual bool isCatchable() const noexcept override; // On ne peut attraper une fiole de temps que dans le monde du cauchemar
	};

} // namespace PM3D

#endif // !FIOLE_DE_TEMPS_H
