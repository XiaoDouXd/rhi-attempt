#pragma once

#include <glm.hpp>

#include "render/gRes/gRes.h"

namespace XD::Render
{
    class BufferLayoutBase : public GRes
    {
    public:
        static constexpr GResType gResType = GResType::BufferLayout;

    public:
        /// @brief 创建缓冲布局
        /// @param types 单元内部的类型 (location 为 0~types.size() 顺序排列)
        /// @param rate 设置缓冲单元按实例输入还是按顶点输入
        /// @param order 排序方式
        BufferLayoutBase(
            std::initializer_list<BufferLayoutType> types,
            BufferLayoutRate rate = BufferLayoutRate::Vertex);

        /// @brief 获得实现信息
        /// @return 实现信息
        const GResHolder<gResType>& getImplInfo();

        /// @brief 总大小
        size_t stride() const;

    private:
        size_t                          _stride;
        std::unique_ptr<GResHolderBase> _holder = nullptr;
    };

    template<BufferLayoutType T>
    class BufferLayoutTypeMap { public: using type = void; };

#define BufferLayoutTypeMapDef(layoutType, cpuType) \
        template<> \
        class BufferLayoutTypeMap<layoutType> { public: using type = cpuType; }

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
        static_assert(sizeof...(layouts) > 0 && sizeof...(layouts) <= 16, "XD::Render BufferLayout: 布局不能为空且大小最大为 16 个");
    private:
        template<size_t i, BufferLayoutType f, BufferLayoutType ...l>
        struct LayoutAtImpl
        { static constexpr BufferLayoutType v = LayoutAtImpl<i - 1, l...>::v; };
        template<BufferLayoutType f, BufferLayoutType ...l>
        struct LayoutAtImpl <0, f, l...>
        { static constexpr BufferLayoutType v = f; };
        template<BufferLayoutType f>
        struct LayoutAtImpl <0, f>
        { static constexpr BufferLayoutType v = f; };
        template<BufferLayoutType f, BufferLayoutType ...l>
        struct LayoutMemSizeImpl
        { static constexpr size_t v = sizeof(typename BufferLayoutTypeMap<f>::type) + LayoutMemSizeImpl<l...>::v; };
        template<BufferLayoutType f>
        struct LayoutMemSizeImpl<f>
        { static constexpr size_t v = sizeof(typename BufferLayoutTypeMap<f>::type); };

        template<size_t s, size_t ...ValSums, uint8_t f, uint8_t ...Vals>
        static constexpr auto calcOffsetImpl(std::integer_sequence<size_t, ValSums...>, std::integer_sequence<uint8_t, f, Vals...>)
        {
            if constexpr (sizeof...(Vals))
            {
                return calcOffsetImpl<s + sizeof(typename BufferLayoutTypeMap<(BufferLayoutType)f>::type)>(
                    std::integer_sequence<size_t,
                        ValSums..., s + sizeof(typename BufferLayoutTypeMap<(BufferLayoutType)f>::type)>{},
                    std::integer_sequence<uint8_t, Vals...>{});
            }
            else
            {
                return std::integer_sequence<size_t,
                    ValSums..., s + sizeof(typename BufferLayoutTypeMap<(BufferLayoutType)f>::type)>{};
            }
        }

        template<uint8_t...Vals>
        static constexpr uuids::uuid getUuidImpl(std::integer_sequence<uint8_t, Vals...>)
        { return uuids::uuid(std::array<uint8_t, 16>{Vals...}); }

        template<uint8_t...Vals>
        static constexpr auto getInitListImpl(std::integer_sequence<uint8_t, Vals...>)
        { return std::initializer_list<BufferLayoutType>{(BufferLayoutType)Vals...}; }

        template<size_t i, size_t f, size_t...l>
        struct GetOffsetImpl
        { static constexpr size_t v = GetOffsetImpl<i - 1, l...>::v; };
        template<size_t f, size_t ...l>
        struct GetOffsetImpl<0, f, l...>
        { static constexpr size_t v = f; };
        template<size_t f>
        struct GetOffsetImpl<0, f>
        { static constexpr size_t v = f; };
        template<size_t i, size_t ...l>
        static constexpr auto getOffsetImpl(std::integer_sequence<size_t, l...>)
        { return GetOffsetImpl<i, l...>::v; }

    public:
        static constexpr std::size_t size = sizeof...(layouts);
        static constexpr std::size_t memSize = LayoutMemSizeImpl<layouts...>::v;
        using __xd_is_buffer_layout = std::true_type;
        using __xd_format_list = std::integer_sequence<uint8_t, (uint8_t)layouts...>;
        using __xd_offset_list = decltype(
            calcOffsetImpl<0>(std::integer_sequence<size_t>{}, __xd_format_list{}));

        template<size_t i>
        struct LayoutAt
        {
            static_assert(i < size, "XD::BufferLayout Error: 索引越界");
            static constexpr BufferLayoutType value = LayoutAtImpl<i, layouts...>::v;
        };
        template<size_t i>
        static constexpr size_t getOffset()
        {
            static_assert(i < size, "XD::BufferLayout Error: 索引越界");
            if constexpr (i) return getOffsetImpl<i - 1>(__xd_offset_list{});
            else return 0;
        }
        static constexpr uuids::uuid getLayoutUUID()
        { return getUuidImpl(std::integer_sequence<uint8_t, (uint8_t)layouts...>{}); }
        static BufferLayoutBase getLayout()
        { return BufferLayoutBase(getInitListImpl(std::integer_sequence<uint8_t, (uint8_t)layouts...>{})); }
    };
}