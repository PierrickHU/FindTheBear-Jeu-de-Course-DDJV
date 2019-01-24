#ifndef C_TERRAIN_H
#define C_TERRAIN_H

#include "ISerializable.h"
#include "ScenePhysique.h"
#include "Component/MeshComponent.h"
#include "Component/PhysicTerrainComponent.h"

#include <memory>

namespace PM3D
{
     class ScenePhysique;
     class CTerrain : public CGameObject, ISerializable
     {
     public:
         int16_t m_Width;
         int16_t m_Depth;
         DirectX::XMFLOAT3 middlePosition;
         DirectX::XMFLOAT3 size;
         std::vector<float> m_Hauteurs;

         std::unique_ptr<PhysicTerrainComponent> physic;
         MeshComponent mesh;

     public:
         CTerrain(Orientation&&, CDispositifD3D11*, ScenePhysique& scenePhysique);
         virtual ~CTerrain() = default;

         virtual void Draw(ID3D11ShaderResourceView* pDepthShaderResourceView = nullptr) override;
         virtual void DrawShadow(const XMMATRIX matViewProjLight);
         virtual void Serialize() override;

         DirectX::XMFLOAT3 getSize() const noexcept;
         DirectX::XMFLOAT3 getMiddlePosition() const noexcept;
         float getHauteur(const float& x, const float& z) const;
         float getHauteurRaycast(const float& x, const float& z) const ; // Vaut mieux utiliser ca si vous pouvez

         bool isInCentre(int x, int y) const noexcept;
         bool isInTerrain(const float& x, const float& z) const;
         float getRayonChateau() const;
         float getRayonCarte() const; // Le rayon est un peut sous-évalué pour éviter de se faire avoir avec les crenelages sur les bords

     private:
          virtual void Update(const float tempsEcoule) override;
          virtual void Unserialize() override;
 
          void Create(const std::vector<float>&);
          void CalculerNormales();
          void ConstuireIndex();
          void ConstuireCoordTex();
			 
			 struct TempVertexType
			 {
				 float x, y, z;
				 float tu, tv;
				 float nx, ny, nz;
			 };
			 struct VectorType
			 {
				 float x, y, z;
			 };

			 void CalculateTerrainVectors();
			 void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);

     };

} //	namespace PM3D

#endif // !C_TERRAIN_H