#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "render/gRes/buffer/bufferLayout.h"
#include "render/gRes/gRes.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"
#include "xdBase/exce.h"

namespace XD::Render
{
    const std::array VkFormatMap =
    {
        vk::Format::eUndefined,     // None
    };

    template<>
    class GResHolder<GResType::BufferLayout> : public GResHolderBase
    {
    public:
        std::vector<vk::VertexInputAttributeDescription> attrs;
        vk::VertexInputBindingDescription binding;
    };

    const GResHolder<BufferLayout::gResType>& BufferLayout::getImplInfo()
    {
        return getResHolder<gResType>(_holder);
    }

    BufferLayout::BufferLayout(std::initializer_list<BufferLayoutType> typs, BufferLayoutRate rate)
    {
        _holder = std::make_unique<GResHolder<BufferLayout::gResType>>();
        auto& holder = getResHolder<gResType>(_holder);
        holder.attrs.resize(typs.size());

        size_t idx = _stride = 0;
        for (auto typ : typs)
        {
            vk::VertexInputAttributeDescription attr;
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

    size_t BufferLayout::stride() const { return _stride; }
}