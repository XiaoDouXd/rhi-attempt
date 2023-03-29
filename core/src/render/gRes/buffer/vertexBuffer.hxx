#pragma once

#ifndef XD__TEMPLATE_HXX
#include "vertexBuffer.hpp" // 这个只是为了保证语法提示的准确性...
#endif

#ifndef XD__TEMPLATE_EXTERN
#define XD__TEMPLATE_EXTERN extern
#endif

namespace XD::Render
{
    // basic layout
    XD__TEMPLATE_EXTERN template class BufferLayout<
        BufferLayoutType::SF32_3D,      // vertex pos       ... 0
        BufferLayoutType::SN8_4D        // vertex color     ... 1
    >;
    using BasicBufferLayout = BufferLayout<
        BufferLayoutType::SF32_3D,
        BufferLayoutType::SN8_4D
    >;

    // uv layout
    XD__TEMPLATE_EXTERN template class BufferLayout<
        BufferLayoutType::SF32_3D,      // vertex pos       ... 0
        BufferLayoutType::SF32_2D,      // vertex uv        ... 1
        BufferLayoutType::SN8_4D        // vertex color     ... 2
    >;
    using UvBufferLayout = BufferLayout<
        BufferLayoutType::SF32_3D,
        BufferLayoutType::SF32_2D,
        BufferLayoutType::SN8_4D
    >;

    // full layout
    XD__TEMPLATE_EXTERN template class BufferLayout<
        BufferLayoutType::SF32_3D,      // vertex pos       ... 0
        BufferLayoutType::SF32_3D,      // vertex normal    ... 1
        BufferLayoutType::SF32_3D,      // vertex tangent   ... 2
        BufferLayoutType::SF32_2D,      // vertex uv        ... 3
        BufferLayoutType::SN8_4D        // vertex color     ... 4
    >;
    using FullBufferLayout = BufferLayout<
        BufferLayoutType::SF32_3D,
        BufferLayoutType::SF32_3D,
        BufferLayoutType::SF32_3D,
        BufferLayoutType::SF32_2D,
        BufferLayoutType::SN8_4D
    >;
}