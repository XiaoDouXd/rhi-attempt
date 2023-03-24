#pragma once

#include "render/gRes/gRes.h"
#include <memory>
#include <stdint.h>

namespace XD::Render
{
    class Buffer : public GRes
    {
    public:
        virtual size_t size() const = 0;
        virtual size_t stride() const = 0;
        virtual uint32_t usage() const = 0;
        virtual void release() noexcept = 0;

    protected:
        Buffer() = default;
        virtual ~Buffer() = default;
        std::unique_ptr<GResMemHolderBase> _memHolder;
    };
} // namespace XD::Render
