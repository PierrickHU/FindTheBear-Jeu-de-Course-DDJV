#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#include "stdafx.h"

namespace PM3D
{

     class CParametresChargement
     {
     public:
          CParametresChargement() = default;
          CParametresChargement(const std::string nomChemin, const std::string nomFichier)
              : NomChemin{ nomChemin }, NomFichier{ nomFichier }
          {}

          std::string NomFichier;
          std::string NomChemin;
          bool bInverserCulling = false;
          bool bMainGauche = false;
     };

     class IChargeur
     {
     public:
          IChargeur() {}
          virtual ~IChargeur() {}

          virtual void Chargement(const CParametresChargement& param) = 0;

          virtual int GetNombreSubmesh() = 0;
          virtual int GetNombreSommetsSubmesh(unsigned int n) = 0;
          virtual DirectX::XMFLOAT3 GetPosition(int noMesh, int NoSommet) = 0;
          virtual DirectX::XMFLOAT2 GetCoordTex(int noMesh, int NoSommet) = 0;
          virtual DirectX::XMFLOAT3 GetNormale(int noMesh, int NoSommet) = 0;
          virtual unsigned int GetIndice(int noMesh, int noPoly, int NoIndice) = 0;
          virtual int  GetNombrePolygonesSubmesh(int noMesh) = 0;

          virtual int  GetNombreMaterial() = 0;
          virtual void GetMaterial(int _i,
               std::string& _NomFichierTexture,
               std::string& _NomMateriau,
               DirectX::XMFLOAT4& _Ambient,
               DirectX::XMFLOAT4& _Diffuse,
               DirectX::XMFLOAT4& _Specular,
               float& _Puissance) = 0;

          virtual int GetMaterialIndex(int i) = 0;

     protected:
          CParametresChargement parametres;


     };


}

#endif // !FILE_LOADER_H