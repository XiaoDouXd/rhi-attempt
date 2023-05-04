#include <memory>
#include <vector>

#include "bufferLayoutFormat.h"
#include "render/public/gRes/buffer/bufferLayout.hpp"
#include "render/public/gRes/gRes.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

namespace XD::Render
{
    template<>
    class GResHolder<GResType::BufferLayout> : public GResHolderBase
    {
    public:
        std::vector<vk::VertexInputAttributeDescription> attrs;
        vk::VertexInputBindingDescription binding;
    };

    const GResHolder<BufferLayoutBase::gResType>& BufferLayoutBase::getImplInfo()
    {
        return getResHolder<gResType>(_holder);
    }

    BufferLayoutBase::BufferLayoutBase(std::initializer_list<Format> types, BufferLayoutRate rate)
    {
        _holder = std::make_unique<GResHolder<BufferLayoutBase::gResType>>();
        auto& holder = getResHolder<gResType>(_holder);
        holder.attrs.resize(types.size());

        size_t idx = _stride = 0;
        for (auto typ : types)
        {
            holder.attrs[idx]   .setFormat(VkFormatMap[(size_t)typ])
                                .setOffset(_stride)
                                .setLocation(idx)
                                .setBinding(0);

            _stride += getBufferLayoutTypeByte(typ); idx++;
        }

        holder.binding.setStride(_stride).setInputRate(
            rate == BufferLayoutRate::Instance ?
                vk::VertexInputRate::eInstance :
                vk::VertexInputRate::eVertex
        ).setBinding(0);
    }

    size_t BufferLayoutBase::stride() const { return _stride; }

    const std::array<vk::Format, (size_t)Format::Num> VkFormatMap =
    {
        vk::Format::eUndefined,     // None
        vk::Format::eR8Sint,        // SI8,
        vk::Format::eR16Sint,       // SI16,
        vk::Format::eR32Sint,       // SI32,
        vk::Format::eR64Sint,       // SI64,

        vk::Format::eR8Uint,        // UI8,
        vk::Format::eR16Uint,       // UI16,
        vk::Format::eR32Uint,       // UI32,
        vk::Format::eR64Uint,       // UI64,

        vk::Format::eR8Unorm,       // SN8,
        vk::Format::eR16Unorm,      // SN16,
        vk::Format::eR16Sfloat,     // SF16,
        vk::Format::eR32Sfloat,     // SF32,
        vk::Format::eR64Sfloat,     // SF64,

        vk::Format::eR8Unorm,       // UN8,
        vk::Format::eR16Unorm,      // UN16,

        vk::Format::eR8G8Sint,      // SI8_2D,
        vk::Format::eR16G16Sint,    // SI16_2D,
        vk::Format::eR32G32Sint,    // SI32_2D,
        vk::Format::eR64G64Sint,    // SI64_2D,

        vk::Format::eR8G8Uint,      // UI8_2D,
        vk::Format::eR16G16Uint,    // UI16_2D,
        vk::Format::eR32G32Uint,    // UI32_2D,
        vk::Format::eR64G64Uint,    // UI64_2D,

        vk::Format::eR8G8Snorm,     // SN8_2D,
        vk::Format::eR16G16Snorm,   // SN16_2D,
        vk::Format::eR16G16Sfloat,  // SF16_2D,
        vk::Format::eR32G32Sfloat,  // SF32_2D,
        vk::Format::eR64G64Sfloat,  // SF64_2D,

        vk::Format::eR8G8Unorm,     // UN8_2D,
        vk::Format::eR16G16Unorm,   // UN16_2D,

        vk::Format::eR8G8B8Sint,            // SI8_3D,
        vk::Format::eR16G16B16Sint,         // SI16_3D,
        vk::Format::eR32G32B32Sint,         // SI32_3D,
        vk::Format::eR64G64B64Sint,         // SI64_3D,

        vk::Format::eR8G8B8Uint,            // UI8_3D,
        vk::Format::eR16G16B16Uint,         // UI16_3D,
        vk::Format::eR32G32B32Uint,         // UI32_3D,
        vk::Format::eR64G64B64Uint,         // UI64_3D,

        vk::Format::eR8G8B8Snorm,           // SN8_3D,
        vk::Format::eR16G16B16Snorm,        // SN16_3D,
        vk::Format::eR16G16B16Sfloat,       // SF16_3D,
        vk::Format::eR32G32B32Sfloat,       // SF32_3D,
        vk::Format::eR64G64B64Sfloat,       // SF64_3D,

        vk::Format::eR8G8B8Unorm,           // UN8_3D,
        vk::Format::eR16G16B16Unorm,        // UN16_3D,

        vk::Format::eR8G8B8A8Sint,          // SI8_4D,
        vk::Format::eR16G16B16A16Sint,      // SI16_4D,
        vk::Format::eR32G32B32A32Sint,      // SI32_4D,
        vk::Format::eR64G64B64A64Sint,      // SI64_4D,

        vk::Format::eR8G8B8A8Uint,          // UI8_4D,
        vk::Format::eR16G16B16A16Uint,      // UI16_4D,
        vk::Format::eR32G32B32A32Uint,      // UI32_4D,
        vk::Format::eR64G64B64A64Uint,      // UI64_4D,

        vk::Format::eR8G8B8A8Snorm,         // SN8_4D,
        vk::Format::eR16G16B16A16Snorm,     // SN16_4D,
        vk::Format::eR16G16B16A16Sfloat,    // SF16_4D,
        vk::Format::eR32G32B32A32Sfloat,    // SF32_4D,
        vk::Format::eR64G64B64A64Sfloat,    // SF64_4D,

        vk::Format::eR8G8B8A8Unorm,         // UN8_4D,
        vk::Format::eR16G16B16A16Unorm,     // UN16_4D,

        // sRGB:
        vk::Format::eR8Srgb,                // Srgb8,
        vk::Format::eR8G8Srgb,              // USrgb8_2D,
        vk::Format::eR8G8B8Srgb,            // USrgb8_3D,
        vk::Format::eR8G8B8A8Srgb,          // USrgb8_4D,
    };
}
#pragma clang diagnostic pop