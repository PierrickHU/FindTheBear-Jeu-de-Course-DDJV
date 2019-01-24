#include "stdafx.h"
#include "ShaderManager.h"
#include "util.h"
#include "Resource.h"

#include <algorithm>
#include <memory>

namespace PM3D
{
    const std::vector<ShaderManager::fileName_t> ShaderManager::shadersToLoad = {
        L"../ressources/shaders/MiniPhongSM.fx",
        L"../ressources/shaders/MiniPhong.fx",
        L"../ressources/shaders/TerrainSM.fx",
        L"../ressources/shaders/Sprite1.fx",
        L"../ressources/shaders/PostEffect.fx",
		  L"../ressources/shaders/TerrainNM.fx",
        L"../ressources/shaders/Skybox.fx",
		  L"../ressources/shaders/ChateauNM.fx"
    };

    void ShaderManager::LoadAll(CDispositifD3D11* pDispositif) {
        for (ShaderManager::fileName_t shaderFilename : shadersToLoad) {
            loadedEffects.emplace(shaderFilename, std::move(std::unique_ptr<ShaderManager::effect_type>(compileEffect(shaderFilename, pDispositif->GetD3DDevice()))));
        }
    }
    ShaderManager::pointeur_effect_type ShaderManager::GetEffet(const ShaderManager::fileName_t& shaderFilename) const {
        auto iter = loadedEffects.find(shaderFilename);
        assert(iter != loadedEffects.end(), "Add your shader to shadersToLoad, we load them all at once at the beginning");
        return iter->second.get();
    }

    ShaderManager::pointeur_effect_type ShaderManager::compileEffect(const ShaderManager::fileName_t& shaderFilename, ID3D11Device* pD3DDevice) {
        ShaderManager::pointeur_effect_type pEffet;
        // Pour l'effet
        ID3DBlob* pFXBlob = nullptr;
        DXEssayer(D3DCompileFromFile(shaderFilename.c_str(), 0, 0, 0,
            "fx_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
            &pFXBlob, nullptr),
            DXE_ERREURCREATION_FX);

        D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(), pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet);

        pFXBlob->Release();
        return std::move(pEffet);
    }
} // namespace PM3D
