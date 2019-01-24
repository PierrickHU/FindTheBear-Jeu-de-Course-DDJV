#ifndef ZONE_H
#define ZONE_H

#include "stdafx.h"
#include <vector>

using IndexContainer = std::vector<unsigned int>;

enum CornerPosition { BOT_LEFT, TOP_LEFT, TOP_RIGHT, BOT_RIGHT };

class Zone {
private:
    static constexpr size_t NB_OBJETS_PAR_ZONE = 100;

public:
    IndexContainer indexContainer;
    std::array<DirectX::XMVECTOR, 4> corners;

    Zone() = default;
    Zone(DirectX::XMVECTOR _centerPosition, DirectX::XMVECTOR _size)
    {
        DirectX::XMFLOAT3 size;
        DirectX::XMStoreFloat3(&size, _size);

        auto translateX = size.x / 2;
        auto translateZ = size.z / 2;

        corners = {
            DirectX::XMVectorAdd(_centerPosition, { -translateX, 0.f, -translateZ }),
            DirectX::XMVectorAdd(_centerPosition, { -translateX, 0.f, translateZ }),
            DirectX::XMVectorAdd(_centerPosition, { translateX, 0.f, translateZ }),
            DirectX::XMVectorAdd(_centerPosition, { translateX, 0.f, -translateZ })
        };

        indexContainer.reserve(NB_OBJETS_PAR_ZONE);
    }
    ~Zone() = default;
};

#endif