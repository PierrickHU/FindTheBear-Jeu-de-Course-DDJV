#ifndef ORIENTATION_H
#define ORIENTATION_H

#include "toolsMath.h"

namespace PM3D {

    class Orientation {
    private:
        DirectX::XMVECTOR position; // Ceci est un vecteur3 !
        DirectX::XMVECTOR quaternion; // Ceci est maintenant un quaternion ! ...
        DirectX::XMFLOAT3 scale = { 1.f ,1.f, 1.f };

    public:
        // Je dirais qu'il y a un peu, beaucoup (trop ?), de constructeurs ! x)
        Orientation();
        Orientation(const DirectX::XMFLOAT3& _position);
        Orientation(const DirectX::XMFLOAT3&& _position);
        Orientation(const DirectX::XMVECTOR& _position);
        Orientation(const DirectX::XMVECTOR&& _position);
        Orientation(const DirectX::XMVECTOR& _position, const DirectX::XMVECTOR& _quaternionDeRotation);
        Orientation(const DirectX::XMVECTOR&& _position, const DirectX::XMVECTOR&& _quaternionDeRotation);
        Orientation(const DirectX::XMFLOAT3& _position, const DirectX::XMFLOAT3& _direction, const DirectX::XMFLOAT3& _up);
        Orientation(const DirectX::XMFLOAT3&& _position, const DirectX::XMFLOAT3&& _direction, const DirectX::XMFLOAT3&& _up);
        Orientation(const DirectX::XMVECTOR& _position, const DirectX::XMVECTOR& _direction, const DirectX::XMVECTOR& _up);
        Orientation(const DirectX::XMVECTOR&& _position, const DirectX::XMVECTOR&& _direction, const DirectX::XMVECTOR&& _up);

        DirectX::XMVECTOR getPosition() const;
        float getPositionX() const;
        float getPositionY() const;
        float getPositionZ() const;

        DirectX::XMVECTOR getQuaternion() const;

        DirectX::XMFLOAT3 getScale() const;
        void setScale(DirectX::XMFLOAT3);

        DirectX::XMVECTOR getLookAt() const;

        DirectX::XMVECTOR getDirection() const;
        float getDirectionX() const;
        float getDirectionY() const;
        float getDirectionZ() const;

        DirectX::XMVECTOR getUp() const;
        float getUpX() const;
        float getUpY() const;
        float getUpZ() const;

        void rotateWithQuaternion(const DirectX::XMVECTOR& quaternion);
        void translate(const DirectX::XMVECTOR& direction);
        void setPosition(const DirectX::XMVECTOR& newPosition);
        void setQuaternion(const DirectX::XMVECTOR& newQuaternion);
        void setDirection(const DirectX::XMVECTOR& newDirection, const DirectX::XMVECTOR& newUp);

        DirectX::XMMATRIX calculMatriceWorld();
    };

}

#endif
