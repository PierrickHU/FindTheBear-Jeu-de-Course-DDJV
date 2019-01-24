#ifndef SCENE_H
#define SCENE_H
#include "ScenePhysique.h"
#include "Formes/CGameObject.h"
#include "Formes/CTerrain.h"
#include "Camera/FrustrumView.h"
#include "Vehicule.h"
#include "Skybox.h"
#include "Item.h"
#include "Zone.h"
#include "RandomGenerateur.h"
#include "GameState.h"
#include "GameManager.h"
#include "Camera/Lumiere.h"
#include "Formes/Chateau.h"
#include "Camera/LumiereManager.h"

#include <array>
#include <vector>


namespace PM3D {
    using CObjet3DPtr = std::unique_ptr<CGameObject>;

    class ScenePhysique;
    class Scene {
        static constexpr size_t NB_ZONES = 4;
        static constexpr size_t MAX_OBJETS = 300;
        //GameManager gameManager;
        LumiereManager lumiereManager;
        std::unique_ptr<CTerrain> terrain;
        std::unique_ptr<Vehicule> vehicule;
        std::unique_ptr<Skybox> skybox;
        std::vector<std::unique_ptr<Item>> items;
        std::vector<CObjet3DPtr> objects;
        int CompteurItemRecup = 0;
        std::array<Zone, NB_ZONES> zones;
        std::unique_ptr<Chateau> chateau;

    public:
        Scene();
        ~Scene() = default;

        void InitPhysX();
        void AddObject(CObjet3DPtr&& objPtr);
        void InitObjets(CDispositifD3D11*);
        void Draw(const FrustrumView& frustrumView, const GameState& state);
        void UpdateItems(float _deltaTime);
        void UpdateNbItems();
        void Anime(float);
        void Clear();
        RandomGenerateur* Rand;
        CTerrain* getTerrain();
        ScenePhysique scenePhysique;
        Vehicule* getVehicule();
        Chateau& getChateau();
        LumiereManager& getLumiereManager();
        std::vector<std::unique_ptr<Item>>& getItems() { return std::move(items); }
    private:
        void InitZones();
        void InitItems(CDispositifD3D11 * _dispositif);
        void InitChateau(ScenePhysique& scenePhysique);
        void DrawItems();
        void DrawShadowItems();

        // Pour les ombres
        CDispositifD3D11* pDispositif;		// On prend en note le dispositif

        const int SHADOWMAP_DIM = 2048;
        const float ANGLE_LUMIERE = DirectX::XM_PIDIV4 * 2.f / 3.f;
        void InitLumieres();
        void InitMatricesShadowMap();
        void UpdateLumiere();
         
        DirectX::XMVECTOR oldPosistionPlayer;

        XMMATRIX matViewLight;
        XMMATRIX matProjLight;
        XMMATRIX matViewProjLight;

     
        ID3D11Texture2D* pDepthTexture; // texture de profondeur 
        ID3D11DepthStencilView* pDepthStencilView;
        ID3D11ShaderResourceView* pDepthShaderResourceView;

        void InitBufferShadowMapping();
    };

} // namespace PM3D


#endif