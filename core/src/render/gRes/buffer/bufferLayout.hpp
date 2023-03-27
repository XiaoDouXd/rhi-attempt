#pragma once

#include <glm.hpp>

#include "render/gRes/gRes.h"

namespace XD::Render
{
    template<BufferLayoutType T>
    class BufferLayoutTypeMap { using type = void; };

#define BufferLayoutTypeMapDef(layoutType, cpuType) \
        template<> \
        class BufferLayoutTypeMap<layoutType> { using type = cpuType; }

    BufferLayoutTypeMapDef(BufferLayoutType::SI8, glm::int8);
    BufferLayoutTypeMapDef(BufferLayoutType::SI16, glm::int16);
    BufferLayoutTypeMapDef(BufferLayoutType::SI32, glm::int32);
    BufferLayoutTypeMapDef(BufferLayoutType::SI64, glm::int64);

    BufferLayoutTypeMapDef(BufferLayoutType::UI8, glm::uint8);
    BufferLayoutTypeMapDef(BufferLayoutType::UI16, glm::uint16);
    BufferLayoutTypeMapDef(BufferLayoutType::UI32, glm::uint32);
    BufferLayoutTypeMapDef(BufferLayoutType::UI64, glm::uint64);

    BufferLayoutTypeMapDef(BufferLayoutType::SN8, glm::i8);
    BufferLayoutTypeMapDef(BufferLayoutType::SN16, glm::i16);
    BufferLayoutTypeMapDef(BufferLayoutType::SF16, glm::float32);
    BufferLayoutTypeMapDef(BufferLayoutType::SF32, glm::float32);
    BufferLayoutTypeMapDef(BufferLayoutType::SF64, glm::float64);

    BufferLayoutTypeMapDef(BufferLayoutType::UN8, glm::u8);
    BufferLayoutTypeMapDef(BufferLayoutType::UN16, glm::u16);

    BufferLayoutTypeMapDef(BufferLayoutType::SI8_2D, glm::i8vec2);
    BufferLayoutTypeMapDef(BufferLayoutType::SI16_2D, glm::i16vec2);
    BufferLayoutTypeMapDef(BufferLayoutType::SI32_2D, glm::i32vec2);
    BufferLayoutTypeMapDef(BufferLayoutType::SI64_2D, glm::i64vec2);

    BufferLayoutTypeMapDef(BufferLayoutType::UI8_2D, glm::u8vec2);
    BufferLayoutTypeMapDef(BufferLayoutType::UI16_2D, glm::u16vec2);
    BufferLayoutTypeMapDef(BufferLayoutType::UI32_2D, glm::u32vec2);
    BufferLayoutTypeMapDef(BufferLayoutType::UI64_2D, glm::u64vec2);

    BufferLayoutTypeMapDef(BufferLayoutType::SN8_2D, glm::i8vec2);
    BufferLayoutTypeMapDef(BufferLayoutType::SN16_2D, glm::i16vec2);
    BufferLayoutTypeMapDef(BufferLayoutType::SF16_2D, glm::f32vec2);
    BufferLayoutTypeMapDef(BufferLayoutType::SF32_2D, glm::f32vec2);
    BufferLayoutTypeMapDef(BufferLayoutType::SF64_2D, glm::f64vec2);

    BufferLayoutTypeMapDef(BufferLayoutType::UN8_2D, glm::u8vec2);
    BufferLayoutTypeMapDef(BufferLayoutType::UN16_2D, glm::u16vec2);

    BufferLayoutTypeMapDef(BufferLayoutType::SI8_3D, glm::i8vec3);
    BufferLayoutTypeMapDef(BufferLayoutType::SI16_3D, glm::i16vec3);
    BufferLayoutTypeMapDef(BufferLayoutType::SI32_3D, glm::i32vec3);
    BufferLayoutTypeMapDef(BufferLayoutType::SI64_3D, glm::i64vec3);

    BufferLayoutTypeMapDef(BufferLayoutType::UI8_3D, glm::u8vec3);
    BufferLayoutTypeMapDef(BufferLayoutType::UI16_3D, glm::u16vec3);
    BufferLayoutTypeMapDef(BufferLayoutType::UI32_3D, glm::u32vec3);
    BufferLayoutTypeMapDef(BufferLayoutType::UI64_3D, glm::u64vec3);

    BufferLayoutTypeMapDef(BufferLayoutType::SN8_3D, glm::i8vec3);
    BufferLayoutTypeMapDef(BufferLayoutType::SN16_3D, glm::i16vec3);
    BufferLayoutTypeMapDef(BufferLayoutType::SF16_3D, glm::f32vec3);
    BufferLayoutTypeMapDef(BufferLayoutType::SF32_3D, glm::f32vec3);
    BufferLayoutTypeMapDef(BufferLayoutType::SF64_3D, glm::f64vec3);

    BufferLayoutTypeMapDef(BufferLayoutType::UN8_3D, glm::u8vec3);
    BufferLayoutTypeMapDef(BufferLayoutType::UN16_3D, glm::u16vec3);

    BufferLayoutTypeMapDef(BufferLayoutType::SI8_4D, glm::i8vec4);
    BufferLayoutTypeMapDef(BufferLayoutType::SI16_4D, glm::i16vec4);
    BufferLayoutTypeMapDef(BufferLayoutType::SI32_4D, glm::i32vec4);
    BufferLayoutTypeMapDef(BufferLayoutType::SI64_4D, glm::i64vec4);

    BufferLayoutTypeMapDef(BufferLayoutType::UI8_4D, glm::u8vec4);
    BufferLayoutTypeMapDef(BufferLayoutType::UI16_4D, glm::u16vec4);
    BufferLayoutTypeMapDef(BufferLayoutType::UI32_4D, glm::u32vec4);
    BufferLayoutTypeMapDef(BufferLayoutType::UI64_4D, glm::u64vec4);

    BufferLayoutTypeMapDef(BufferLayoutType::SN8_4D, glm::i8vec4);
    BufferLayoutTypeMapDef(BufferLayoutType::SN16_4D, glm::i16vec4);
    BufferLayoutTypeMapDef(BufferLayoutType::SF16_4D, glm::f32vec4);
    BufferLayoutTypeMapDef(BufferLayoutType::SF32_4D, glm::f32vec4);
    BufferLayoutTypeMapDef(BufferLayoutType::SF64_4D, glm::f64vec4);

    BufferLayoutTypeMapDef(BufferLayoutType::UN8_4D, glm::u8vec4);
    BufferLayoutTypeMapDef(BufferLayoutType::UN16_4D, glm::u16vec4);

    BufferLayoutTypeMapDef(BufferLayoutType::Srgb8, glm::u8);
    BufferLayoutTypeMapDef(BufferLayoutType::Srgb8_2D, glm::u8vec2);
    BufferLayoutTypeMapDef(BufferLayoutType::Srgb8_3D, glm::u8vec3);
    BufferLayoutTypeMapDef(BufferLayoutType::Srgb8_4D, glm::u8vec4);

#undef BufferLayoutTypeMapDef
    template<BufferLayoutType typ>
    inline size_t constexpr getBufferLayoutTypeByte()
    {
        if constexpr (std::is_same<BufferLayoutTypeMap<typ>, void>::value)
            throw Exce(__LINE__, __FILE__, "XD::Render BufferLayout: 非法的布局设置");
        return sizeof(BufferLayoutTypeMap<typ>::type);
    }

    template <BufferLayoutType ...layouts>
    class BufferLayout
    {
    private:
        template<size_t i, BufferLayoutType f, BufferLayoutType ...l>
        struct LayoutAtImpl
        { static constexpr BufferLayoutType v = LayoutAtImpl<i - 1, f, l...>::v; };
        template<BufferLayoutType f, BufferLayoutType ...l>
        struct LayoutAtImpl <0, f, l...>
        { static constexpr BufferLayoutType v = f; };
        template<BufferLayoutType f>
        struct LayoutAtImpl <0, f>
        { static constexpr BufferLayoutType v = f; };

        template<BufferLayoutType...Vals>
        static constexpr auto calcOffset(std::integer_sequence<BufferLayoutType, Vals...>)
        {
            std::integer_sequence<size_t, (0 + ... + sizeof(BufferLayoutTypeMap<Vals>::type))> res;
            return res;
        }

        template<BufferLayoutType...Vals>
        static constexpr size_t memorySizeImpl(std::integer_sequence<BufferLayoutType, Vals...>)
        {
            return (0 + ... + sizeof(BufferLayoutTypeMap<Vals>::type));
        }

    public:
        static constexpr std::size_t size = sizeof...(layouts);
        using __xd_is_buffer_layout = std::true_type;
        using __xd_format_list = std::integer_sequence<BufferLayoutType, layouts...>;
        using __xd_offset_list = decltype(calcOffset(__xd_format_list{}));

        template<size_t i>
        struct LayoutAt
        { static constexpr BufferLayoutType value = LayoutAtImpl<i, layouts...>::v; };
        static constexpr size_t memorySize() { return memorySizeImpl(__xd_format_list{}); }
    };
}