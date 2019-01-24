#include "stdafx.h"
#include "toolsMath.h"
#include <limits>
#include <cmath>

using namespace DirectX;
using namespace physx;

inline DirectX::XMVECTOR getQuaternionRotation(const DirectX::FXMVECTOR& axis, const float angle) {
   return (equalsFloat(angle, 0.f)) ? DirectX::XMQuaternionIdentity() : DirectX::XMQuaternionRotationAxis(axis, angle);
}
DirectX::XMVECTOR getQuaternionRotation(const DirectX::FXMVECTOR& axis1, const float angle1, const DirectX::FXMVECTOR& axis2, const float angle2) {
   return DirectX::XMQuaternionMultiply(getQuaternionRotation(axis1, angle1), getQuaternionRotation(axis2, angle2));
}
DirectX::XMVECTOR rotateVectorWithQuaternion(DirectX::FXMVECTOR& vector, const DirectX::FXMVECTOR& quaternion, const DirectX::FXMVECTOR& pointRotation) {
   return DirectX::XMQuaternionMultiply(DirectX::XMQuaternionMultiply(quaternion, vector - pointRotation), DirectX::XMQuaternionInverse(quaternion)) + pointRotation;
}

DirectX::XMMATRIX getMatriceRotation(const DirectX::FXMVECTOR& _axis, const float _angle) {
   return DirectX::XMMatrixRotationAxis(_axis, _angle);
}
DirectX::XMMATRIX getMatriceRotationFromQuaternion(const DirectX::FXMVECTOR& _quaternion) {
   return DirectX::XMMatrixRotationQuaternion(_quaternion);
}

DirectX::XMMATRIX getMatriceTranslation(const float _translationX, const float _translationY, const float _translationZ) {
   return DirectX::XMMatrixTranslation(_translationX, _translationY, _translationZ);
}
DirectX::XMMATRIX getMatriceTranslation(const DirectX::FXMVECTOR& translation) {
   return DirectX::XMMatrixTranslationFromVector(translation);
}

DirectX::XMMATRIX getMatriceTransposition(const DirectX::FXMMATRIX& _matrice) {
   return DirectX::XMMatrixTranspose(_matrice);
}

float getAngleVecteur(const DirectX::FXMVECTOR& vector1, const DirectX::FXMVECTOR& vector2, const DirectX::FXMVECTOR& _axisRotation) {
   DirectX::XMVECTOR vectorNormal = normaliseVecteur(produitVectoriel(vector1, _axisRotation));
   float angle1 = DirectX::XMVector3AngleBetweenNormals(normaliseVecteur(vector1), normaliseVecteur(vector2)).vector4_f32[0];
   float angle2 = DirectX::XMVector3AngleBetweenNormals(normaliseVecteur(vectorNormal), normaliseVecteur(vector2)).vector4_f32[0];
   float signe = (angle2 >= DirectX::XM_PIDIV2 || angle2 <= -DirectX::XM_PIDIV2) ? 1.f : -1.f;
   return signe * angle1;
}

inline DirectX::XMMATRIX getMatriceIdentite() {
   return DirectX::XMMatrixIdentity();
}

DirectX::XMVECTOR normaliseVecteur(const DirectX::FXMVECTOR& _vector) {
   return DirectX::XMVector3Normalize(_vector);
}

float normeVecteur(const DirectX::FXMVECTOR& _vector) {
   return DirectX::XMVector3Length(_vector).vector4_f32[0];
}

DirectX::XMVECTOR produitVectoriel(const DirectX::FXMVECTOR& _vector1, const DirectX::FXMVECTOR& _vector2) {
   return DirectX::XMVector3Cross(_vector1, _vector2);
}

DirectX::XMVECTOR getProjectionPlanaire(const DirectX::FXMVECTOR& _vector, const DirectX::FXMVECTOR& _vectorNormalPlan, const DirectX::FXMVECTOR& _pointPlan) {
   DirectX::XMVECTOR vecteurProjete;
   DirectX::XMVECTOR vecteurNormal1 = normaliseVecteur(produitVectoriel(_vectorNormalPlan, _vector));
   DirectX::XMVECTOR vecteurNormal2 = normaliseVecteur(produitVectoriel(_vectorNormalPlan, vecteurNormal1));
   vecteurProjete = (_vector*vecteurNormal1)*vecteurNormal1 + (_vector*vecteurNormal2)*vecteurNormal2;
   return vecteurProjete;
}

float distanceBetweenTwoPoints(const DirectX::FXMVECTOR& vector1, DirectX::FXMVECTOR& vector2) {
    return normeVecteur(vector1 - vector2);
}

DirectX::XMVECTOR convertIntoVector(const DirectX::XMFLOAT3& vecteur) {
   return DirectX::XMLoadFloat3(&vecteur);
}

inline DirectX::XMFLOAT3 convertIntoFloat3(const DirectX::XMVECTOR& vecteur) {
   return DirectX::XMFLOAT3{ vecteur.vector4_f32[0], vecteur.vector4_f32[1], vecteur.vector4_f32[2] };
}

PxVec3 toPxVec3(const XMFLOAT3& vecteur) {
    return {vecteur.x, vecteur.y, vecteur.z};
}
XMFLOAT3 toXMFLOAT3(const PxVec3& vecteur) {
    return {vecteur.x, vecteur.y, vecteur.z};
}

XMVECTOR toVXMVECTOR(const PxVec3& vecteur) {
    return {vecteur.x, vecteur.y, vecteur.z};
}

PxVec3 toPxVec3(const XMVECTOR& vecteur) {
    return {vecteur.vector4_f32[0], vecteur.vector4_f32[1], vecteur.vector4_f32[2]};
}

physx::PxQuat toPxQuat(const DirectX::XMVECTOR& quat) {
    PxQuat q2{quat.vector4_f32[0], quat.vector4_f32[1], quat.vector4_f32[2], quat.vector4_f32[3]};
    return q2;
}

inline bool equalsFloat(const float& a, const float& b, const float& precision) {
   return fabs(a - b) < precision;
}