#pragma once

#include "render/gRes/buffer/buffer.h"
#include "bufferLayout.h"
#include "render/gRes/buffer/bufferLayout.h"
#include "render/gRes/gRes.h"

namespace XD::Render
{
    class VertexBuffer : public Buffer
    {
    public:
        /// @brief 顶点缓冲
        VertexBuffer();
        ~VertexBuffer();

        /// @brief 总大小
        size_t size() const noexcept override;

    private:
        BufferLayout _layout;
    };
}