#ifndef PARTIE_OURS_H
#define PARTIE_OURS_H

#include "Item.h"

namespace PM3D
{
	class PartieOurs : public Item {
    public:
		PartieOurs(const Orientation& _orientation, const float _dx, const float _dy, const float _dz, CDispositifD3D11* _pDispositif, ScenePhysique& scenePhysique);

        virtual void OnTrigger() override; // On décrémente le nombre de parties d'ours

        virtual bool isCatchable() const noexcept override; // On ne peut attraper un nounours que dans le monde du reve !
	};

} // namespace PM3D

#endif // !PARTIE_OURS_H
