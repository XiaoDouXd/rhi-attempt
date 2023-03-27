#include <memory>

#include "gRes.h"
#include "xdBase/exce.h"

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

        1,      // SN8,     // glm::i8
        2,      // SN16,    // glm::i16
        4,      // SF16,    // glm::f32
        4,      // SF32,
        8,      // SF64,

        1,      // UN8,     // glm::u8
        2,      // UN16,    // glm::u16

        2,      // SI8_2D,
        4,      // SI16_2D,
        8,      // SI32_2D,
        16,     // SI64_2D,

        2,      // UI8_2D,
        4,      // UI16_2D,
        8,      // UI32_2D,
        16,     // UI64_2D,

        2,      // SN8_2D,      // glm::i8vec2
        4,      // SN16_2D,     // glm::i16vec2
        8,      // SF16_2D,     // glm::f32vec2
        8,      // SF32_2D,
        16,     // SF64_2D,

        2,      // UN8_2D,      // glm::u8vec2
        4,      // UN16_2D,     // glm::u16vec2

        3,      // SI8_3D,
        6,      // SI16_3D,
        12,     // SI32_3D,
        24,     // SI64_3D,

        3,      // UI8_3D,
        6,      // UI16_3D,
        12,     // UI32_3D,
        24,     // UI64_3D,

        3,      // SN8_3D,      // glm::i8vec3
        6,      // SN16_3D      // glm::i16vec3
        12,     // SF16_3D,     // glm::f32vec3
        12,     // SF32_3D,
        24,     // SF64_3D,

        3,      // UN8_3D,      // glm::u8vec3
        6,      // UN16_3D,     // glm::u16vec3

        4,      // SI8_4D,
        8,      // SI16_4D,
        16,     // SI32_4D,
        32,     // SI64_4D,

        4,      // UI8_4D,
        8,      // UI16_4D,
        16,     // UI32_4D,
        32,     // UI64_4D,

        4,      // SN8_4D,      // glm::i8vec4
        8,      // SN16_4D,     // glm::i16vec4
        16,     // SF16_4D,     // glm::f32vec4
        16,     // SF32_4D,
        32,     // SF64_4D,

        4,      // UN8_4D,      // glm::u8vec4
        8,      // UN16_4D,     // glm::u16vec4

        // sRGB:
        1,      // Srgb8,       // glm::u8
        2,      // Srgb8_2D,    // glm::u8vec2
        3,      // Srgb8_3D,    // glm::u8vec3
        4,      // Srgb8_4D,    // glm::u8vec4
    };
} // namespace XD::Render
