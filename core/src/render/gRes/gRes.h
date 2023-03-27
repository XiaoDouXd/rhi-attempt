#pragma once

#include <memory>
#include <stdint.h>

#include "xdBase/exce.h"
#include "xdBase/uuidGen.h"

namespace XD::Render
{
    enum class GResType : uint8_t
    {
        None,
        // --------------

        Buffer,
        BufferLayout,

        VertexShader,
        PixelShader,
        ComputeShader,
        GeometryShader,

        Tex2D,
        Tex2DArray,
        Tex3D,
        TexCube,

        Fence,
        ComputeFence,

        CmdPool,
        CmdQueue,

        Pass,
        Pipeline,

        // --------------
        Num,
    };

    class GResHolderBase { public: virtual ~GResHolderBase() = default; };
    template<GResType type> class GResHolder : public GResHolderBase {};

    class GRes
    {
    public:
        uuids::uuid getUUID() const;
        virtual ~GRes() = default;

    protected:
        GRes();
        GRes(GRes&&) = default;
        GRes& operator=(GRes&&) = default;
        GRes(const GRes&) = delete;
        const GRes& operator=(const GRes&) = delete;

        template<GResType T> static GResHolder<T>& getResHolder(std::unique_ptr<GResHolderBase>& base)
        {
            if (!base) throw Exce(__LINE__, __FILE__, "XD::Render::GRes Error: Unexpected nullptr exception.");
            return *(reinterpret_cast<GResHolder<T>*>(&(*base)));
        }
        template<GResType T> static const GResHolder<T>& getResHolder(const std::unique_ptr<GResHolderBase>& base)
        {
            if (!base) throw Exce(__LINE__, __FILE__, "XD::Render::GRes Error: Unexpected nullptr exception.");
            return *(reinterpret_cast<const GResHolder<T>*>(&(*base)));
        }

    private:
        uuids::uuid _uuid;
    };

    // ------------------------------------------ BufferTypeDef
    enum class BufferType : uint8_t
    {
        None,
        // --------------

        IndexBuffer,
        VertexBuffer,

        // --------------
        Num,
    };

    // ------------------------------------------ LayoutTypeDef

    enum class BufferLayoutRate { Instance, Vertex };
    enum class BufferLayoutType : uint8_t
    {
        None,
        // --------------

        SI8,
        SI16,
        SI32,
        SI64,

        UI8,
        UI16,
        UI32,
        UI64,

        SN8,
        SN16,
        SF16,
        SF32,
        SF64,

        UN8,
        UN16,

        SI8_2D,
        SI16_2D,
        SI32_2D,
        SI64_2D,

        UI8_2D,
        UI16_2D,
        UI32_2D,
        UI64_2D,

        SN8_2D,
        SN16_2D,
        SF16_2D,
        SF32_2D,
        SF64_2D,

        UN8_2D,
        UN16_2D,

        SI8_3D,
        SI16_3D,
        SI32_3D,
        SI64_3D,

        UI8_3D,
        UI16_3D,
        UI32_3D,
        UI64_3D,

        SN8_3D,
        SN16_3D,
        SF16_3D,
        SF32_3D,
        SF64_3D,

        UN8_3D,
        UN16_3D,

        SI8_4D,
        SI16_4D,
        SI32_4D,
        SI64_4D,

        UI8_4D,
        UI16_4D,
        UI32_4D,
        UI64_4D,

        SN8_4D,
        SN16_4D,
        SF16_4D,
        SF32_4D,
        SF64_4D,

        UN8_4D,
        UN16_4D,

        // sRGB:
        Srgb8,
        Srgb8_2D,
        Srgb8_3D,
        Srgb8_4D,

        // --------------
        Num,
    };
    extern const std::array<size_t, (size_t)BufferLayoutType::Num> BufferLayoutByte;
    inline size_t getBufferLayoutTypeByte(BufferLayoutType typ)
    {
        if (typ == BufferLayoutType::Num)
            throw Exce(__LINE__, __FILE__, "XD::Render BufferLayout: 非法的布局设置");
        return BufferLayoutByte[(size_t)typ];
    }
} // namespace XD::Render
