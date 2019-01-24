#include "stdafx.h"
#include "LumiereManager.h"

namespace PM3D {

   LumiereManager::LumiereManager() {
         lumieres.emplace(LightType::SHADOW_MAP, Lumiere{});
         lumieres.emplace(LightType::MINIPHONG_REVE, Lumiere{});
         lumieres.emplace(LightType::MINIPHONG_CAUCHEMARD, Lumiere{});
         lumieres.emplace(LightType::TUNNEL_REVE, Lumiere{});
         lumieres.emplace(LightType::TUNNEL_CAUCHEMARD, Lumiere{});
   }

   Lumiere& LumiereManager::getLumiere(LightType type) {
      return lumieres[type];
   }
   void LumiereManager::setLumiere(LightType type, Lumiere lumiere) {
      lumieres.at(type) = lumiere;
   }

}
