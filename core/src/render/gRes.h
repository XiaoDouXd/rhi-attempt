#pragma once

#include "xdBase/uuidGen.h"

namespace XD::Render
{
    class GRes
    {
    public:
        uuids::uuid getUUID() const;
        virtual ~GRes() = default;

    protected:
        GRes();

    private:
        uuids::uuid _uuid;
    };
} // namespace XD::Render
