#pragma once

#include <tuple>
#include <type_traits>

#include "render/gRes/buffer/buffer.h"
#include "bufferLayout.h"
#include "bufferLayout.hpp"
#include "render/gRes/buffer/bufferLayout.h"
#include "render/gRes/gRes.h"

namespace XD::Render
{
    class VertexBufferBase : public Buffer
    {
    public:
        VertexBufferBase(std::initializer_list<BufferLayoutType> types, uint8_t* data, size_t size, bool isDynamic = false);
        VertexBufferBase(const BufferLayoutBase& layout, uint8_t* data, size_t size, bool isDynamic);
        ~VertexBufferBase();

        /// @brief 更新顶点缓冲
        bool update();

        /// @brief 总大小
        size_t size() const noexcept override;

    private:
        bool                _isDynamic;
        BufferLayoutBase    _layout;
    };

    template<typename layout>
    requires layout::__xd_is_buffer_layout::value
    class Vertex
    {
    public:
        Vertex() { _data = new uint8_t[layout::memorySize()](); }
        ~Vertex() { delete[] _data; }

        template<size_t i>
        typename BufferLayoutTypeMap<layout::template LayoutAt<i>::value>::type get()
        {
            size_t offset = std::get<i>(typename layout::__xd_offset_list{});
            return *reinterpret_cast<typename BufferLayoutTypeMap<layout::template LayoutAt<i>::value>::type*>(
                _data + offset);
        }

        template<size_t i>
        void get(typename BufferLayoutTypeMap<layout::template LayoutAt<i>::value>::type value)
        {
            size_t offset = std::get<i>(typename layout::__xd_offset_list{});
            auto ptr = reinterpret_cast<typename BufferLayoutTypeMap<layout::template LayoutAt<i>::value>::type*>(
                _data + offset);
            (*ptr) = value;
        }

    private:
        uint8_t* _data = nullptr;
    };

    template<typename layout>
    requires layout::__xd_is_buffer_layout::value
    class VertexBuffer
    {
    public:

    private:
        static std::initializer_list<BufferLayoutType> getInitList()
        {
            typename layout::__xd_format_list seq;
            return std::initializer_list<BufferLayoutType>{seq};
        }
    };
}