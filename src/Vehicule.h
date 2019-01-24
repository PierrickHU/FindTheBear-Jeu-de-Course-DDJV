#ifndef VEHICULE_H
#define VEHICULE_H

#include "ScenePhysique.h"
#include "Formes/Orientation.h"
#include "Component/MeshComponent.h"
#include "Component/PhysicPlayerComponent.h"
#include "Component/InputPlayerComponent.h"
#include "Formes/Billboard.h"

#include <memory>
#include <vector>
#include <chrono>

namespace PM3D
{
    class CTerrain;
    class ScenePhysique;
   class Vehicule : public CGameObject
   {
   public:
      MeshComponent* mesh;
      std::unique_ptr<PhysicPlayerComponent> physic;
      std::unique_ptr<InputPlayerComponent> input;
      std::vector<std::unique_ptr<Billboard>> stars;
      std::chrono::milliseconds dureeDisableMove{ 0 };
      std::chrono::time_point<std::chrono::steady_clock> lastTimeDisableMove = std::chrono::high_resolution_clock::now();

      Vehicule(const Orientation& _orientation, const float _dx, const float _dy, const float _dz, CDispositifD3D11* _pDispositif, ScenePhysique& scenePhysique);
      virtual ~Vehicule() = default;

      bool canMove() const noexcept; // Permet de savoir si le véhicule peut bouger ou non
      void disableMoveFor(std::chrono::milliseconds duree); // Permet de désactiver le mouvement du joueur pendant une certain période

      virtual void Draw(ID3D11ShaderResourceView* pDepthShaderResourceView = nullptr) override;
      virtual void DrawShadow(const XMMATRIX matViewProjLight);
      virtual void Update(const float tempsEcoule, const CTerrain& terrain, ScenePhysique& sp);
   };

} // namespace PM3D

#endif // !VEHICULE_H
