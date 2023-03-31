#pragma once

#include "render/public/gRes/buffer/buffer.h"
#include "render/public/gRes/gRes.h"
#include "xdBase/blob.h"

namespace XD::Render
{
    class Texture : public GRes
    {
    public:
        static constexpr GResType gResType = GResType::Tex;
    public:
        Texture(const uuids::uuid devId = uuids::uuid()) : _devId(devId) {}
        ~Texture();

    protected:
        uuids::uuid                         _devId;
        std::unique_ptr<GResHolderBase>     _holder;
    };

    class TextureBuffer : public GRes
    {
    public:
        TextureBuffer();
        ~TextureBuffer();
    };

    class TextureTransitBuffer : public Buffer
    {
    public:
        
    };
}