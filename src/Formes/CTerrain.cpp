#include "stdafx.h"
#include "CTerrain.h"
#include "util.h"
#include "DispositifD3D11.h"
#include "resource.h"
#include "MoteurWindows.h"
#include "ScenePhysique.h"
#include "Orientation.h"
#include "ShadersParams.h"

#include <fstream>
#include <iostream>
#include <cassert>
#include <algorithm>
using namespace physx;

//const std::string filenameHeightmap = "../ressources/terrains/heightmap.txt";
const std::string filenameHeightmap = "../ressources/terrains/250_250.txt";
const std::string filenameTerrain = "../ressources/terrain.dat";

namespace PM3D
{
   CTerrain::CTerrain(Orientation&& orientation, CDispositifD3D11* pDispositif, ScenePhysique& scenePhysique)
      : CGameObject(std::move(orientation))
   {
      Unserialize();
      physic = std::make_unique<PhysicTerrainComponent>(this, scenePhysique);

      mesh.shaderFilename = L"../ressources/shaders/TerrainNM.fx";
      mesh.Init(pDispositif);
   }

   void CTerrain::Draw(ID3D11ShaderResourceView* pDepthShaderResourceView)
   {
      mesh.Draw(matWorld, pDepthShaderResourceView);
   }

   void CTerrain::DrawShadow(const XMMATRIX matViewProjLight) {
      mesh.DrawShadow(matWorld, matViewProjLight);
   }

   void CTerrain::Update(const float tempsEcoule) {
      calculMatriceWorld();
   };

   void CTerrain::Unserialize()
   {
      std::fstream file;
      file.open(filenameHeightmap, std::fstream::in);
      assert(file.is_open());

      file >> m_Width;
      assert(m_Width > 0);
      file >> m_Depth;
      assert(m_Depth > 0);
      m_Hauteurs.reserve(m_Width * m_Depth);

      float scaleXZ, scaleY;
      file >> scaleXZ;
      assert(scaleXZ > 0);
      file >> scaleY;
      assert(scaleY > 0);
      orientation.setScale({ scaleXZ, scaleY, scaleXZ });

      int hauteur;
      while (file.good())
      {
         file >> hauteur;
         m_Hauteurs.push_back(hauteur);
      }
      assert(m_Hauteurs.size() == m_Width * m_Depth);

      size = {
       m_Width * scaleXZ,
       *std::max_element(begin(m_Hauteurs), end(m_Hauteurs)) * scaleY,
       m_Depth * scaleXZ };

      Create(std::move(m_Hauteurs));
   }

   void CTerrain::Create(const std::vector<float>& m_Hauteurs)
   {
      mesh.m_Sommets.reserve(m_Width * m_Depth);

      auto scaleX = orientation.getScale().x;
      auto scaleY = orientation.getScale().y;
      auto scaleZ = orientation.getScale().z;

      for (int j = 0; j < m_Depth; ++j)
      {
         for (int i = 0; i < m_Width; ++i)
         {
            mesh.m_Sommets.push_back(CSommet{ {
                   i * scaleX,
                   m_Hauteurs[i + j * m_Width] * scaleY,
                   j * scaleZ
                } });
         }
      }

      assert(mesh.m_Sommets.size() == m_Hauteurs.size());

      middlePosition = { m_Width * scaleX / 2 - 1, 0 , m_Depth * scaleZ / 2 - 1 };

      mesh.materials.emplace_back(CMoteurWindows::GetInstance().GetRessourcesManager().GetTexture(L"../ressources/textures/stone.dds"));
      mesh.materials.emplace_back(CMoteurWindows::GetInstance().GetRessourcesManager().GetTexture(L"../ressources/textures/grass.dds"));
      mesh.materials.emplace_back(CMoteurWindows::GetInstance().GetRessourcesManager().GetTexture(L"../ressources/textures/brick.dds"));
      mesh.materials.emplace_back(CMoteurWindows::GetInstance().GetRessourcesManager().GetTexture(L"../ressources/textures/stoneNM.dds"));
      mesh.materials.emplace_back(CMoteurWindows::GetInstance().GetRessourcesManager().GetTexture(L"../ressources/textures/grassNM.dds"));
      mesh.materials.emplace_back(CMoteurWindows::GetInstance().GetRessourcesManager().GetTexture(L"../ressources/textures/brickNM.dds"));
      CalculerNormales();
      ConstuireIndex();
      ConstuireCoordTex();
      CalculateTerrainVectors();
      //// Ne conserver que les coordonnees qui comptent
      //vector<CSommet> sommetsTmp = mesh.m_Sommets;
      //vector<uint16_t> indexTmp = mesh.m_Indices;
      //vector<CSommet> newSommets;
      //vector<uint16_t> newIndex;
      //for (int x = 0; x < m_Width; x++) {
      //    for (int y = 0; y < m_Depth; y++) {
      //        if (isInCentre(x, y)) {
      //            int i = x + y * m_Width;
      //            newSommets.push_back(mesh.m_Sommets[i]);
      //            newIndex.push_back(mesh.m_Indices[i]);
      //        }
      //    }
      //}
      //mesh.m_Sommets = newSommets;
      //mesh.m_Indices = newIndex;
   }

   void CTerrain::ConstuireCoordTex() {
      int nbTexturesSelonX = 50; // Toutes les texutres doivent avoir la même taille de 4096*4096
      int nbTexturesSelonZ = 50;
      float tailleTerrainX = (m_Width - 1) * orientation.getScale().x;
      float tailleTerrainZ = (m_Depth - 1) * orientation.getScale().z;
      for (auto& sommet : mesh.m_Sommets) {
         float avancementX = sommet.m_Position.x / tailleTerrainX;
         float avancementZ = sommet.m_Position.z / tailleTerrainZ;
         float texCoordX = std::fmod(avancementX, tailleTerrainX / nbTexturesSelonX) * nbTexturesSelonX;
         float texCoordZ = std::fmod(avancementZ, tailleTerrainZ / nbTexturesSelonZ) * nbTexturesSelonZ;
         sommet.m_TexCoord = { texCoordX, texCoordZ };
         //sommet.m_TexCoord = { sommet.m_Position.x / ((m_Width - 1)*orientation.getScale().x),
             //1 - sommet.m_Position.z / ((m_Width - 1)*orientation.getScale().z) };
      }
   }

   void CTerrain::CalculateTerrainVectors()
   {
      int faceCount, i, index;
      TempVertexType vertex1, vertex2, vertex3;
      VectorType tangent, binormal;


      // Calculate the number of faces in the terrain model.
      faceCount = mesh.m_Indices.size() / 3;

      // Initialize the index to the model data.
      index = 0;

      // Go through all the faces and calculate the the tangent, binormal, and normal vectors.
      for (i = 0; i < faceCount; i++)
      {

         auto indice1 = mesh.m_Indices[index];
         // Get the three vertices for this face from the terrain model.
         vertex1.x = mesh.m_Sommets[indice1].m_Position.x;
         vertex1.y = mesh.m_Sommets[indice1].m_Position.y;
         vertex1.z = mesh.m_Sommets[indice1].m_Position.z;
         vertex1.tu = mesh.m_Sommets[indice1].m_TexCoord.x;
         vertex1.tv = mesh.m_Sommets[indice1].m_TexCoord.y;
         vertex1.nx = mesh.m_Sommets[indice1].m_Normal.x;
         vertex1.ny = mesh.m_Sommets[indice1].m_Normal.y;
         vertex1.nz = mesh.m_Sommets[indice1].m_Normal.z;
         index++;

         auto indice2 = mesh.m_Indices[index];

         vertex2.x = mesh.m_Sommets[indice2].m_Position.x;
         vertex2.y = mesh.m_Sommets[indice2].m_Position.y;
         vertex2.z = mesh.m_Sommets[indice2].m_Position.z;
         vertex2.tu = mesh.m_Sommets[indice2].m_TexCoord.x;
         vertex2.tv = mesh.m_Sommets[indice2].m_TexCoord.y;
         vertex2.nx = mesh.m_Sommets[indice2].m_Normal.x;
         vertex2.ny = mesh.m_Sommets[indice2].m_Normal.y;
         vertex2.nz = mesh.m_Sommets[indice2].m_Normal.z;
         index++;

         auto indice3 = mesh.m_Indices[index];

         vertex3.x = mesh.m_Sommets[indice3].m_Position.x;
         vertex3.y = mesh.m_Sommets[indice3].m_Position.y;
         vertex3.z = mesh.m_Sommets[indice3].m_Position.z;
         vertex3.tu = mesh.m_Sommets[indice3].m_TexCoord.x;
         vertex3.tv = mesh.m_Sommets[indice3].m_TexCoord.y;
         vertex3.nx = mesh.m_Sommets[indice3].m_Normal.x;
         vertex3.ny = mesh.m_Sommets[indice3].m_Normal.y;
         vertex3.nz = mesh.m_Sommets[indice3].m_Normal.z;
         index++;

         // Calculate the tangent and binormal of that face.
         CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

         // Store the tangent and binormal for this face back in the model structure.
         mesh.m_Sommets[indice3].m_Tangent.x = tangent.x;
         mesh.m_Sommets[indice3].m_Tangent.y = tangent.y;
         mesh.m_Sommets[indice3].m_Tangent.z = tangent.z;
         mesh.m_Sommets[indice3].m_Bitangent.x = binormal.x;
         mesh.m_Sommets[indice3].m_Bitangent.y = binormal.y;
         mesh.m_Sommets[indice3].m_Bitangent.z = binormal.z;

         mesh.m_Sommets[indice2].m_Tangent.x = tangent.x;
         mesh.m_Sommets[indice2].m_Tangent.y = tangent.y;
         mesh.m_Sommets[indice2].m_Tangent.z = tangent.z;
         mesh.m_Sommets[indice2].m_Bitangent.x = binormal.x;
         mesh.m_Sommets[indice2].m_Bitangent.y = binormal.y;
         mesh.m_Sommets[indice2].m_Bitangent.z = binormal.z;

         mesh.m_Sommets[indice1].m_Tangent.x = tangent.x;
         mesh.m_Sommets[indice1].m_Tangent.y = tangent.y;
         mesh.m_Sommets[indice1].m_Tangent.z = tangent.z;
         mesh.m_Sommets[indice1].m_Bitangent.x = binormal.x;
         mesh.m_Sommets[indice1].m_Bitangent.y = binormal.y;
         mesh.m_Sommets[indice1].m_Bitangent.z = binormal.z;
      }

      return;



   }

   void CTerrain::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangent, VectorType& binormal)
   {
      float vector1[3], vector2[3];
      float tuVector[2], tvVector[2];
      float den;
      float length;


      // Calculate the two vectors for this face.
      vector1[0] = vertex2.x - vertex1.x;
      vector1[1] = vertex2.y - vertex1.y;
      vector1[2] = vertex2.z - vertex1.z;

      vector2[0] = vertex3.x - vertex1.x;
      vector2[1] = vertex3.y - vertex1.y;
      vector2[2] = vertex3.z - vertex1.z;

      // Calculate the tu and tv texture space vectors.
      tuVector[0] = vertex2.tu - vertex1.tu;
      tvVector[0] = vertex2.tv - vertex1.tv;

      tuVector[1] = vertex3.tu - vertex1.tu;
      tvVector[1] = vertex3.tv - vertex1.tv;

      // Calculate the denominator of the tangent/binormal equation.
      den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

      // Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
      tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
      tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
      tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

      binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
      binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
      binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

      // Calculate the length of the tangent.
      length = (float)sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

      // Normalize the tangent and then store it.
      tangent.x = tangent.x / length;
      tangent.y = tangent.y / length;
      tangent.z = tangent.z / length;

      // Calculate the length of the binormal.
      length = (float)sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

      // Normalize the binormal and then store it.
      binormal.x = binormal.x / length;
      binormal.y = binormal.y / length;
      binormal.z = binormal.z / length;

      return;
   }

   void CTerrain::CalculerNormales()
   {
      for (size_t j = 0; j < m_Depth; ++j)
      {
         for (size_t i = 0; i < m_Width; ++i)
         {
            DirectX::XMVECTOR normal{ 0.f,0.f,0.f,0.f };

            const bool voisinGauche = i > 0;
            const bool voisinHaut = j < m_Width - 1;
            const bool voisinDroite = i < m_Width - 1;
            const bool voisinBas = j > 0;

            const int indexCourant = static_cast<int>(i + j * m_Width);
            DirectX::XMVECTOR positionCourante = XMLoadFloat3(&mesh.m_Sommets[indexCourant].m_Position);
            DirectX::XMVECTOR vectGauche;
            DirectX::XMVECTOR vectHaut;
            DirectX::XMVECTOR vectDroite;
            DirectX::XMVECTOR vectBas;

            if (voisinGauche)
            {
               vectGauche = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&mesh.m_Sommets[indexCourant - 1].m_Position), positionCourante);
            }
            if (voisinHaut)
            {
               vectHaut = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&mesh.m_Sommets[indexCourant + m_Width].m_Position), positionCourante);
            }
            if (voisinDroite)
            {
               vectDroite = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&mesh.m_Sommets[indexCourant + 1].m_Position), positionCourante);
            }
            if (voisinBas)
            {
               vectBas = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&mesh.m_Sommets[indexCourant - m_Width].m_Position), positionCourante);
            }

            if (voisinGauche && voisinHaut)
            {
               normal = normaliseVecteur(DirectX::XMVectorAdd(normal, DirectX::XMVector3Cross(vectHaut, vectGauche)));
            }
            if (voisinHaut && voisinDroite)
            {
               normal = normaliseVecteur(DirectX::XMVectorAdd(normal, DirectX::XMVector3Cross(vectHaut, vectDroite)));
            }
            if (voisinDroite && voisinBas)
            {
               normal = normaliseVecteur(DirectX::XMVectorAdd(normal, DirectX::XMVector3Cross(vectBas, vectDroite)));
            }
            if (voisinBas && voisinGauche)
            {
               normal = normaliseVecteur(DirectX::XMVectorAdd(normal, DirectX::XMVector3Cross(vectGauche, vectBas)));
            }

            DirectX::XMStoreFloat3(&mesh.m_Sommets[i + j * m_Width].m_Normal, normal);
         }
      }
   }

   void CTerrain::ConstuireIndex()
   {
      const int indicesSize = 6 * (m_Width - 1) * (m_Depth - 1);
      mesh.m_Indices.resize(indicesSize);

      int index = 0;

      for (int y = 0; y < m_Depth - 1; ++y)
      {
         for (int x = 0; x < m_Width - 1; ++x)
         {
            if (isInCentre(x, y) && isInCentre(x + 1, y + 1) && isInCentre(x + 1, y)) {
               mesh.m_Indices[index++] = y * m_Width + x;
               mesh.m_Indices[index++] = (y + 1) * m_Width + (x + 1);
               mesh.m_Indices[index++] = y * m_Width + (x + 1);
            }
            if (isInCentre(x, y) && isInCentre(x, y + 1) && isInCentre(x + 1, y + 1)) {
               mesh.m_Indices[index++] = y * m_Width + x;
               mesh.m_Indices[index++] = (y + 1) * m_Width + x;
               mesh.m_Indices[index++] = (y + 1) * m_Width + (x + 1);
            }
         }
      }
   }

   DirectX::XMFLOAT3 CTerrain::getSize() const noexcept {
      return size;
   }

   DirectX::XMFLOAT3 CTerrain::getMiddlePosition() const noexcept {
      return middlePosition;
   }

   void CTerrain::Serialize()
   {
      std::fstream file(filenameTerrain, std::fstream::out | std::fstream::binary);

      assert(file.is_open());

      file.write(reinterpret_cast<const char*>(&m_Width), sizeof(m_Width));
      file.write(reinterpret_cast<const char*>(&m_Depth), sizeof(m_Depth));

      file.write(reinterpret_cast<const char*>(mesh.m_Sommets.data()), mesh.m_Sommets.size() * sizeof(CSommet));
      file.write(reinterpret_cast<const char*>(mesh.m_Indices.data()), mesh.m_Indices.size() * sizeof(CSommet));
   }

   float CTerrain::getHauteur(const float& x, const float& z) const {
      float hauteur = 0.f;
      float scaleX = orientation.getScale().x;
      float scaleZ = orientation.getScale().z;
      float scaleY = orientation.getScale().y;
      if (x >= 0.f && x < m_Width*scaleX && z >= 0.f && z < m_Depth*scaleZ) {
         int indexLigneZ = z / scaleZ;
         int indexColonneX = x / scaleX;
         hauteur = m_Hauteurs[indexLigneZ * m_Width + indexColonneX] * scaleY;
      }
      else {
         throw;
      }
      return hauteur;
   }

   class raycast_fail {};
   float CTerrain::getHauteurRaycast(const float& x, const float& z) const {
      // On va lancer un raycast vers le bas depuis (x, hauteurMax, z) et voir la distance qu'il parcours !
      ScenePhysique sp = CMoteurWindows::GetInstance().GetScenePhysique();
      PxVec3 direction{ 0.0f, -1.0f, 0.0f };
      PxReal hauteurMax = size.y + 1.f;
      PxReal distanceMax = hauteurMax;
      const PxHitFlags outputFlags = PxHitFlag::eDISTANCE;
      PxQueryFilterData filterData = PxQueryFilterData();
      filterData.data.word0 = ScenePhysique::Filter_Group::TERRAIN;
      PxRaycastBuffer hit;

      PxVec3 depart{ x, hauteurMax, z };
      if (sp.gScene->raycast(depart, direction, distanceMax, hit, outputFlags, filterData)) {
         return hauteurMax - hit.block.distance;
      }
      throw raycast_fail{};
   }

   bool CTerrain::isInTerrain(const float& x, const float& z) const {
      // On va lancer un raycast vers le bas depuis (x, hauteurMax, z) et voir la distance qu'il parcours !
      ScenePhysique sp = CMoteurWindows::GetInstance().GetScenePhysique();
      PxVec3 direction{ 0.0f, -1.0f, 0.0f };
      PxReal hauteurMax = size.y + 1.f;
      PxReal distanceMax = hauteurMax;
      const PxHitFlags outputFlags = PxHitFlag::eDISTANCE;
      PxQueryFilterData filterData = PxQueryFilterData();
      filterData.data.word0 = ScenePhysique::Filter_Group::TERRAIN;
      PxRaycastBuffer hit;

      PxVec3 depart{ x, hauteurMax, z };
      return sp.gScene->raycast(depart, direction, distanceMax, hit, outputFlags, filterData);
   }

   bool CTerrain::isInCentre(int xInd, int yInd) const noexcept {
      int xIndCentre = m_Width / 2;
      int yIndCentre = m_Depth / 2;
      float rayon = (m_Width + m_Depth) / 4.f;
      int dx = abs(xInd - xIndCentre);
      int dy = abs(yInd - yIndCentre);
      float distance = sqrt(dx * dx + dy * dy);
      return distance <= rayon;
   }

    float CTerrain::getRayonChateau() const {
        float rayon = (m_Width * orientation.getScale().x + m_Depth * orientation.getScale().z) / 4.f; // Rayon du terrain entier
        float ratioChateau = 0.5f; // constante à modifier
        return rayon * ratioChateau;
     }

    float CTerrain::getRayonCarte() const {
        float rayon = (m_Width * orientation.getScale().x + m_Depth * orientation.getScale().z) / 4.f; // Rayon du terrain entier
        return rayon;
     }

} // namespace PM3D
