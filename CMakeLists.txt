cmake_minimum_required( VERSION 3.0 )
project( JeuCourse C CXX )

#add_definitions(-DDEBUG_FRAMEWORK)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

set_property(GLOBAL PROPERTY CMAKE_SKIP_INSTALL_ALL_DEPENDENCY  ON)
set_property(GLOBAL PROPERTY CMAKE_SKIP_PACKAGE_ALL_DEPENDENCY  ON)
set_property(GLOBAL PROPERTY CMAKE_SUPPRESS_REGENERATION  ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#Compile Option
SET(MY_COMPILE_OPTIONS /std:c++latest -D_UNICODE -DUNICODE /MP /FR )

set(MY_RELEASE_OPTIONS ${MY_COMPILE_OPTIONS})
set(MY_DEBUG_OPTIONS ${MY_COMPILE_OPTIONS} -D_DEBUG /MDd)

set(JEU_COURSE_RELEASE_OPTIONS ${MY_RELEASE_OPTIONS} /Yu)
set(JEU_COURSE_DEBUG_OPTIONS ${MY_DEBUG_OPTIONS} /Yu)

set(MOTEUR_PHYSX_RELEASE_OPTIONS ${MY_RELEASE_OPTIONS})
set(MOTEUR_PHYSX_DEBUG_OPTIONS ${MY_DEBUG_OPTIONS})

#add_compile_options("$<$<CONFIG:RELEASE>:${MY_RELEASE_OPTIONS}>")
#add_compile_options("$<$<CONFIG:DEBUG>:${MY_DEBUG_OPTIONS}>")

SET (DOSSIER_MOTEUR_PHYSX ${CMAKE_SOURCE_DIR}/MoteurPhysX)
SET (DOSSIER_BIN_SORTIE ${CMAKE_SOURCE_DIR}/Bin)
SET (DOSSIER_INCLUDE_SORTIE ${CMAKE_SOURCE_DIR}/Include)

#Include .h directories
SET (MOTEUR_PHYSX_INCLUDE_DIR
	${DOSSIER_MOTEUR_PHYSX}/Include
	${DOSSIER_MOTEUR_PHYSX}/Include/Graphics/include/win32/GL
	${DOSSIER_MOTEUR_PHYSX}/Include/PxShared/include
	${DOSSIER_MOTEUR_PHYSX}/Include/PxInclude
	${DOSSIER_MOTEUR_PHYSX}/Include/PxShared/src/foundation/include
	${DOSSIER_MOTEUR_PHYSX}/Include/PxToolkit/include
)
SET (JEU_COURSE_INCLUDE_DIR
	${CMAKE_SOURCE_DIR}/src
	${DOSSIER_MOTEUR_PHYSX}/src
	${CMAKE_SOURCE_DIR}/Include/External_Libraries/assimp
	${MOTEUR_PHYSX_INCLUDE_DIR}
)

include_directories(${JEU_COURSE_INCLUDE_DIR})
include_directories(${MOTEUR_PHYSX_INCLUDE_DIR})

#Include .lib directories
SET (MOTEUR_PHYSX_LIB_DIR
	${DOSSIER_MOTEUR_PHYSX}/Include
	${DOSSIER_MOTEUR_PHYSX}/Include/PxShared/lib/vc15win64
	${DOSSIER_MOTEUR_PHYSX}/Include/SnippetUtils/Lib
	${DOSSIER_MOTEUR_PHYSX}/Include/SnippetRender/Lib
	${DOSSIER_MOTEUR_PHYSX}/Include/Graphics/lib/win64/glut
	${DOSSIER_MOTEUR_PHYSX}/Include/API_PhysX/Lib
	${DOSSIER_MOTEUR_PHYSX}/Include/PxToolkit/lib
)
SET (JEU_COURSE_LIB_DIR
	${CMAKE_SOURCE_DIR}/Include
	${CMAKE_SOURCE_DIR}/Include/External_Libraries/Lib
)
link_directories(${JEU_COURSE_LIB_DIR})
link_directories(${MOTEUR_PHYSX_LIB_DIR})

#Projet Jeu de Voiture
SET( JEU_COURSE_RESSOURCES 
	ressources/PetitMoteur3D.ico
	ressources/small.ico
	ressources/PetitMoteur3D.rc
	ressources/terrains/heightmap.txt
	ressources/terrain.dat
)

SET( JEU_COURSE_HEADERS 
	src/GameManager.h
	# Formes
	src/Formes/Bloc.h
	src/Formes/Chateau.h
	# src/Formes/Cone.h
	# src/Formes/Cylindre.h
	# src/Formes/Sphere.h
	src/Formes/ShadersParams.h
	src/Formes/Orientation.h
	src/Formes/Orientation.h
	src/Formes/CGameObject.h
	src/Formes/CSimpleGameObject.h
	src/Formes/CTerrain.h
	src/Formes/CSommet.h
	src/Formes/Billboard.h

    src/Component/CamInputComponent.h
    src/Component/CamFirstPersonComponent.h
    src/Component/CamFreeInputComponent.h
	src/Component/CamThirdPersonInputComponent.h
    src/Component/InputComponent.h
    src/Component/InputMenuComponent.h
    src/Component/InputPlayerComponent.h
    src/Component/MeshComponent.h
	src/Component/BillboardMeshComponent.h
    src/Component/PhysicComponent.h
    src/Component/PhysicDynamicComponent.h
    src/Component/PhysicPlayerComponent.h
    src/Component/PhysicTerrainComponent.h
    src/Component/PhysicTriggerComponent.h
    src/Camera/CCamera.h
    src/Camera/FrustrumView.h
    src/Camera/Lumiere.h
    src/Camera/LumiereManager.h
	src/d3dx11effect.h
	src/d3dxGlobal.h
	src/DIManipulateur.h
	src/dispositif.h
	src/dispositifD3D11.h
	src/Horloge.h
	src/InfoDispositif.h
	src/ISerializable.h
	src/Logger.h
    src/Material.h
	src/Moteur.h
	src/MoteurWindows.h
	src/PetitMoteur3D.h
	src/Resource.h
	src/Singleton.h
	src/stdafx.h
	src/toolsMath.h
	src/util.h
	src/CScene.h
	src/Item.h
	src/PartieOurs.h
	src/FioleDeTemps.h
	src/Boost.h

	src/ScenePhysique.h
	src/DDSTextureLoader.h
	src/RessourcesManager.h
	src/Texture.h
	src/Zone.h
	src/RandomGenerateur.h
    src/Vehicule.h
    src/Minuteur.h
    src/PanneauPE.h
    src/AfficheurSprite.h
    src/AfficheurTexte.h
	src/Skybox.h
	#LOADER STOP LENLEVER SVP
	src/CAssimpLoader.h
	src/CFileLoader.h
	src/HUD.h
	src/GameState.h
	src/MainMenu.h
	src/EndMenu.h
	src/OptionsMenu.h
	src/ShaderManager.h
)

SET( JEU_COURSE_SRC 
	src/GameManager.cpp
	# Formes
	src/Formes/Bloc.cpp
	src/Formes/Chateau.cpp
	# src/Formes/Cone.cpp
	# src/Formes/Cylindre.cpp
	# src/Formes/Sphere.cpp
	src/Formes/Orientation.cpp
	src/Formes/CSommet.cpp
	src/Formes/CTerrain.cpp
	src/Formes/Billboard.cpp
    src/Formes/CSimpleGameObject.cpp

    src/Component/CamFreeInputComponent.cpp
	src/Component/CamFirstPersonComponent.cpp
	src/Component/CamThirdPersonInputComponent.cpp
    src/Component/InputPlayerComponent.cpp
	src/Component/InputMenuComponent.cpp
    src/Component/MeshComponent.cpp
	src/Component/BillboardMeshComponent.cpp
    src/Component/PhysicPlayerComponent.cpp
    src/Component/PhysicTerrainComponent.cpp
    src/Component/PhysicDynamicComponent.cpp
	src/Component/PhysicTriggerComponent.cpp
	#Framework
	src/Camera/CCamera.cpp
    src/Camera/FrustrumView.cpp
    src/Camera/Lumiere.cpp
    src/Camera/LumiereManager.cpp
	src/DIManipulateur.cpp
	src/dispositif.cpp
	src/dispositifD3D11.cpp
	src/Horloge.cpp
	src/InfoDispositif.cpp
	src/Logger.cpp
	src/MoteurWindows.cpp
	src/PetitMoteur3D.cpp
	src/stdafx.cpp
	src/toolsMath.cpp
	src/CScene.cpp
	src/ScenePhysique.cpp
	src/DDSTextureLoader.cpp
	src/RessourcesManager.cpp
	src/Texture.cpp
	src/Item.cpp
	src/PartieOurs.cpp
	src/FioleDeTemps.cpp
	src/Boost.cpp
	src/HUD.cpp

	src/Vehicule.cpp
    src/Minuteur.cpp
    src/PanneauPE.cpp
    src/AfficheurSprite.cpp
    src/AfficheurTexte.cpp
	src/Skybox.cpp
    src/Material.cpp
    src/EndMenu.cpp

	#LOADER STOP LENLEVER SVP
	src/CAssimpLoader.cpp
	src/MainMenu.cpp
	src/OptionsMenu.cpp
	src/ShaderManager.cpp
)
SET( JEU_COURSE_SHADERS 
	
	src/shaders/MiniPhong.fx
	src/shaders/Terrain.fx
	src/shaders/PostEffect.fx
    src/shaders/MiniPhongSM.fx
    src/shaders/Sprite1.fx
	src/shaders/Skybox.fx
	src/shaders/SkyboxET.fx
    src/shaders/TerrainSM.fx
	src/shaders/TerrainNM.fx
	src/shaders/ChateauNM.fx
)

SET(jobfiles "${JEU_COURSE_HEADERS};\n${JEU_COURSE_SRC};\n${JEU_COURSE_SHADERS};\n${JEU_COURSE_RESSOURCES}")
file(WRITE jobfilesJeuVoiture.txt "${jobfiles}")

#precompiled file
set_source_files_properties(src/stdafx.cpp PROPERTIES COMPILE_OPTIONS "/Ycstdafx.h")

add_executable(
	JeuCourse
	WIN32
	${JEU_COURSE_SRC}
	${JEU_COURSE_HEADERS}
	${JEU_COURSE_RESSOURCES}
	${JEU_COURSE_SHADERS}
)

SET( JEU_COURSE_TARGET_PROPERTIES 
	RUNTIME_OUTPUT_DIRECTORY "${DOSSIER_BIN_SORTIE}"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${DOSSIER_BIN_SORTIE}"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${DOSSIER_BIN_SORTIE}"
)

SET( MOTEUR_PHYSX_TARGET_LINK_LIBRARIES_DEBUG 
	PhysX3CharacterKinematicDEBUG_x64.lib
	PhysX3CommonDEBUG_x64.lib
	PhysX3CookingDEBUG_x64.lib
	PhysX3DEBUG_x64.lib
	PhysX3ExtensionsDEBUG.lib
	PhysX3VehicleDEBUG.lib
	SceneQueryDEBUG.lib
	SimulationControllerDEBUG.lib
	PsFastXmlDEBUG_x64.lib
	PxFoundationDEBUG_x64.lib
	PxPvdSDKDEBUG_x64.lib
	PxTaskDEBUG_x64.lib
	SnippetUtilsDEBUG.lib
	SampleBaseDEBUG.lib
	PxToolkitDEBUG.lib
)

SET( MOTEUR_PHYSX_TARGET_LINK_LIBRARIES_RELEASE 
	PhysX3CharacterKinematic_x64.lib
	PhysX3Common_x64.lib
	PhysX3Cooking_x64.lib
	PhysX3_x64.lib
	PhysX3Extensions.lib
	PhysX3Vehicle.lib
	SceneQuery.lib
	SimulationController.lib
	PsFastXml_x64.lib
	PxFoundation_x64.lib
	PxPvdSDK_x64.lib
	PxTask_x64.lib
	SnippetUtils.lib
	SampleBase.lib
	PxToolkit.lib
)

SET( JEU_COURSE_TARGET_LINK_LIBRARIES 
	dxguid.lib
	winmm.lib
	d3d11.lib
	DXGI.lib
	d3dcompiler.lib
	dinput8.lib
	kernel32.lib
	user32.lib
	gdi32.lib
	winspool.lib
	comdlg32.lib
	advapi32.lib
	shell32.lib
	ole32.lib
	oleaut32.lib
	uuid.lib
	odbc32.lib
	odbccp32.lib
	assimp.lib
)

SET( JEU_COURSE_TARGET_LINK_LIBRARIES_DEBUG
	${JEU_COURSE_TARGET_LINK_LIBRARIES}
	Effects11d.lib
	${MOTEUR_PHYSX_TARGET_LINK_LIBRARIES_DEBUG}
)

SET( JEU_COURSE_TARGET_LINK_LIBRARIES_RELEASE
	${JEU_COURSE_TARGET_LINK_LIBRARIES}
	Effects11.lib
	${MOTEUR_PHYSX_TARGET_LINK_LIBRARIES_RELEASE}
)


set_target_properties(JeuCourse  PROPERTIES ${JEU_COURSE_TARGET_PROPERTIES})

target_link_libraries(JeuCourse "$<$<CONFIG:RELEASE>:${JEU_COURSE_TARGET_LINK_LIBRARIES_RELEASE}>")
target_link_libraries(JeuCourse "$<$<CONFIG:DEBUG>:${JEU_COURSE_TARGET_LINK_LIBRARIES_DEBUG}>")

target_compile_options(JeuCourse PUBLIC "$<$<CONFIG:RELEASE>:${JEU_COURSE_RELEASE_OPTIONS}>")
target_compile_options(JeuCourse PUBLIC "$<$<CONFIG:DEBUG>:${JEU_COURSE_DEBUG_OPTIONS}>")

#Copie des shader du src dans le dossier ressources
file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR}/src/shaders DOSSIER_SHADERS_SRC_NATIVE_PATH)
file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR}/ressources/shaders DOSSIER_SHADERS_RESSOURCE_NATIVE_PATH)

add_custom_command(TARGET JeuCourse
                   POST_BUILD
                   COMMAND xcopy ARGS "/y /f /e /i \"${DOSSIER_SHADERS_SRC_NATIVE_PATH}\"  \"${DOSSIER_SHADERS_RESSOURCE_NATIVE_PATH}\""
                   COMMENT "Running copy shaders")
#Copie des dll de assimp du Include dans le Bin
file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR}/Include/External_Libraries/Assimp/ DOSSIER_ASSIMP_NATIVE_PATH)
file(TO_NATIVE_PATH ${DOSSIER_BIN_SORTIE} DOSSIER_BIN_SORTIE_NATIVE_PATH)

add_custom_command(TARGET JeuCourse
                   PRE_BUILD
                   COMMAND xcopy ARGS "/y /f /e /i \"${DOSSIER_ASSIMP_NATIVE_PATH}DLL\"  \"${DOSSIER_BIN_SORTIE_NATIVE_PATH}\""
                   COMMENT "Running copy dll")


#Projet moteurPhysX
SET( MOTEUR_PHYSX_HEADERS 
	${DOSSIER_MOTEUR_PHYSX}/src/Incopiable.h
	${DOSSIER_MOTEUR_PHYSX}/src/SampleAllocator.h
	${DOSSIER_MOTEUR_PHYSX}/src/commonImport.h
)

SET( MOTEUR_PHYSX_SRC 

	#Framework
	${DOSSIER_MOTEUR_PHYSX}/src/SampleAllocator.cpp
)

SET(jobfiles "${MOTEUR_PHYSX_HEADERS};\n${MOTEUR_PHYSX_SRC}")
file(WRITE jobfilesMoteurPhysX.txt "${jobfiles}")

add_library(
	MoteurPhysX
	SHARED
	${MOTEUR_PHYSX_SRC}
	${MOTEUR_PHYSX_HEADERS}
)

SET( MOTEUR_PHYSX_TARGET_PROPERTIES 
	LIBRARY_OUTPUT_DIRECTORY "${DOSSIER_BIN_SORTIE}"
	LIBRARY_OUTPUT_DIRECTORY_DEBUG "${DOSSIER_BIN_SORTIE}"
	LIBRARY_OUTPUT_DIRECTORY_RELEASE "${DOSSIER_BIN_SORTIE}"
)
set_target_properties(MoteurPhysX  PROPERTIES ${MOTEUR_PHYSX_TARGET_PROPERTIES})

target_link_libraries(MoteurPhysX "$<$<CONFIG:RELEASE>:${MOTEUR_PHYSX_TARGET_LINK_LIBRARIES_RELEASE}>")
target_link_libraries(MoteurPhysX "$<$<CONFIG:DEBUG>:${MOTEUR_PHYSX_TARGET_LINK_LIBRARIES_DEBUG}>")

target_compile_options(MoteurPhysX PUBLIC "$<$<CONFIG:RELEASE>:${MOTEUR_PHYSX_RELEASE_OPTIONS}>")
target_compile_options(MoteurPhysX PUBLIC "$<$<CONFIG:DEBUG>:${MOTEUR_PHYSX_DEBUG_OPTIONS}>")

file(TO_NATIVE_PATH ${DOSSIER_MOTEUR_PHYSX}/Include/API_PhysX/ DOSSIER_MOTEUR_PHYSX_NATIVE_PATH)

add_custom_command(TARGET MoteurPhysX
                   PRE_BUILD
                   COMMAND xcopy ARGS "/y /f /e /i \"${DOSSIER_MOTEUR_PHYSX_NATIVE_PATH}DLL\"  \"${DOSSIER_BIN_SORTIE_NATIVE_PATH}\""
                   COMMENT "Running copy dll")

#Link Projects
target_link_libraries(JeuCourse MoteurPhysX)

# Filters
include( Filters.cmake )
CREATE_VISUAL_STUDIO_FILTERS(JEU_COURSE)
CREATE_VISUAL_STUDIO_FILTERS(JEU_COURSE)
