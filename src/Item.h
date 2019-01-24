#ifndef ITEM_H
#define ITEM_H

#include "ScenePhysique.h"
#include "Formes/Orientation.h"
#include "Component/PhysicTriggerComponent.h"
#include "dispositifD3D11.h"
#include "GameState.h"

#include <memory>
using DirectX::XMMATRIX;

namespace PM3D
{
	class CTerrain;
	class ScenePhysique;
	class MeshComponent;
	class Item : public CGameObject {
        static int currentIndex; // Commence a 0 et est decremente 
	public:
		MeshComponent* mesh;
		std::unique_ptr<PhysicTriggerComponent> physic;
		int indexTrigger;
		Item(const Orientation& _orientation, const float _dx, const float _dy, const float _dz, CDispositifD3D11* _pDispositif, ScenePhysique& scenePhysique, ModelType model);
		virtual ~Item() = default;

		virtual void Draw(ID3D11ShaderResourceView* pDepthShaderResourceView = nullptr) override;
		virtual void Update(const float tempsEcoule, const CTerrain& terrain, ScenePhysique& sp);
        void DrawShadow(const XMMATRIX matViewProjLight);

        virtual bool isCatchable() const noexcept; // Permet de savoir si on peut attraper un item ou non

        // La fonction declanche lorsque l'on ramasse un item !
        virtual void OnTrigger() {}; // Par default on ne fait rien, ce seront les enfants qui en heriteront qui specifieront l'effet escompte !
	};

} // namespace PM3D


#endif // !ITEM_H
