#include "stdafx.h"
#include "CSommet.h"

namespace PM3D
{
// Definir l'organisation de notre sommet
D3D11_INPUT_ELEMENT_DESC CSommet::layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BITANGENT", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

UINT CSommet::numElements = ARRAYSIZE(CSommet::layout);

CSommet::CSommet(const DirectX::XMFLOAT3& position)
    : m_Position(position)
{}

CSommet::CSommet(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT2& texCoord)
	: m_Position(position)
	, m_Normal(normal)
    , m_TexCoord(texCoord)
{}

CSommet::CSommet(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal)
   : m_Position(position)
   , m_Normal(normal)
{}

} // namespace PM3D
