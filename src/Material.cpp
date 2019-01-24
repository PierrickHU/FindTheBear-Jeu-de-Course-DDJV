#include "stdafx.h"
#include "Material.h"

namespace PM3D
{
   const float Material::AMBIENT_DEFAUT = 0.8f;
   const float Material::DIFFUSE_DEFAUT = 0.9f;
   const float Material::SPECULAR_DEFAUT = 0.1f;
   const float Material::PUISSANCE_DEFAUT = 1.f;

   Material::Material() : Material{ nullptr } {}
   Material::Material(CTexture* texture)
      : texture{ texture }
      , Ambient{ AMBIENT_DEFAUT, AMBIENT_DEFAUT, AMBIENT_DEFAUT, 1.f }
      , Diffuse{ DIFFUSE_DEFAUT, DIFFUSE_DEFAUT, DIFFUSE_DEFAUT, 1.f }
      , Specular{ SPECULAR_DEFAUT, SPECULAR_DEFAUT, SPECULAR_DEFAUT, 1.f }
      , transparent{ false }
      , Puissance{ PUISSANCE_DEFAUT }
      , NomFichierTexture{ "" }
   {}
}