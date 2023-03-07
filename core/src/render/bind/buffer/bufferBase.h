#pragma once

#include "render/bind/bindBase.h"

namespace XD::Render
{
    class BufferBase : public BindBase
    {
    public:
        virtual void release() noexcept = 0;

    protected:
        BufferBase() = default;
        virtual ~BufferBase() = default;
    };
} // namespace XD::Render
