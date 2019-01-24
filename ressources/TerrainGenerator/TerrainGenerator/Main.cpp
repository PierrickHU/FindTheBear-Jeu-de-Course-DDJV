
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "RandomGenerateur.h"
using namespace std;;

struct CaracteristiqueMap {
    string nomFichier;
    int nbPointsX;
    int nbPointsY;
    int nbPointsDeControleX;
    int nbPointsDeControleY;
};

struct Vector2 {
    float x, y;
};

template<class T>
T demander(string message) {
    T t;
    for (; cout << message && !(cin >> t); cin.clear(), cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'));
    cin.clear();
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return t;
}

CaracteristiqueMap demanderCharacteristiques() {
    CaracteristiqueMap cm;
    cm.nomFichier = demander<string>("Entrez le nom du fichier : ");
    cm.nbPointsX = demander<int>("Entrez le nombre de points en X : ");
    cm.nbPointsY = demander<int>("Entrez le nombre de points en Y : ");
    cm.nbPointsDeControleX = demander<int>("Entrez le nombre de points de controle en X : ");
    cm.nbPointsDeControleY = demander<int>("Entrez le nombre de points de controle en Y : ");
    return cm;
}

float dotGridGradient(int ix, int iy, float x, float y, vector<vector<Vector2>> pdc, CaracteristiqueMap cm) {
    float dx = x - (float)ix * (cm.nbPointsX / cm.nbPointsDeControleX);
    float dy = y - (float)iy * (cm.nbPointsY / cm.nbPointsDeControleY);

    return dx * pdc[iy][ix].x + dy * pdc[iy][ix].y; // dot "."
}

float perlin(int x, int y, vector<vector<Vector2>> pdc, CaracteristiqueMap cm) {
    // Coordonnées des 4 pdc englobant
    int x0 = min(x / (cm.nbPointsX / cm.nbPointsDeControleX), cm.nbPointsDeControleX - 1);
    int y0 = min(y / (cm.nbPointsY / cm.nbPointsDeControleY), cm.nbPointsDeControleY - 1);
    int x1 = min(x0 + 1, cm.nbPointsDeControleX - 1);
    int y1 = min(y0 + 1, cm.nbPointsDeControleY - 1);

    // Derterminer les poids d'interpolations
    float poidsX = static_cast<float>(x - x0 * (cm.nbPointsX / cm.nbPointsDeControleX)) / static_cast<float>(static_cast<float>(cm.nbPointsX) / static_cast<float>(cm.nbPointsDeControleX));
    float poidsY = static_cast<float>(y - y0 * (cm.nbPointsY / cm.nbPointsDeControleY)) / static_cast<float>(static_cast<float>(cm.nbPointsY) / static_cast<float>(cm.nbPointsDeControleY));

    // Interpolation
    float n0, n1, ix0, ix1, value;
    n0 = dotGridGradient(x0, y0, x, y, pdc, cm);
    n1 = dotGridGradient(x1, y0, x, y, pdc, cm);
    ix0 = (1.0f - poidsX)*n0 + poidsX * n1;
    n0 = dotGridGradient(x0, y1, x, y, pdc, cm);
    n1 = dotGridGradient(x1, y1, x, y, pdc, cm);
    ix1 = (1.0f - poidsX)*n0 + poidsX * n1;
    value = (1.0f - poidsY)*ix0 + poidsY * ix1;

    return value;
}

int normalizeIn0_255(float val, float valMin, float valMax) {
    return 0.f + (valMax - val) * (255.f - 0.f) / (valMax - valMin);
}

void creerMap(CaracteristiqueMap cm, vector<vector<Vector2>> pdc) {
    ofstream out{ cm.nomFichier };
    out << cm.nbPointsX << " ";
    out << cm.nbPointsY << endl;
    out << "10.0 0.3" << endl; // Les ratios

    // Créer les valeurs
    cout << "Calcul des valeurs brutes ..." << endl;
    vector<vector<float>> hauteursBrutes;
    float valMin = 0, valMax = 0;
    for (int i = 0; i < cm.nbPointsX; i++) {
        vector<float> v;
        cout << "Ligne " << i << " des valeurs brutes ..." << endl;
        for (int j = 0; j < cm.nbPointsY; j++) {
            v.push_back(perlin(i, j, pdc, cm));
            if (v.back() < valMin)
                valMin = v.back();
            if (v.back() > valMax)
                valMax = v.back();
        }
        hauteursBrutes.push_back(v);
    }

    // Ecrire les valeurs normalisées
    cout << "Calcul des valeurs normalisées ..." << endl;
    for (int i = 0; i < cm.nbPointsX; i++) {
        cout << "Ligne " << i << " des valeurs normalisées ..." << endl;
        for (int j = 0; j < cm.nbPointsY; j++) {
            out << normalizeIn0_255(hauteursBrutes[i][j], valMin, valMax);
            if (j != cm.nbPointsY - 1)
                out << " ";
        }
        if (i != cm.nbPointsX - 1)
            out << endl;
    }
}

bool isInCentre(int x, int y, CaracteristiqueMap cm) {
    int xIndCentre = cm.nbPointsDeControleX / 2;
    int yIndCentre = cm.nbPointsDeControleY / 2;
    float rayon = (static_cast<float>(cm.nbPointsDeControleX) + static_cast<float>(cm.nbPointsDeControleY)) / 6.f;
    int dx = abs(x - xIndCentre);
    int dy = abs(y - yIndCentre);
    float distance = sqrt(static_cast<float>(dx) * static_cast<float>(dx) + static_cast<float>(dy) * static_cast<float>(dy));
    return distance <= rayon;
}

vector<vector<Vector2>> calculerPointsDeControles(CaracteristiqueMap cm) {
    vector<vector<Vector2>> pdc;
    RandomGenerateur& rd = RandomGenerateur::get();
    
    for (int i = 0; i < cm.nbPointsDeControleX; i++) {
        vector<Vector2> v;
        cout << "Ligne " << i << " des points de controle ..." << endl;
        for (int j = 0; j < cm.nbPointsDeControleY; j++) {
            Vector2 v2{ rd.random(-1.f, 1.f), rd.random(-1.f, 1.f) };
            float norme = sqrt(v2.x * v2.x + v2.y * v2.y);
            v2.x /= norme;
            v2.y /= norme;
            float coefAmortissementCentre = 0.3f;
            if (isInCentre(i, j, cm)) {
                v2.x *= coefAmortissementCentre;
                v2.y *= coefAmortissementCentre;
            }
            v.push_back(v2);
        }
        pdc.push_back(v);
    }

    return pdc;
}

int main() {
    CaracteristiqueMap caracteristiques = demanderCharacteristiques();

    cout << "Calcul des points de contrôle ..." << endl;
    vector<vector<Vector2>> pointsDeControles = calculerPointsDeControles(caracteristiques);

    cout << "Création de la carte ..." << endl;
    creerMap(caracteristiques, pointsDeControles);
}