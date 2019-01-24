#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "d3dx11effect.h"
#include "Formes/ShadersParams.h"
#include "dispositifD3D11.h"

#include <vector>
#include <map>

namespace PM3D
{
    class ShaderManager
    {
        using fileName_t = std::wstring;
        using effect_type = ID3DX11Effect;
        using pointeur_effect_type = ID3DX11Effect*;

        static const std::vector<fileName_t> shadersToLoad;
    public:
        void LoadAll(CDispositifD3D11*);
        pointeur_effect_type GetEffet(const fileName_t&) const;
    private:
        std::map<fileName_t, std::unique_ptr<effect_type>> loadedEffects;
        pointeur_effect_type compileEffect(const fileName_t&, ID3D11Device*);
    };

} // namespace PM3D
#endif // !SHADER_MANAGER_H