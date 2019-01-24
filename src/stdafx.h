// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclure les en-têtes Windows rarement utilisés
// Fichiers d'en-tête Windows :
#include <windows.h>
//#undef min
//#undef max

// Fichiers d'en-tête C RunTime
#include <cassert>
#include <cstdint>
#include <tchar.h>

// Fichiers d'en-tête C++ RunTime
#include <string>
#include <exception>
#include <memory>
#include <vector>
#include <array>
#include <math.h>
#include <fstream>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <dxgiformat.h>
#include <assert.h>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#define _USE_MATH_DEFINES
using std::vector;

// TODO : faites référence ici aux en-têtes supplémentaires nécessaires au programme
#define _XM_NO_INTRINSICS_
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "d3dx11effect.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

// Assimp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

// Moteur Physique
#include "PxPhysicsAPI.h"
