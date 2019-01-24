#include "stdafx.h"
#include "Orientation.h"
#include "toolsMath.h"
#include "PxPhysicsAPI.h"
#include <math.h>
using namespace DirectX;
using namespace physx;

namespace PM3D {

    Orientation::Orientation() :
        position{ XMVECTOR{0.0f, 0.0f, 0.0f} },
        quaternion{ DirectX::XMQuaternionIdentity() }
    {}
    Orientation::Orientation(const XMFLOAT3& _position) :
        position{ XMVECTOR{_position.x, _position.y, _position.z} },
        quaternion{ DirectX::XMQuaternionIdentity() }
    {}
    Orientation::Orientation(const XMFLOAT3&& _position) :
        position{ XMVECTOR{std::move(_position.x), std::move(_position.y), std::move(_position.z)} },
        quaternion{ DirectX::XMQuaternionIdentity() }
    {}
    Orientation::Orientation(const XMVECTOR& _position) :
        position{ _position },
        quaternion{ DirectX::XMQuaternionIdentity() }
    {}
    Orientation::Orientation(const XMVECTOR&& _position) :
        position{ std::move(_position) },
        quaternion{ DirectX::XMQuaternionIdentity() }
    {}
    Orientation::Orientation(const XMVECTOR& _position, const XMVECTOR& _quaternionDeRotation) :
        position{ _position },
        quaternion{ _quaternionDeRotation }
    {}
    Orientation::Orientation(const XMVECTOR&& _position, const XMVECTOR&& _quaternionDeRotation) :
        position{ std::move(_position) },
        quaternion{ std::move(_quaternionDeRotation) }
    {}
    Orientation::Orientation(const XMFLOAT3& _position, const XMFLOAT3& _direction, const XMFLOAT3& _up) :
        position{ XMVECTOR{_position.x, _position.y, _position.z} }
    {
        setDirection(XMVECTOR{ _direction.x, _direction.y, _direction.z },
                     XMVECTOR{ _up.x, _up.y, _up.z });
    }
    Orientation::Orientation(const XMFLOAT3&& _position, const XMFLOAT3&& _direction, const XMFLOAT3&& _up) :
        position{ XMVECTOR{std::move(_position.x), std::move(_position.y), std::move(_position.z)} }
    {
        setDirection(XMVECTOR{ std::move(_direction.x), std::move(_direction.y), std::move(_direction.z) },
                     XMVECTOR{ std::move(_up.x), std::move(_up.y), std::move(_up.z) });
    }
    Orientation::Orientation(const XMVECTOR& _position, const XMVECTOR& _direction, const XMVECTOR& _up) :
        Orientation(XMFLOAT3{_position.vector4_f32[0], _position.vector4_f32[1], _position.vector4_f32[2]},
                    XMFLOAT3{_direction.vector4_f32[0], _direction.vector4_f32[1], _direction.vector4_f32[2]},
                    XMFLOAT3{_up.vector4_f32[0], _up.vector4_f32[1], _up.vector4_f32[2]})
    {}
    Orientation::Orientation(const XMVECTOR&& _position, const XMVECTOR&& _direction, const XMVECTOR&& _up) :
        Orientation(XMFLOAT3{std::move(_position.vector4_f32[0]), std::move(_position.vector4_f32[1]), std::move(_position.vector4_f32[2])},
                    XMFLOAT3{std::move(_direction.vector4_f32[0]), std::move(_direction.vector4_f32[1]), std::move(_direction.vector4_f32[2])},
                    XMFLOAT3{std::move(_up.vector4_f32[0]), std::move(_up.vector4_f32[1]), std::move(_up.vector4_f32[2])})
    {}
 
    XMVECTOR Orientation::getPosition() const { return position; }

    float Orientation::getPositionX() const { return position.vector4_f32[0]; }
    float Orientation::getPositionY() const { return position.vector4_f32[1]; }
    float Orientation::getPositionZ() const { return position.vector4_f32[2]; }

    XMVECTOR Orientation::getQuaternion() const { return quaternion; }
    XMFLOAT3 Orientation::getScale() const { return scale; }
    void Orientation::setScale(XMFLOAT3 newScale) {
        scale = newScale;
    }

    XMVECTOR Orientation::getDirection() const {
        XMVECTOR directionInitiale{0.0f, 0.0f, 1.0f};
        return rotateVectorWithQuaternion(directionInitiale, quaternion);
    }

    XMVECTOR Orientation::getLookAt() const {
        return getDirection() + position;
    }

    float Orientation::getDirectionX() const {
        return getDirection().vector4_f32[0];
    }
    float Orientation::getDirectionY() const {
        return getDirection().vector4_f32[1];
    }
    float Orientation::getDirectionZ() const {
        return getDirection().vector4_f32[2];
    }

    XMVECTOR Orientation::getUp() const {
        XMVECTOR directionInitiale{0.0f, 1.0f, 0.0f};
        return rotateVectorWithQuaternion(directionInitiale, quaternion);
    }

    float Orientation::getUpX() const {
        return getUp().vector4_f32[0];
    }
    float Orientation::getUpY() const {
        return getUp().vector4_f32[1];
    }
    float Orientation::getUpZ() const {
        return getUp().vector4_f32[2];
    }

    void Orientation::rotateWithQuaternion(const XMVECTOR& _quaternion) {
        quaternion = DirectX::XMQuaternionMultiply(_quaternion, quaternion); // OMG C'ETAIT DANS CE SENS LA !!! Mais d'où c'est le nouveau en premier ???? x) Bon ça marche c'est cool :3
       quaternion = DirectX::XMQuaternionNormalize(quaternion);
    }
    void Orientation::translate(const XMVECTOR& _direction) {
        position += _direction;
    }

    void Orientation::setPosition(const DirectX::XMVECTOR& newPosition) {
        position = newPosition;
    }
    void Orientation::setQuaternion(const DirectX::XMVECTOR& newRotation) {
        quaternion = newRotation;
        quaternion = DirectX::XMQuaternionNormalize(quaternion);
    }
    void Orientation::setDirection(const DirectX::XMVECTOR& newDirection, const DirectX::XMVECTOR& newUp) {
        XMMATRIX m = DirectX::XMMatrixLookToLH(getPosition(), newDirection, newUp);
        XMVECTOR q = DirectX::XMQuaternionRotationMatrix(m);
        quaternion = q;
    }

    DirectX::XMMATRIX Orientation::calculMatriceWorld() {
        XMVECTOR inverseQuatY = getQuaternion();
        inverseQuatY.vector4_f32[1] *= -1;

        return DirectX::XMMatrixScalingFromVector(convertIntoVector(getScale()))
            * getMatriceRotationFromQuaternion(inverseQuatY)
            * getMatriceTranslation(getPosition());
    }
}
