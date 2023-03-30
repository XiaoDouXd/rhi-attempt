#pragma once

#include "render/gRes/gRes.h"
#include "xdBase/blob.h"

namespace XD::Render
{
    class Shader : public GRes
    {
    public:
        Shader(const Blob& data);
        ~Shader();
    };
}