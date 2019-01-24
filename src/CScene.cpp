#include "stdafx.h"
#include "CScene.h"
#include "Formes/Orientation.h"
#include "toolsMath.h"
#include "CAssimpLoader.h"
#include "util.h"
#include "Resource.h"
#include "Skybox.h"
#include "ScenePhysique.h"
#include "Camera/LumiereManager.h"
#include "Formes/CSimpleGameObject.h"
#include "MoteurWindows.h"
#include "RessourcesManager.h"

#include <memory>
#include <algorithm>

using namespace DirectX;

const int NOMBREITEMS = 60;

namespace PM3D
{
   Scene::Scene()
   {
      objects.reserve(MAX_OBJETS);
   }

   void Scene::InitPhysX() {
      scenePhysique.Init();
   }

   void Scene::InitObjets(CDispositifD3D11* _dispositif)
   {
      // Récupérer le dispositif
      pDispositif = _dispositif;

      // Le terrain
      terrain = std::make_unique<CTerrain>(DirectX::XMFLOAT3{ 0.f, 0.f, 0.f }, _dispositif, scenePhysique);

      // Le chateau
      InitChateau(scenePhysique);

      // Le vehicule
      Orientation orientationVehicule{ DirectX::XMVECTOR{
          terrain->getMiddlePosition().x,
          1.f + terrain->getHauteur(terrain->getMiddlePosition().x, terrain->getMiddlePosition().z),
          terrain->getMiddlePosition().z } };
      vehicule = std::make_unique<Vehicule>(orientationVehicule, 1.f, 1.f, 1.f, _dispositif, scenePhysique);

      // Les zones
      InitZones();

      InitItems(_dispositif);

      // Les lumieres
      InitLumieres();


      // La skybox
      skybox = std::make_unique<Skybox>(orientationVehicule, _dispositif);
   }

   void Scene::InitZones() {
      auto zone2Pos = XMVectorDivide(XMLoadFloat3(&terrain->getSize()), { 4.f, 1.f, 4.f });
      auto zoneSize = XMVectorDivide(XMLoadFloat3(&terrain->getSize()), { 2.f, 1.f, 2.f });
      zones[0] = Zone{ XMVectorMultiply(zone2Pos, { 1.f, 0.f, 3.f }), zoneSize };
      zones[1] = Zone{ XMVectorMultiply(zone2Pos, { 3.f, 0.f, 3.f }), zoneSize };
      zones[2] = Zone{ XMVectorMultiply(zone2Pos, { 1.f, 0.f, 1.f }), zoneSize };
      zones[3] = Zone{ XMVectorMultiply(zone2Pos, { 3.f, 0.f, 1.f }), zoneSize };
   }

   void Scene::InitItems(CDispositifD3D11* _dispositif) {
      for (int i = 0; i != NOMBREITEMS; i++)
      {
          Orientation orientationItem = CMoteurWindows::GetInstance().getGameManager().getRandomOrientationArbre(30.f, 20.f, 20.f);

          float x = RandomGenerateur::get().random(0.0f, 30.f);
          float z = RandomGenerateur::get().random(0.0f, 30.f);

          orientationItem.setScale({ 30.f + x, 40.f, 30.f + z});


       // AddObject(std::make_unique<CSimpleGameObject>(orientationItem, ModelType::TREE_1, _dispositif, scenePhysique));
              //      CSimpleGameObject(Orientation& orientation, ModelType type, CDispositifD3D11* _pDispositif, ScenePhysique& scenePhysique);
          AddObject(std::make_unique<CSimpleGameObject>(orientationItem, static_cast<ModelType>(ModelType::TREE_1 + RandomGenerateur::get().random(0, 2)), _dispositif, scenePhysique));
      }
   }

   void Scene::InitChateau(ScenePhysique& scenePhysique) {
      chateau = std::make_unique<Chateau>(pDispositif, scenePhysique); // Et voilà ! :D
   }

   void Scene::AddObject(CObjet3DPtr&& objPtr) {
      objects.push_back(std::move(objPtr));
      auto vecMiddleToObj = XMVectorSubtract(objects.back()->orientation.getPosition(), XMLoadFloat3(&terrain->getMiddlePosition()));
      zones[(XMVectorGetX(vecMiddleToObj) > 0 ? 1 : 0) + (XMVectorGetZ(vecMiddleToObj) > 0 ? 0 : 2)].indexContainer.emplace_back(objects.size() - 1);
   }

   void Scene::DrawItems() {
      for (auto& item : items)
      {
         item->Draw();
      }
   }

   void Scene::DrawShadowItems() {
      for (auto& item : items)
      {
         item->DrawShadow(matViewProjLight);
      }
   }
   //void Scene::AddMainMenu(CObjet3DPtr&& objPtr) {
   //   mainMenuElements.push_back(std::move(objPtr));
   //}

   //void Scene::AddOptionMenu(CObjet3DPtr&& objPtr) {
   //   optionsMenuElements.push_back(std::move(objPtr));
   //}

   //void Scene::AddEndMenu(CObjet3DPtr&& objPtr) {
   //    endMenuElements.push_back(std::move(objPtr));
   //}

   void Scene::Draw(const FrustrumView& _frustrumView, const GameState& state)
   {
      IndexContainer res;
      switch (state)
      {
      case MAIN_MENU:
         //for (auto& obj : mainMenuElements)
         //   obj->Draw();
         break;

      case GAME:
      {

          for (auto i = 0; i < NB_ZONES; ++i)
          {
              if (_frustrumView.IsInRectangle(zones[i]))
                  res.insert(res.end(), zones[i].indexContainer.begin(), zones[i].indexContainer.end());
          }

         // ******* OMBRES CREATION SHADOW MAP *******
         UpdateLumiere();
         auto pImmediateContext = pDispositif->GetImmediateContext();
         // Utiliser la surface de la texture comme surface de rendu
         pImmediateContext->OMSetRenderTargets(0, nullptr, pDepthStencilView);
         // Effacer le shadow map
         pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
         // Modifier les dimension du viewport
         pDispositif->SetViewPortDimension(SHADOWMAP_DIM, SHADOWMAP_DIM);

         //Tunnel
         auto posVehicule = vehicule->orientation;
         bool isInTunnel = chateau->isInTunnel(posVehicule.getPositionX(), posVehicule.getPositionZ());
         if (!isInTunnel) {
            // on dessine dans le depthstencilview
            terrain->DrawShadow(matViewProjLight);
            DrawShadowItems();
            chateau->DrawShadow(matViewProjLight);
            vehicule->DrawShadow(matViewProjLight);

            for (auto& obj : objects)
                obj->DrawShadow(matViewProjLight);
         }

         // On remet les render target original
         ID3D11RenderTargetView* tabRTV[1];
         tabRTV[0] = pDispositif->GetRenderTargetView();
         pImmediateContext->OMSetRenderTargets(1, tabRTV, pDispositif->GetDepthStencilView());

         // Dimension du viewport - defaut
         pDispositif->ResetViewPortDimension();

         // CRITIQUE, on remet le rasterizer state par defaut
         pDispositif->SetNormalRSState();

         skybox->Draw();
         terrain->Draw(pDepthShaderResourceView);
         chateau->Draw();
         DrawItems();


         for (auto& obj : objects)
             obj->Draw(pDepthShaderResourceView);


         //This should always be the last one to be drawn because of the alpha blending with the billboards
         vehicule->Draw(pDepthShaderResourceView);
         break;
      }

      case OPTIONS_MENU:

         //terrain->Draw(pDepthShaderResourceView);
         //chateau->Draw();
         //for (auto& obj : optionsMenuElements)
         //   obj->Draw();
         //skybox->Draw();

         skybox->Draw();
         terrain->Draw(pDepthShaderResourceView);
         chateau->Draw();
         DrawItems();
         for (auto& obj : objects)
             obj->Draw(pDepthShaderResourceView);
         vehicule->Draw(pDepthShaderResourceView);

         break;

      case END_MENU:
      {
         skybox->Draw();
         terrain->Draw(pDepthShaderResourceView);
         chateau->Draw();
         DrawItems();
         for (auto& obj : objects)
             obj->Draw(pDepthShaderResourceView);
         vehicule->Draw(pDepthShaderResourceView);

         //for (auto& obj : endMenuElements)
         //    obj->Draw();

         break;
      }
      default:
         //throw HustonWeHaveAProblem{};
         break;
      }
   }


   void Scene::UpdateItems(float _deltaTime) {

      for (int j = 0; j != items.size(); j++)
      {
         items[j]->Update(_deltaTime, *terrain, scenePhysique);
      }
   }

   void Scene::UpdateNbItems() {
      if (!scenePhysique.deleteTrigger.empty())
      {
         for (int indiceTrigger : scenePhysique.deleteTrigger)
         {
            auto it = std::find_if(items.begin(), items.end(), [&indiceTrigger](const std::unique_ptr<Item> &item) {
               return item->indexTrigger == indiceTrigger;

            });

            (*it)->OnTrigger();
            (*it)->physic->rigidbody->release();
            (*it).swap(items.back());
            items.pop_back();
            CompteurItemRecup++;

            //items[indiceTrigger]->physic->rigidbody->release();

         }
         scenePhysique.deleteTrigger.clear();
      }
   }

   void Scene::Anime(float _deltaTime)
   {
      skybox->Update(_deltaTime);

      vehicule->Update(_deltaTime, *terrain, scenePhysique);
      UpdateItems(_deltaTime);
      UpdateNbItems();

      scenePhysique.gScene->simulate(_deltaTime);
      scenePhysique.gScene->fetchResults(true);
   }

   void Scene::Clear()
   {
      objects.clear();
   }

   CTerrain* Scene::getTerrain()
   {
      return terrain.get();
   }

   Vehicule* Scene::getVehicule()
   {
      return vehicule.get();
   }

   LumiereManager& Scene::getLumiereManager() {
      return lumiereManager;
   }

   Chateau& Scene::getChateau() {
      return *chateau;
   }

   void Scene::InitBufferShadowMapping() {

      auto pD3DDevice = pDispositif->GetD3DDevice();

      // ***********SHADOW MAP************

      // TEXTURE SHADOW MAP
      D3D11_TEXTURE2D_DESC depthTextureDesc;
      ZeroMemory(&depthTextureDesc, sizeof(depthTextureDesc));
      depthTextureDesc.Width = SHADOWMAP_DIM;
      depthTextureDesc.Height = SHADOWMAP_DIM;
      depthTextureDesc.MipLevels = 1;
      depthTextureDesc.ArraySize = 1;
      depthTextureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
      depthTextureDesc.SampleDesc.Count = 1;
      depthTextureDesc.SampleDesc.Quality = 0;
      depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
      depthTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
      depthTextureDesc.CPUAccessFlags = 0;
      depthTextureDesc.MiscFlags = 0;

      DXEssayer(pD3DDevice->CreateTexture2D(&depthTextureDesc, nullptr, &pDepthTexture), DXE_ERREURCREATIONTEXTURE);

      // Creation de la vue du tampon de profondeur (ou de stencil)
      D3D11_DEPTH_STENCIL_VIEW_DESC descDSView;
      ZeroMemory(&descDSView, sizeof(descDSView));
      descDSView.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
      descDSView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
      descDSView.Texture2D.MipSlice = 0;
      DXEssayer(
         pD3DDevice->CreateDepthStencilView(pDepthTexture, &descDSView, &pDepthStencilView),
         DXE_ERREURCREATIONDEPTHSTENCILTARGET);

      // Creation d'une shader resource view pour lire le tampon de profondeur dans le shader.
      D3D11_SHADER_RESOURCE_VIEW_DESC sr_desc;
      ZeroMemory(&sr_desc, sizeof(sr_desc));
      sr_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
      sr_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      sr_desc.Texture2D.MostDetailedMip = 0;
      sr_desc.Texture2D.MipLevels = 1;

      DXEssayer(
         pD3DDevice->CreateShaderResourceView(pDepthTexture, &sr_desc, &pDepthShaderResourceView),
         DXE_ERREURCREATIONSRV);
   }

   void Scene::InitMatricesShadowMap()
   {
      //POSITIONNEMENT
      int distanceVehicule = 50;
      // Matrice de la vision vu par la lumiere
      auto playerOrientation = vehicule->orientation;
      auto playerPosition = playerOrientation.getPosition();
      oldPosistionPlayer = playerPosition;
      auto playerDirection = playerOrientation.getDirection();

      DirectX::XMVECTOR axeRotation = produitVectoriel(playerDirection, playerOrientation.getUp());
      DirectX::XMVECTOR quaternionRot = getQuaternionRotation(axeRotation, ANGLE_LUMIERE);
      DirectX::XMVECTOR vecPosLight = rotateVectorWithQuaternion(-playerDirection, quaternionRot);
      DirectX::XMVECTOR upLight = normaliseVecteur(rotateVectorWithQuaternion(getAxeY(), quaternionRot));
      vecPosLight = normaliseVecteur(vecPosLight) * distanceVehicule;

      auto positionInit = playerPosition + vecPosLight;

      //FRUSTRUM
      const float champDeVision = DirectX::XM_PIDIV4;  // 45 degres
      const float ratioDAspect = 1.0f; 	// 512/512
      const float planRapproche = 2.0f; 	// Pas besoin d'etre trop pres
      const XMVECTOR positionTerrain = terrain->orientation.getPosition();
      const XMFLOAT3 scaleTerrain = terrain->orientation.getScale();
      const XMFLOAT3 dimensionTerrain = terrain->getSize();
      const float diagonaleTerrain = distanceBetweenTwoPoints(positionTerrain, positionTerrain + XMVectorSet(dimensionTerrain.x*scaleTerrain.x, dimensionTerrain.y*scaleTerrain.y, dimensionTerrain.z*scaleTerrain.z, 1.0));
      const float planEloigne = distanceVehicule + min(diagonaleTerrain, 1000.f);

      Orientation orientationInitLumiere{ positionInit,-normaliseVecteur(vecPosLight), upLight };
      FrustrumView frustrumInit = FrustrumView(champDeVision,
         ratioDAspect,
         planRapproche,
         planEloigne, true);

      Lumiere& lumiereShadowMap = lumiereManager.getLumiere(LumiereManager::LightType::SHADOW_MAP);
      lumiereShadowMap = Lumiere(orientationInitLumiere, CouleurLumiere(), frustrumInit);

      matViewLight = DirectX::XMMatrixLookAtLH(
         lumiereShadowMap.orientation.getPosition(),
         lumiereShadowMap.orientation.getPosition() + lumiereShadowMap.orientation.getDirection(),
         upLight);


      //// VERSION LUMIERE POINT
      //matProjLight = DirectX::XMMatrixPerspectiveFovLH(champDeVision,
      //   ratioDAspect,
      //   planRapproche,
      //   planEloigne);
      //// VERSION LUMIERE DIRECTIONNELLE
      //matProjLight = DirectX::XMMatrixOrthographicLH(1000.f,
      //   1000.f,
      //   planRapproche,
      //   planEloigne);
      matProjLight = lumiereShadowMap.getMatProj();

      matViewProjLight = matViewLight * matProjLight;
   }

   void Scene::UpdateLumiere() {

      Lumiere& lumiereShadowMap = lumiereManager.getLumiere(LumiereManager::LightType::SHADOW_MAP);

      int distanceVehiculeMin = 50;
      int distanceVehiculeMax = 200;
      int distanceVehicule = 50;

      //POSITIONNEMENT
      auto playerOrientation = vehicule->orientation;
      auto playerPosition = playerOrientation.getPosition();
      auto playerDirection = playerOrientation.getDirection();

      //on eloigne la lumiere si elle est en face du vehicule
      float produitScalaire = DirectX::XMVector3Dot(normaliseVecteur(playerPosition - lumiereShadowMap.orientation.getPosition()), normaliseVecteur(lumiereShadowMap.orientation.getDirection())).vector4_f32[0];
      if (produitScalaire < 0.f) {
         distanceVehicule = distanceVehiculeMin + (-produitScalaire) * (distanceVehiculeMax - distanceVehiculeMin);
      }
      lumiereShadowMap.orientation.translate(playerPosition - oldPosistionPlayer);
      const float distanceLumiereVehicule = distanceBetweenTwoPoints(playerPosition, lumiereShadowMap.orientation.getPosition());
      lumiereShadowMap.orientation.translate((distanceVehicule - distanceLumiereVehicule)*normaliseVecteur(lumiereShadowMap.orientation.getPosition() - playerPosition));

      oldPosistionPlayer = playerPosition;

      //FRUSTRUM
      const float champDeVision = DirectX::XM_PIDIV4;  // 45 degres
      const float ratioDAspect = 1.0f; 	// 512/512
      const float planRapproche = 2.0f; 	// Pas besoin d'etre trop pres
      const XMVECTOR positionTerrain = terrain->orientation.getPosition();
      const XMFLOAT3 scaleTerrain = terrain->orientation.getScale();
      const XMFLOAT3 dimensionTerrain = terrain->getSize();
      const float diagonaleTerrain = distanceBetweenTwoPoints(positionTerrain, positionTerrain + XMVectorSet(dimensionTerrain.x*scaleTerrain.x, dimensionTerrain.y*scaleTerrain.y, dimensionTerrain.z*scaleTerrain.z, 1.0));
      const float planEloigne = distanceVehicule + max(produitScalaire, 0.f) * min(diagonaleTerrain, 2000.f);

      matViewLight = DirectX::XMMatrixLookAtLH(
         lumiereShadowMap.orientation.getPosition(),
         lumiereShadowMap.orientation.getPosition() + lumiereShadowMap.orientation.getDirection(),
         lumiereShadowMap.orientation.getUp());

      FrustrumView frustrumInit = FrustrumView(champDeVision,
         ratioDAspect,
         planRapproche,
         planEloigne, true);

      lumiereShadowMap = Lumiere(lumiereShadowMap.orientation, CouleurLumiere(), frustrumInit);
      matProjLight = lumiereShadowMap.getMatProj();

      matViewProjLight = matViewLight * matProjLight;

      // LUMIERE SPOT POUR LE TUNNEL
      DirectX::XMVECTOR positionInit{
           playerOrientation.getPositionX()
        , playerOrientation.getPositionY()
        , playerOrientation.getPositionZ() };

      XMVECTOR dirLight = playerOrientation.getDirection();
      XMVECTOR upLight = playerOrientation.getUp();
      Orientation orientationInitLumiere = Orientation(positionInit, normaliseVecteur(dirLight), upLight);

      XMVECTOR ambient = 1.f/2.f * lumiereManager.getLumiere(LumiereManager::LightType::MINIPHONG_REVE).getCouleur().ambient;
      XMVECTOR diffus = lumiereManager.getLumiere(LumiereManager::LightType::MINIPHONG_REVE).getCouleur().diffus;
      XMVECTOR speculaire = lumiereManager.getLumiere(LumiereManager::LightType::MINIPHONG_REVE).getCouleur().speculaire;

      lumiereManager.getLumiere(LumiereManager::LightType::TUNNEL_REVE) = Lumiere(orientationInitLumiere, CouleurLumiere(ambient, diffus, speculaire));

      ambient = 1.f/2.f * lumiereManager.getLumiere(LumiereManager::LightType::MINIPHONG_CAUCHEMARD).getCouleur().ambient;
      diffus = 2.f*lumiereManager.getLumiere(LumiereManager::LightType::MINIPHONG_CAUCHEMARD).getCouleur().diffus;
      speculaire = 2.f*lumiereManager.getLumiere(LumiereManager::LightType::MINIPHONG_CAUCHEMARD).getCouleur().speculaire;

      lumiereManager.getLumiere(LumiereManager::LightType::TUNNEL_CAUCHEMARD) = Lumiere(orientationInitLumiere, CouleurLumiere(ambient, diffus, speculaire));
   }

   void Scene::InitLumieres() {

      XMVECTOR posLight;
      XMVECTOR dirLight;
      XMVECTOR upLight;
      Orientation orientationInitLumiere;

      XMVECTOR ambient;
      XMVECTOR diffus;
      XMVECTOR speculaire;

      // LUMIERE DIRECTIONELLE POUR LE REVE

      // Matrice de la vision vu par la lumiere
      Orientation playerOrientation = vehicule->orientation;
      auto playerPosition = playerOrientation.getPosition();
      auto playerDirection = playerOrientation.getDirection();

      DirectX::XMVECTOR axeRotation = produitVectoriel(playerDirection, playerOrientation.getUp());
      DirectX::XMVECTOR quaternionRot = getQuaternionRotation(axeRotation, ANGLE_LUMIERE);
      DirectX::XMVECTOR vecPosLight = rotateVectorWithQuaternion(-playerDirection, quaternionRot);
      upLight = normaliseVecteur(rotateVectorWithQuaternion(getAxeY(), quaternionRot));
      vecPosLight = normaliseVecteur(vecPosLight);

      posLight = {};
      dirLight = -vecPosLight;
      upLight = upLight;
      orientationInitLumiere = Orientation(posLight, normaliseVecteur(dirLight), upLight);

      ambient = DirectX::XMVectorSet(0.7f, 0.7f, 0.7f, 1.0f);
      diffus = DirectX::XMVectorSet(0.7f, 0.7f, 0.7f, 1.0f);
      speculaire = DirectX::XMVectorSet(0.7f, 0.7f, 0.7f, 1.0f);

      lumiereManager.getLumiere(LumiereManager::LightType::MINIPHONG_REVE) = Lumiere(orientationInitLumiere, CouleurLumiere(ambient, diffus, speculaire));

      // LUMIERE DIRECTIONELLE POUR LE CAUCHEMARD
      posLight = {};
      dirLight = -vecPosLight;
      upLight = upLight;
      orientationInitLumiere = Orientation(posLight, normaliseVecteur(dirLight), upLight);

      ambient = DirectX::XMVectorSet(132.f / 255.f, 46.f / 255.f, 27.f / 255.f, 1.f);
      ambient = DirectX::XMVectorSet(49.f / 255.f, 140.f / 255.f, 231.f / 255.f, 1.f);
      diffus = DirectX::XMVectorSet(165.f / 255.f, 38.f / 255.f, 10.f / 255.f, 1.f);
      speculaire = 2.f*DirectX::XMVectorSet(240.f / 255.f, 195.f / 255.f, 0.f / 255.f, 1.0f);

      lumiereManager.getLumiere(LumiereManager::LightType::MINIPHONG_CAUCHEMARD) = Lumiere(orientationInitLumiere, CouleurLumiere(ambient, diffus, speculaire));

      // LUMIERE SPOT POUR LE TUNNEL REVE
      DirectX::XMVECTOR positionInit{
           playerOrientation.getPositionX()
        , playerOrientation.getPositionY()
        , playerOrientation.getPositionZ() };

      dirLight = playerOrientation.getDirection();
      upLight = playerOrientation.getUp();
      orientationInitLumiere = Orientation(positionInit, normaliseVecteur(dirLight), upLight);

      ambient = 1.f / 3.f*lumiereManager.getLumiere(LumiereManager::LightType::MINIPHONG_REVE).getCouleur().ambient;
      diffus = lumiereManager.getLumiere(LumiereManager::LightType::MINIPHONG_REVE).getCouleur().diffus;
      speculaire = lumiereManager.getLumiere(LumiereManager::LightType::MINIPHONG_REVE).getCouleur().speculaire;

      lumiereManager.getLumiere(LumiereManager::LightType::TUNNEL_REVE) = Lumiere(orientationInitLumiere, CouleurLumiere(ambient, diffus, speculaire));

      // LUMIERE SPOT POUR LE TUNNEL CAUCHEMARD

      dirLight = playerOrientation.getDirection();
      upLight = playerOrientation.getUp();
      orientationInitLumiere = Orientation(positionInit, normaliseVecteur(dirLight), upLight);

      ambient = 1.f / 3.f*lumiereManager.getLumiere(LumiereManager::LightType::MINIPHONG_CAUCHEMARD).getCouleur().ambient;
      diffus = lumiereManager.getLumiere(LumiereManager::LightType::MINIPHONG_CAUCHEMARD).getCouleur().diffus;
      speculaire = lumiereManager.getLumiere(LumiereManager::LightType::MINIPHONG_CAUCHEMARD).getCouleur().speculaire;

      lumiereManager.getLumiere(LumiereManager::LightType::TUNNEL_CAUCHEMARD) = Lumiere(orientationInitLumiere, CouleurLumiere(ambient, diffus, speculaire));

      InitBufferShadowMapping();
      InitMatricesShadowMap();
   }
}
