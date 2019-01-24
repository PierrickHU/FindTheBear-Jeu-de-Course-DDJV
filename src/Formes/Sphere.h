#ifndef SPHERE_H
#define SPHERE_H

#include "CObjetMesh.h"
#include "dispositifD3D11.h"
#include "PxPhysicsAPI.h"
#include "ScenePhysique.h"

namespace PM3D
{
// Permet de créer des Sphéres ! <3
class Sphere : public CObjetMesh{
public:
    Sphere(DirectX::XMFLOAT3& _position, const float rayon, const int echantillonnageVertical, const int echantillonnageHorizontal, CDispositifD3D11* pDispositif_, ScenePhysique& scenePhysique);
	virtual ~Sphere();

	virtual void AnimeSpecific(float tempsEcoule) override;
	virtual void Draw() override;

private:
    virtual void InitBuffer() override;
    virtual void InitEffet() override;

	// Définitions des valeurs d'animation
    DirectX::XMMATRIX matWorld;
	float rotation;
    float rayon;

    physx::PxShape* shape = NULL;
    physx::PxActor* actor = NULL;
};

} // namespace PM3D

#endif
