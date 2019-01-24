#ifndef CHATEAU_H
#define CHATEAU_H

#include "Formes/Bloc.h"
#include "dispositifD3D11.h"
#include <vector>

using std::vector;

namespace PM3D
{
    class ScenePhysique;
	class Chateau {
        vector<Bloc> murs;
        vector<Bloc> toitsTunnel;
        float largeur = 25.f;
        float hauteur = 125.f;
    public:
        Chateau(CDispositifD3D11* pDispositif, ScenePhysique& scenePhysique);

        void Draw();
        void DrawShadow(const XMMATRIX matViewProjLight);

        bool isInTunnel(float x, float y) const;
        bool isInChateau(float x, float z, float dx, float dz) const;
	};

} // namespace PM3D

#endif // !Chateau
