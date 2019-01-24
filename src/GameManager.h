#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "Item.h"
#include "dispositifD3D11.h"
#include "Formes/Orientation.h"

#include <array>
#include <vector>
#include <chrono>
#include <string>

using std::vector;
using std::wstring;
using std::unique_ptr;
using std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::milliseconds;

namespace PM3D {

    class mauvais_monde_exception {};

    enum GameResult { DUNNO, WIN, LOSE };
    // Le but de cette classe est de centraliser toute la logique de jeu

    class GameManager {
    public:
        enum Monde { REVE, CAUCHEMAR };


    private:
        const int NB_PARTIES_OURS = 8;
        const int NB_BOOSTS = 15;
        const int NB_FIOLE_DE_TEMPS = 15;
        milliseconds tempsDeJeu{ 40'000 };
        milliseconds dureeMinimaleCauchemar{ 20'000 };
        milliseconds dureeMinimaleAvantSwapMonde{ 1'000 };
        int nbPartiesOursActuel = NB_PARTIES_OURS;
        int nbBoostActuel = NB_BOOSTS;
        int nbFiolesDeTemps = NB_FIOLE_DE_TEMPS;
        Monde mondeActuel = Monde::REVE;
        time_point<steady_clock> tempsDebutJeu;
        GameResult gameResult = DUNNO;
        wstring messageDefaite{};

        // Effets de transitions
        time_point<steady_clock> lastTimeChangementMonde;
        milliseconds tempsEffetTransitionCroissant{1 * 1000};
        milliseconds tempsEffetTransitionDecroissant{500};

    public:
        GameManager();
        std::pair<unsigned int, unsigned int> getNbOursResult();
        void Init(vector<unique_ptr<Item>>& items, CDispositifD3D11* dispositif); // S'occupe d'instancier les diff�rents items (parties de l'ours, boost, fioles de temps) du jeu
        void Anime();
        void changerMonde(); // Change de monde !
        Orientation getRandomOrientation(float offsetY, float dx, float dz) const;
        Orientation getRandomOrientationArbre(float offsetY, float dx, float dz) const;

        void decrementPartiesOurs();
        void addTempsDeJeu(milliseconds tempsAdditionnel);

        void perdre(wstring messageDefaite);
        void gagner();
        wstring getMessageDefaite() const noexcept;

        GameResult getGameResult() const;
        milliseconds getRemainingTime() const;
        milliseconds getTimeSinceBeginning() const;
        Monde getMondeActuel() const noexcept; // Permet de savoir si on est dans le monde du Reve ou le monde du Cauchemar
        milliseconds tempsAvantRetourAuReve() const; // Permet de savoir le temps qu'il reste avant de pouvoir retourner au Reve

        float shader_getPuissanceAttenuation() const; // Permet de r�cup�rer le param�tre d'un shader pour l'effet de transition ! <3
        float shader_getPuissanceEclaircissement() const; // Permet de r�cup�rer le param�tre d'un shader pour l'effet de transition ! <3
    };

} // namespace PM3D


#endif