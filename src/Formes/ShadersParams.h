#ifndef SHADERS_PARAMS_H
#define SHADERS_PARAMS_H

#include "stdafx.h"

struct ShadersParams
{
   DirectX::XMMATRIX matWorldViewProj;	// la matrice totale 
   DirectX::XMMATRIX matWorldViewProjLight;  // WVP pour la lumiere
   DirectX::XMMATRIX matWorld;		   // matrice de transformation dans le monde 
   DirectX::XMVECTOR posLumiere; 	   // la position de la source d'eclairage (Point)
   DirectX::XMVECTOR dirLumiere; 	   // la direction de la source d'eclairage (Directionnel)
   DirectX::XMVECTOR vCamera;          // la position de la camera
   DirectX::XMVECTOR vAEcl;            // la valeur ambiante de l'eclairage     
   DirectX::XMVECTOR vAMat;            // la valeur ambiante du materiau        KA
   DirectX::XMVECTOR vDEcl;            // la valeur diffuse de l'eclairage      
   DirectX::XMVECTOR vSEcl;            // la valeur speculaire de l'eclairage   
   DirectX::XMVECTOR vDMat;            // la valeur diffuse du materiau         KD
   DirectX::XMVECTOR vSMat;            // la valeur speculaire du materiau      KS
   float puissance;			           // la puissance de specularite
   int bTex;                           // Texture ou materiau 
   int bTunnel;
   DirectX::XMVECTOR centreTerrain;    // Le centre du terrain
   // ****** TAMPON POUR LA TAILLE *******
   char tamponAjustement[4];          // tampon inutile permettant de s'assurer que le buffer a une taille multiple de 16
};


#endif