#ifndef BOOST_H
#define BOOST_H

#include "Item.h"

namespace PM3D
{
	class Boost : public Item {
    public:
		Boost(const Orientation& _orientation, const float _dx, const float _dy, const float _dz, CDispositifD3D11* _pDispositif, ScenePhysique& scenePhysique);

        virtual void OnTrigger() override; // On accélère la vitesse du joueur
	};

} // namespace PM3D

#endif // !Boost
