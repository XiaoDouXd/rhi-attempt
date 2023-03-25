#include "gRes.h"
#include "xdBase/exce.h"
#include <memory>

namespace XD::Render
{
    GRes::GRes() : _uuid(UUID::gen()) { }
    uuids::uuid GRes::getUUID() const { return _uuid; }

    // ------------------------------------------ layoutTypeDef

    const std::array<size_t, (size_t)BufferLayoutType::Num> BufferLayoutByte =
    {
        0,      // None,

        1,      // SI8,
        2,      // SI16,
        4,      // SI32,
        8,      // SI64,

        1,      // UI8,
        2,      // UI16,
        4,      // UI32,
        8,      // UI64,

        1,      // SN8,
        2,
        4,
        8,
        2,      // SF16,
        4,      // SF32,
        8,      // SF64,

        1,      // UNI8,
        2,
        4,
        8,
        2,      // UF16,
        4,      // UF32,
        8,      // UF64,

        2,      // SI8_2D,
        4,      // SI16_2D,
        8,      // SI32_2D,
        16,     // SI64_2D,

        2,      // UI8_2D,
        4,      // UI16_2D,
        8,      // UI32_2D,
        16,     // UI64_2D,

        2,      // SN8_2D,
        4,
        8,
        16,
        4,      // SF16_2D,
        8,      // SF32_2D,
        16,     // SF64_2D,

        2,      // UN8_2D,
        4,
        8,
        16,
        4,      // UF16_2D,
        8,      // UF32_2D,
        16,     // UF64_2D,

        3,      // SI8_3D,
        6,      // SI16_3D,
        12,     // SI32_3D,
        24,     // SI64_3D,

        3,      // UI8_3D,
        6,      // UI16_3D,
        12,     // UI32_3D,
        24,     // UI64_3D,

        3,      // SN8_3D,
        6,
        12,
        24,
        6,      // SF16_3D,
        12,     // SF32_3D,
        24,     // SF64_3D,

        3,      // UN8_3D,
        6,
        12,
        24,
        6,      // UF16_3D,
        12,     // UF32_3D,
        24,     // UF64_3D,

        4,      // SI8_4D,
        8,      // SI16_4D,
        16,     // SI32_4D,
        32,     // SI64_4D,

        4,      // UI8_4D,
        8,      // UI16_4D,
        16,     // UI32_4D,
        32,     // UI64_4D,

        4,      // SN8_4D,
        8,
        16,
        32,
        8,      // SF16_4D,
        16,     // SF32_4D,
        32,     // SF64_4D,

        4,      // UN8_4D,
        8,
        16,
        32,
        8,      // UF16_4D,
        16,     // UF32_4D,
        32,     // UF64_4D,

        // sRGB:
    };
} // namespace XD::Render
