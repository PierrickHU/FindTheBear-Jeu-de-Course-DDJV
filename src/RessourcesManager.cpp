#include "stdafx.h"
#include "dispositifD3D11.h"
#include "RessourcesManager.h"

#include <algorithm>

namespace PM3D
{
    const std::vector<std::wstring> RessourcesManager::TexturesToLoad = {
        L"../ressources/textures/stone.dds",
        L"../ressources/textures/grass.dds",
        L"../ressources/textures/green_grass.dds",
        L"../ressources/assets/cube/bear.dds",
        L"../ressources/textures/brick_pavement.dds",
        L"../ressources/textures/brick.dds",
        L"../ressources/textures/MainMenu.dds",
		  L"../ressources/textures/stoneNM.dds",
		  L"../ressources/textures/grassNM.dds",
		  L"../ressources/textures/brickNM.dds",
          L"../ressources/textures/Star.dds",
          L"../ressources/assets/skybox/skybox.dds",
          L"../ressources/assets/mysterybox/rewards.dds"
    };

    const std::vector<RessourcesManager::ModelInfo> RessourcesManager::ModelsToLoad = {
        { CLOUD, { std::string{"../ressources/assets/cloud/"}, "cloud.obj" } },
        { CUBE_TRI, { std::string{"../ressources/assets/cube/"}, "cubeDos.obj" } },
      { TREE_1,{ std::string{ "../ressources/assets/tree/" }, "Lowpoly_tree_sample.obj" } },
      { TREE_2,{ std::string{ "../ressources/assets/tree/" }, "Lowpoly_tree_sample2.obj" } },
      { TREE_3,{ std::string{ "../ressources/assets/tree/" }, "Lowpoly_tree_sample3.obj" } },
        { BEAR, { std::string{"../ressources/assets/bear/choupibear/"}, "choupibear.obj" }},
        { SANDWATCH, { std::string{"../ressources/assets/sandwatch/sandwatch1/"}, "Sand_Watch_tri_back.obj" }},
        { SKYBOX,{ std::string{ "../ressources/assets/skybox/" }, "skybox.obj" } },
    { MYSTERYBOX,{ std::string{ "../ressources/assets/mysterybox/" }, "mysterybox.obj" } }
    };

    void RessourcesManager::LoadAll(CDispositifD3D11* _pDispositif)
    {
        for (const auto& textureName : TexturesToLoad)
            loadedTextures.push_back(std::make_unique<CTexture>(textureName, _pDispositif));

        CChargeurAssimp chargeur;
        for (auto& modelInfo : ModelsToLoad)
        {
            chargeur.Chargement(modelInfo.param);
            loadedModels.emplace_back(modelInfo.type, new MeshComponent(chargeur));
        }
    }

    CTexture* const RessourcesManager::GetTexture(const std::wstring& filename)
    {
        auto iter = std::find_if(loadedTextures.begin(), loadedTextures.end(), [&filename](std::unique_ptr<CTexture>& t) {
            return t->GetFilename() == filename;
        });

        assert(iter != loadedTextures.end(), "Add your texture to TexturesToInit, we load them all at once at the beginning");
        return iter->get();
    }

    MeshComponent* const RessourcesManager::GetModelMesh(const ModelType type)
    {
        auto iter = std::find_if(loadedModels.begin(), loadedModels.end(), [&type](const Model& m) {
            return m.type == type;
        });

        assert(iter != loadedModels.end(), "Add your model to ModelsToInit, we load them all at once at the beginning");
        return iter->mesh;
    }
} // namespace PM3D
