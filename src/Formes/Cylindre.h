#ifndef CYLINDRE_H
#define CYLINDRE_H

#include "CObjetMesh.h"
#include "dispositifD3D11.h"
#include "ScenePhysique.h"

namespace PM3D
{
// Permet de créer des Sphéres ! <3
class Cylindre : public CObjetMesh {
public:
    Cylindre(DirectX::XMFLOAT3& positionCentre, const float rayon, const float demiHauteur, const int echantillonnageBase, CDispositifD3D11* pDispositif_, ScenePhysique& scenePhysique);
	virtual ~Cylindre();

	virtual void AnimeSpecific(float tempsEcoule) override;
	virtual void Draw() override;

private:
    virtual void InitBuffer() override;
    virtual void InitEffet() override;

    DirectX::XMMATRIX matWorld;
	float rotation;

    float demiHauteur;
    float rayon;

    physx::PxShape* shape;
};

} // namespace PM3D

#endif
