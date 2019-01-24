#ifndef AFFICHEUR_SPRITE_H
#define AFFICHEUR_SPRITE_H

#include "Formes/CGameObject.h"

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT2;
using DirectX::XMMATRIX;

namespace PM3D
{

class CDispositifD3D11;

class CSommetSprite
{
public:
	CSommetSprite() = default;
	CSommetSprite(const XMFLOAT3& position, const XMFLOAT2& coordTex)
		: m_Position(position)
		, m_CoordTex(coordTex)
	{
	}

public:
	static UINT numElements;
	static D3D11_INPUT_ELEMENT_DESC layout[];

	XMFLOAT3 m_Position;
	XMFLOAT2 m_CoordTex;
};

class CAfficheurSprite : public CGameObject
{
public:
	CAfficheurSprite(CDispositifD3D11* _pDispositif);
	virtual ~CAfficheurSprite();
	virtual void Draw(ID3D11ShaderResourceView* pDepthShaderResourceView = nullptr) override;

	void AjouterSprite(const std::wstring& NomTexture, int _x, int _y, int _dx = 0, int _dy = 0);
	void AjouterPanneau(const std::string& NomTexture, const XMFLOAT3& _position,
		float _dx = 0.0f, float _dy = 0.0f);
	void AjouterSpriteTexte(ID3D11ShaderResourceView* pTexture, const DirectX::XMFLOAT2&);
    void MoveSprite(size_t index, int dx, int dy, const DirectX::XMFLOAT2&);

private:
	class CSprite
	{
	public:
		ID3D11ShaderResourceView * pTextureD3D;

		XMMATRIX matPosDim;
		bool bPanneau;
		CSprite()
			: bPanneau(false)
			, pTextureD3D(nullptr)
		{
		}
	};

	class CPanneau : public CSprite
	{
	public:
		XMFLOAT3 position;
		XMFLOAT2 dimension;

		CPanneau()
		{
			bPanneau = true;
		}
	};

	static CSommetSprite sommets[6];
	ID3D11Buffer* pVertexBuffer;
	CDispositifD3D11* pDispositif;

	ID3D11Buffer* pConstantBuffer;
	ID3DX11Effect* pEffet;
	ID3DX11EffectTechnique* pTechnique;
	ID3DX11EffectPass* pPasse;
	ID3D11InputLayout* pVertexLayout;

	ID3D11SamplerState* pSampleState;

	// Tous nos sprites
	std::vector<std::unique_ptr<CSprite>> tabSprites;

	void InitEffet();
};

} // namespace PM3D

#endif // !AFFICHEUR_SPRITE_H
