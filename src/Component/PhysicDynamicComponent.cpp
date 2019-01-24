#include "stdafx.h"
#include "PhysicDynamicComponent.h"
#include "Formes/CTerrain.h"
#include "PxPhysicsAPI.h"
#include "ScenePhysique.h"
#include "toolsMath.h"
using namespace physx;

#undef min
#undef max

namespace PM3D
{
    void PhysicDynamicComponent::UpdateSimulation(Orientation orientation, const CTerrain& terrain, ScenePhysique& sp)
    {
        // Si on est trop proche du sol, alors la linearVelocity en y est minor� par 0 !
        float linearVelocityY = rigidbody->getLinearVelocity().y;
        float scacleY = orientation.getScale().z;
        float orientationY = orientation.getPositionY();
        float terrainY;
        try {
            terrainY = terrain.getHauteurRaycast(orientation.getPositionX(), orientation.getPositionZ());
            if (orientationY - orientation.getScale().y < terrainY) { // On peut d�commenter ce passage pour une meilleure pr�cision, mais si on fait �a il ne touchera plus le sol :'(
                linearVelocityY = std::max(linearVelocityY, 0.f);
                PxTransform t = rigidbody->getGlobalPose();
                t.p.y = terrainY + orientation.getScale().y;
                rigidbody->setGlobalPose(t);
            }

        // On est en dehors du terrain !
        } catch (...) {
        }

        // Au lieu de setter compl�tement la vitesse, on va setter la vitesse en fonction de la vitesse et de la vitesse pr�c�dente pour amortir !
        //PxVec3 oldLinearVelocity = rigidbody->getLinearVelocity();
        //PxVec3 oldAngularVelocity = rigidbody->getAngularVelocity();
        //PxVec3 newLinearVelocity = (PxVec3{ DirectX::XMVectorGetX(vitesseLineaire), linearVelocityY, DirectX::XMVectorGetZ(vitesseLineaire) } + oldLinearVelocity) / 2.f;
        //PxVec3 newAngularVelocity = (PxVec3{ DirectX::XMVectorGetX(vitesseAngulaire),  DirectX::XMVectorGetY(vitesseAngulaire),  DirectX::XMVectorGetZ(vitesseAngulaire) } + oldAngularVelocity) / 2.f;
        //rigidbody->setLinearVelocity(newLinearVelocity);
        //rigidbody->setAngularVelocity(newAngularVelocity);
        rigidbody->setLinearVelocity(PxVec3{ DirectX::XMVectorGetX(vitesseLineaire), linearVelocityY, DirectX::XMVectorGetZ(vitesseLineaire) });
        rigidbody->setAngularVelocity(PxVec3{ DirectX::XMVectorGetX(vitesseAngulaire),  DirectX::XMVectorGetY(vitesseAngulaire),  DirectX::XMVectorGetZ(vitesseAngulaire) });
    };

    void PhysicDynamicComponent::UpdateFromSimulation(CGameObject* obj)
    {
        auto t = rigidbody->getGlobalPose();
        obj->orientation.setPosition({t.p.x, t.p.y, t.p.z});
        obj->orientation.setQuaternion({t.q.x, -t.q.y, t.q.z, t.q.w }); // J'ai rajout� un - au Y pour que �a match avec physix ! Et pour le moment je crois que �a marche :)
    };

    int PhysicDynamicComponent::GetVitesse()
    {
        return normeVecteur(vitesseLineaire);
    }
}