#include "stdafx.h"
#include "dispositifD3D11.h"
#include "Texture.h"
#include "resource.h"
#include "util.h"
#include "DDSTextureLoader.h"

namespace PM3D
{

CTexture::~CTexture()
{
	// DXRelacher(ressource);
}

CTexture::CTexture(const std::wstring& filename, CDispositifD3D11* pDispositif)
	: m_Filename(filename)
	, ressource(nullptr)
{
	ID3D11Device* pDevice = pDispositif->GetD3DDevice();

	// Charger la texture en ressource
	DXEssayer(DirectX::CreateDDSTextureFromFile(pDevice,
		m_Filename.c_str(),
		nullptr,
		&ressource), DXE_FICHIERTEXTUREINTROUVABLE);
}

} // namespace PM3D
