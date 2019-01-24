#ifndef BLOC_H
#define BLOC_H

#include "Formes/Orientation.h"
#include "Component/MeshComponent.h"
#include "Component/PhysicComponent.h"
#include <string>

namespace PM3D
{
    class CDispositifD3D11;
    class ScenePhysique;
    class Bloc {
        MeshComponent* meshComponent;
        PhysicComponent physicComponent;
        Orientation orientation;
    public:
        Bloc(const Orientation& _orientation, std::wstring textureFilename, CDispositifD3D11* _pDispositif, ScenePhysique& scenePhysique); // Orientation s'occupe de la position, la rotation et le scale !

        void Draw();

        MeshComponent& getMesh();
        Orientation& getOrientation();
    };

} // namespace PM3D

#endif // !BLOC_H
