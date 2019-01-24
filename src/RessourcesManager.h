#ifndef GESTIONNAIRE_TEXTURES_H
#define GESTIONNAIRE_TEXTURES_H

#include "texture.h"
#include "Component/MeshComponent.h"
#include "GameState.h"

namespace PM3D
{

class RessourcesManager
{
    struct ModelInfo
    {
        ModelType type;
        CParametresChargement param;

        ModelInfo(const ModelType type, const CParametresChargement& param)
            :type{ type }, param{ param }
        {}
        ~ModelInfo() = default;
    };

    struct Model
    {
        ModelType type;
        MeshComponent* mesh;

        Model(const ModelType type, MeshComponent* mesh)
            :type{ type }, mesh{ mesh }
        {}
        ~Model() = default;
    };

    static const std::vector<std::wstring> TexturesToLoad;
    static const std::vector<ModelInfo> ModelsToLoad;
public:
    void LoadAll(CDispositifD3D11*);
	CTexture* const GetTexture(const std::wstring&);
	MeshComponent* const GetModelMesh(const ModelType);
private:
    std::vector<Model> loadedModels;
	std::vector<std::unique_ptr<CTexture>> loadedTextures;
};

} // namespace PM3D
#endif // !GESTIONNAIRE_TEXTURES_H