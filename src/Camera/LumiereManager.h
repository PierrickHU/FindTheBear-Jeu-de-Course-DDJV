#ifndef LUMIERE_MANAGER_H
#define LUMIERE_MANAGER_H

#include "stdafx.h"
#include "Lumiere.h"
#include "Singleton.h"

#include <map>

namespace PM3D {

   class LumiereManager {
   public:
      enum LightType { SHADOW_MAP, MINIPHONG_REVE, MINIPHONG_CAUCHEMARD, TUNNEL_REVE, TUNNEL_CAUCHEMARD };
   private:
      std::map<LightType, Lumiere> lumieres;

   public:

      LumiereManager();
      ~LumiereManager() = default;

      Lumiere& getLumiere(LightType);
      void setLumiere(LightType, Lumiere);
   };

} // namespace PM3D
#endif //!LUMIERE_MANAGER_H