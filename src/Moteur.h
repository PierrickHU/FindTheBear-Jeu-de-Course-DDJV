#ifndef MOTEUR_H
#define MOTEUR_H

#include "Singleton.h"
#include "dispositif.h"
#include "Camera/CCamera.h"
#include "DIManipulateur.h"
#include "CScene.h"
#include "RessourcesManager.h"
#include "ShaderManager.h"
#include "Logger.h"
#include "ScenePhysique.h"

#include "AfficheurSprite.h"
#include "AfficheurTexte.h"
#include "PanneauPE.h"
#include "HUD.h"
#include "GameState.h"
#include "MainMenu.h"
#include "EndMenu.h"
#include "OptionsMenu.h"
#include "Component/InputPlayerComponent.h"

#include <string>
#include <DirectXMath.h>
#include <vector>
#include <chrono>

namespace PM3D
{
   const int IMAGESPARSECONDE = 60;
   const float EcartTemps = 1.0f / static_cast<float>(IMAGESPARSECONDE);

   class CTerrain;

   //
   //   TEMPLATE: CMoteur
   //
   //   BUT: Template servant a construire un objet Moteur qui implantera les
   //         aspects "generiques" du moteur de jeu
   //
   //   COMMENTAIRES:
   //
   //        Comme plusieurs de nos objets representeront des elements uniques 
   //        du systeme (ex: le moteur lui-meme, le lien vers 
   //        le dispositif Direct3D), l'utilisation d'un singleton 
   //        nous simplifiera plusieurs aspects.
   //
   template <class T, class TClasseDispositif>
   class CMoteur : public CSingleton<T>
   {
   public:
      std::string logPath;
      Logger logger{};

      std::string getLogPath() { return logPath; }
      Logger& getLogger() { return logger; }
   public:
      static const int TERRAIN_INDEX = 0;

      virtual void Run()
      {
         bool bBoucle = true;

         while (bBoucle)
         {
            // Propre a la plateforme - (Conditions d'arret, interface, messages)
            bBoucle = RunSpecific();

            // appeler la fonction d'animation
            if (bBoucle)
            {
               bBoucle = Animation();
            }
         }
      }

      virtual int Initialisations()
      {
         // Propre a la plateforme
         InitialisationsSpecific();

         // * Initialisation du dispositif de rendu
         pDispositif = CreationDispositifSpecific(CDS_FENETRE);

         // * Initialisation de la scene
         InitScene();

         // * Initialisation des parametres de l'animation et 
         //   preparation de la premiere image
         InitAnimation();

         InitLogger();

         return 0;
      }

      virtual void InitLogger() {
         char buf[256];
         GetCurrentDirectoryA(256, buf);
         logPath = ".\\..\\logs";
         getLogger().Init(std::string(getLogPath()), "testLog.log");
      }

      virtual bool Animation()
      {
         // methode pour lire l'heure et calculer le 
         // temps ecoule
         const int64_t TempsCompteurCourant = GetTimeSpecific();
         const float TempsEcoule = GetTimeIntervalsInSec(TempsCompteurPrecedent, TempsCompteurCourant);

         // Est-il temps de rendre l'image?
         if (TempsEcoule > EcartTemps)
         {
            // Affichage optimise
            pDispositif->Present(); // On enlevera cela plus tard
                                    // On prepare la prochaine image
            AnimeScene(TempsEcoule);

            // On rend l'image sur la surface de travail
            // (tampon d'arriere plan)
            RenderScene();

            // Calcul du temps du prochain affichage
            TempsCompteurPrecedent = TempsCompteurCourant;
         }

         return true;
      }

      const DirectX::XMMATRIX& GetMatViewProj() const { return m_MatViewProj; }

      CCamera& GetCamera() { return camera; }
      Scene& GetScene() { return scene; }
      ScenePhysique& GetScenePhysique() { return scene.scenePhysique; }
      void SetTriggerInScene(vector<int> triggers) {
         scene.scenePhysique.deleteTrigger.clear();
         scene.scenePhysique.deleteTrigger = triggers;

      }
      CDIManipulateur& GetGestionnaireDeSaisie() { return GestionnaireDeSaisie; }
      RessourcesManager& GetRessourcesManager() { return ressourcesManager; }
      ShaderManager& GetShaderManager() { return shaderManager; }
      CTerrain* getTerrain() { return scene.getTerrain(); }
      Vehicule* getVehicule() { return scene.getVehicule(); }
      GameManager& getGameManager() { return gameManager; }

   protected:

      virtual ~CMoteur()
      {
         Cleanup();
      }

      // Specifiques - Doivent etre implantes
      virtual bool RunSpecific() = 0;
      virtual int InitialisationsSpecific() = 0;

      virtual int64_t GetTimeSpecific() const = 0;
      virtual float GetTimeIntervalsInSec(int64_t start, int64_t stop) const = 0;

      virtual TClasseDispositif* CreationDispositifSpecific(const CDS_MODE cdsMode) = 0;
      virtual void BeginRenderSceneSpecific() = 0;
      virtual void EndRenderSceneSpecific() = 0;

      // Autres fonctions
      virtual int InitAnimation()
      {
         TempsSuivant = GetTimeSpecific();
         TempsCompteurPrecedent = TempsSuivant;

         // premiere Image
         RenderScene();

         return true;
      }

      // Fonctions de rendu et de presentation de la scene
      virtual bool RenderScene() = 0;

      virtual void Cleanup()
      {
         // detruire les objets
         scene.Clear();

         // Detruire le dispositif
         if (pDispositif)
         {
            delete pDispositif;
            pDispositif = nullptr;
         }
      }

      virtual int InitScene()
      {
         ressourcesManager.LoadAll(pDispositif);
         shaderManager.LoadAll(pDispositif);

         // Initialisation de PhysX
         scene.InitPhysX();

         // Initialisation des objets 3D - création et/ou chargement
         if (!InitObjets())
         {
            return 1;
         }

         return 0;
      }

      bool InitObjets()
      {
         scene.InitObjets(pDispositif);

         camera.Init(pDispositif);
         m_MatViewProj = camera.GetMatView() * camera.getFrustrumView().GetMatProj();

         // Création de l'afficheur de sprites et ajout des sprites
         CAfficheurTexte::Init();
         std::unique_ptr<CAfficheurSprite> HUDDisplay = std::make_unique<CAfficheurSprite>(pDispositif);
         std::unique_ptr<CAfficheurSprite> mainMenuDisplay = std::make_unique<CAfficheurSprite>(pDispositif);
         std::unique_ptr<CAfficheurSprite> optionsMenuDisplay = std::make_unique<CAfficheurSprite>(pDispositif);
         std::unique_ptr<CAfficheurSprite> endMenuDisplay = std::make_unique<CAfficheurSprite>(pDispositif);

         hud.Init(pDispositif, HUDDisplay);
         mainMenu.Init(pDispositif, mainMenuDisplay);
         optionsMenu.Init(pDispositif, optionsMenuDisplay);
         endMenu.Init(pDispositif, endMenuDisplay);

         //Puis, il est ajouté à la scène
         mainMenuElements.push_back(std::move(mainMenuDisplay));
         optionsMenuElements.push_back(std::move(optionsMenuDisplay));
         endMenuElements.push_back(std::move(endMenuDisplay));

         pPanneauPE = std::make_unique<CPanneauPE>(pDispositif);

         HUDElements.push_back(std::move(HUDDisplay));

         return true;
      }

      bool AnimeScene(float tempsEcoule)
      {
         GestionnaireDeSaisie.StatutClavier();

         switch (gameState)
         {
         case MAIN_MENU:
            mainMenu.Update(tempsEcoule);
            if (GestionnaireDeSaisie.ToucheAppuyee(DIK_RETURN)) {
               // Le GameManager
               gameManager.Init(scene.getItems(), pDispositif);
               gameState = GAME;
            }
            break;

         case GAME:
            gameManager.Anime();

            if (GestionnaireDeSaisie.ToucheAppuyee(DIK_F5))
                gameManager.addTempsDeJeu(milliseconds{ 1'000'000 });

            scene.Anime(tempsEcoule);
            hud.Update(getVehicule()->physic->GetVitesse(),
               gameManager.getRemainingTime(),
               gameManager.tempsAvantRetourAuReve(),
               gameManager.getNbOursResult()
            );

            if (GestionnaireDeSaisie.ToucheAppuyee(DIK_F1))
               gameState = OPTIONS_MENU;

            if (gameManager.getGameResult() != GameResult::DUNNO)
               gameState = END_MENU;

            break;

         case OPTIONS_MENU:
            optionsMenu.Update(tempsEcoule);
            gameManager.addTempsDeJeu(std::chrono::milliseconds((long long)tempsEcoule*1000));
            if (GestionnaireDeSaisie.ToucheAppuyee(DIK_RETURN))
               gameState = GAME;

            break;

         case END_MENU:
            endMenu.Update(tempsEcoule, gameManager.getGameResult(), gameManager.getTimeSinceBeginning());

            if (GestionnaireDeSaisie.ToucheAppuyee(DIK_RETURN) || GestionnaireDeSaisie.ToucheAppuyee(DIK_ESCAPE))
               exit(0);

            break;
         }

         camera.Update(m_MatViewProj, tempsEcoule);

         return true;
      }

   protected:
      using CObjet3DPtr = std::unique_ptr<CGameObject>;
      GameState gameState = MAIN_MENU;
      // HUD
      HUD hud;
      std::vector<CObjet3DPtr> HUDElements;
      // menus
      MainMenu mainMenu;
      OptionsMenu optionsMenu;
      EndMenu endMenu;
      std::vector<CObjet3DPtr> mainMenuElements;
      std::vector<CObjet3DPtr> optionsMenuElements;
      std::vector<CObjet3DPtr> endMenuElements;

      //Game manager
      GameManager gameManager;


      // Variables pour le temps de l'animation
      int64_t TempsSuivant;
      int64_t TempsCompteurPrecedent;

      // Le dispositif de rendu
      TClasseDispositif* pDispositif;

      // La seule scene
      Scene scene;

      // Les matrices
      DirectX::XMMATRIX m_MatViewProj;

      //Les cameras
      CCamera camera;

      CDIManipulateur GestionnaireDeSaisie;

      // Le gestionnaire de texture
      RessourcesManager ressourcesManager;

      // Le gestionnaire de shaders
      ShaderManager shaderManager;

      // Pour le texte

      // post effect
      std::unique_ptr<CPanneauPE> pPanneauPE;
   };

} // namespace PM3D

#endif // !MOTEUR_H
