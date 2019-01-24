#ifndef TOOLS_MATH_H
#define TOOLS_MATH_H

#include "PxPhysicsAPI.h"
#include <DirectXMath.h>

#include <numeric>
#include <vector>

constexpr DirectX::XMVECTOR getAxeX() { return DirectX::XMVECTOR{ 1.f,0.f,0.f }; }
constexpr DirectX::XMVECTOR getAxeY() { return DirectX::XMVECTOR{ 0.f,1.f,0.f }; }
constexpr DirectX::XMVECTOR getAxeZ() { return DirectX::XMVECTOR{ 0.f,0.f,1.f }; }

constexpr DirectX::XMVECTOR getOrigineMonde() { return DirectX::XMVECTOR{ 0.f,0.f,0.f }; }

DirectX::XMVECTOR getQuaternionRotation(const DirectX::FXMVECTOR& axis, const float angle);
DirectX::XMVECTOR getQuaternionRotation(const DirectX::FXMVECTOR& axis1, const float angle1, const DirectX::FXMVECTOR& axis2, const float angle2);

DirectX::XMVECTOR rotateVectorWithQuaternion(const DirectX::FXMVECTOR& vector, const DirectX::FXMVECTOR& quaternion, const DirectX::FXMVECTOR& pointRotation = getOrigineMonde());

DirectX::XMMATRIX getMatriceRotation(const DirectX::FXMVECTOR& axis, const float angle);
DirectX::XMMATRIX getMatriceRotationFromQuaternion(const DirectX::FXMVECTOR& quaternion);

DirectX::XMMATRIX getMatriceTranslation(const float translationX, const float translationY, const float translationZ);
DirectX::XMMATRIX getMatriceTranslation(const DirectX::FXMVECTOR& translation);

DirectX::XMMATRIX getMatriceTransposition(const DirectX::FXMMATRIX& matrice);

float getAngleVecteur(const DirectX::FXMVECTOR& vector1, const DirectX::FXMVECTOR& vector2, const DirectX::FXMVECTOR& axisRotation = getAxeY());

DirectX::XMMATRIX getMatriceIdentite();

DirectX::XMVECTOR normaliseVecteur(const DirectX::FXMVECTOR& vector);

float normeVecteur(const DirectX::FXMVECTOR& _vector);

DirectX::XMVECTOR produitVectoriel(const DirectX::FXMVECTOR& vector1, const DirectX::FXMVECTOR& vector2);

DirectX::XMVECTOR getProjectionPlanaire(const DirectX::FXMVECTOR& vector, const DirectX::FXMVECTOR& vectorNormalPlan, const DirectX::FXMVECTOR& pointPlan);

float distanceBetweenTwoPoints(const DirectX::FXMVECTOR& vector1, DirectX::FXMVECTOR& vector2);

DirectX::XMVECTOR convertIntoVector(const DirectX::XMFLOAT3& vecteur);
DirectX::XMFLOAT3 convertIntoFloat3(const DirectX::XMVECTOR& vecteur);
physx::PxVec3 toPxVec3(const DirectX::XMFLOAT3& vecteur);
DirectX::XMFLOAT3 toXMFLOAT3(const physx::PxVec3& vecteur);
DirectX::XMVECTOR toVXMVECTOR(const physx::PxVec3& vecteur);
physx::PxVec3 toPxVec3(const DirectX::XMVECTOR& vecteur);
physx::PxQuat toPxQuat(const DirectX::XMVECTOR& vecteur);

bool equalsFloat(const float& a, const float& b, const float& precision = std::numeric_limits<float>().epsilon());

template<class T>
T moyenne(const std::vector<T>& list) {
   return std::accumulate(list.begin(), list.end(), T{}, [list](T elem1, T elem2) {
      return elem1 + (elem2 / (T)list.size());
   }
   );
}

#endif // !TOOLS_MATH_H



