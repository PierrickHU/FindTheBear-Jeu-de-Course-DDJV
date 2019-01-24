// PetitMoteur3D.cpp: definit le point d'entree pour l'application.
//

#include "stdafx.h"
#include "PetitMoteur3D.h"
#include "MoteurWindows.h"

using namespace PM3D;

int APIENTRY _tWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	// Pour ne pas avoir d'avertissement
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

    // Creation de l'objet Moteur
    CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

    // Specifiques a une application Windows
    rMoteur.SetWindowsAppInstance(hInstance);

    // Initialisation du moteur
    rMoteur.Initialisations();

    // Boucle d'application
    rMoteur.Run();

    return (int)1;
}
