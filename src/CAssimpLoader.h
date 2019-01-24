#ifndef ASSIMP_LOADER_H
#define ASSIMP_LOADER_H

#include "CFileLoader.h"

namespace PM3D
{
     class CChargeurAssimp :
public IChargeur
     {
     protected:
          // Objet de classe Importer (Assimp)
          Assimp::Importer* pImporter;
          const aiScene* scene;

     public:
          CChargeurAssimp();
          ~CChargeurAssimp();

          virtual void Chargement(const CParametresChargement& param);

          virtual int  GetNombreSubmesh();
          virtual int	 GetNombreSommetsSubmesh(unsigned int n);
          virtual DirectX::XMFLOAT3 GetPosition(int noMesh, int NoSommet);
          virtual DirectX::XMFLOAT2 GetCoordTex(int noMesh, int NoSommet);
          virtual DirectX::XMFLOAT3 GetNormale(int noMesh, int NoSommet);
          virtual unsigned int GetIndice(int noMesh, int noPoly, int NoIndice);
          virtual int  GetNombrePolygonesSubmesh(int noMesh);

          virtual int  GetNombreMaterial();

          virtual void GetMaterial(int _i,
               std::string& _NomFichierTexture,
              std::string& _NomMateriau,
               DirectX::XMFLOAT4& _Ambient,
               DirectX::XMFLOAT4& _Diffuse,
               DirectX::XMFLOAT4& _Specular,
               float& _Puissance);

          virtual int GetMaterialIndex(int i);
     };
}

#endif // !ASSIMP_LOADER_H