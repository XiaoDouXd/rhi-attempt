#pragma once

#include "render/public/gRes/gRes.h"
#include "xdBase/blob.h"

namespace XD::Render
{
    class Shader : public GRes
    {
    public:
        Shader(const Blob& data);
        Shader(Blob&& data);
        Shader(const std::tuple<uint8_t*, size_t>& data);
        ~Shader();
    };
}