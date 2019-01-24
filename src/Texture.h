#ifndef TEXTURE_H
#define TEXTURE_H

#include "dispositifD3D11.h"

namespace PM3D
{

class CTexture
{
public:
	CTexture(const std::wstring& filename, CDispositifD3D11* pDispositif);
	~CTexture();

    ID3D11ShaderResourceView* ressource;

	const std::wstring& GetFilename() const { return m_Filename; }

private:
	std::wstring m_Filename;
};

} // namespace PM3D
#endif