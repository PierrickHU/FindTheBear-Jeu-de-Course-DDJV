#include "stdafx.h"
#include "Formes/Orientation.h"
#include "Resource.h"
#include "Skybox.h"
#include "Moteur.h"
#include "MoteurWindows.h"


namespace PM3D {

   Skybox::Skybox(const Orientation& _orientation, CDispositifD3D11* _pDispositif)
      :CGameObject(_orientation)
   {
      Init(_pDispositif);
   }

   void Skybox::Init(CDispositifD3D11 * _pDispositif)
   {
      orientation.setScale(DirectX::XMFLOAT3{ 2500.0f, 2500.0f, 2500.0f });
      skyboxMesh = CMoteurWindows::GetInstance().GetRessourcesManager().GetModelMesh(ModelType::SKYBOX);
      skyboxMesh->shaderFilename = L"../ressources/shaders/Skybox.fx";
      skyboxMesh->InitSkybox(_pDispositif);
      //skyboxMesh->InitEffetSkybox();
   }

   void Skybox::Update(const float tempsEcoule)
   {
      calculMatriceWorld();
   }

}

