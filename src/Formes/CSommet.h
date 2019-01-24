#ifndef C_SOMMET_H
#define C_SOMMET_H

namespace PM3D
{
     class CSommet
     {
     public:
          CSommet() = default;
          CSommet(const DirectX::XMFLOAT3& position);
          CSommet(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal);
          CSommet(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT2& texCoord);

          static UINT numElements;
          static D3D11_INPUT_ELEMENT_DESC layout[];

          DirectX::XMFLOAT3 m_Position;
          DirectX::XMFLOAT3 m_Normal;
          DirectX::XMFLOAT2 m_TexCoord;
			 DirectX::XMFLOAT3 m_Tangent;
			 DirectX::XMFLOAT3 m_Bitangent;


     };

} // namespace PM3D
#endif // C_SOMMET_H