#pragma once

#include "xdBase/uuidGen.h"
#include <memory>
#include <stdint.h>

namespace XD::Render
{
    enum class GResType : uint8_t
    {
        None,
        // --------------

        // buffer
        IndexBuffer,

        // shader

        // --------------
        Num,
    };

    class GResMemHolderBase { public: virtual ~GResMemHolderBase() = default; };
    template<GResType type> class GResMemHolder : public GResMemHolderBase {};

    class GRes
    {
    public:
        uuids::uuid getUUID() const;
        virtual ~GRes() = default;

    protected:
        GRes();
        template<GResType T> static GResMemHolder<T>& GetMemHolder(std::unique_ptr<GResMemHolderBase>& base)
        {
            if (!base) throw Exce(__LINE__, __FILE__, "XD::Render::GRes Error: Unexpected nullptr exception.");
            return *(dynamic_cast<GResMemHolder<T>*>(&(*base)));
        }

    private:
        uuids::uuid _uuid;
    };
} // namespace XD::Render
