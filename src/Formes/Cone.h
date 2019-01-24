#ifndef CONE_H
#define CONE_H

#include "dispositifD3D11.h"
#include "ScenePhysique.h"
#include "Orientation.h"

namespace PM3D
{
class CDispositifD3D11;

// Permet de créer des Sphéres ! <3
class Cone : public CObjetMesh {
public:
    Cone(const Orientation& orientation, const float rayonBase, const float hauteur, const int echantillonnageBase, CDispositifD3D11* pDispositif_, ScenePhysique& scenePhysique);

	// Destructeur
	virtual ~Cone();

	virtual void AnimeSpecific(float tempsEcoule) override;
	virtual void Draw() override;

private:
    virtual void InitBuffer() override;
    virtual void InitEffet() override;

	DirectX::XMMATRIX matWorld;
	float rotation;

    float hauteur;
    float rayonBase;

    physx::PxShape* shape;
};

} // namespace PM3D

#endif
