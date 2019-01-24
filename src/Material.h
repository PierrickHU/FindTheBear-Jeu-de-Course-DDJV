#ifndef MATERIAL_H
#define MATERIAL_H

#include "stdafx.h"
#include "string"
#include "Texture.h"

namespace PM3D
{
    class Material
    {
    private:
       static const float AMBIENT_DEFAUT;
       static const float DIFFUSE_DEFAUT;
       static const float SPECULAR_DEFAUT;
       static const float PUISSANCE_DEFAUT;
    public:
        std::string NomFichierTexture;
        std::string NomMateriau;
        CTexture* texture;

        DirectX::XMFLOAT4 Ambient;
        DirectX::XMFLOAT4 Diffuse;
        DirectX::XMFLOAT4 Specular;
        float Puissance;
        bool transparent;

        Material();
        Material(CTexture*);

        ~Material() = default;
    };
}

#endif // !MESH_COMPONENT_H


