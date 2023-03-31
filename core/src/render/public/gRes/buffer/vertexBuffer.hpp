#pragma once

#include <span>
#include <tuple>
#include <type_traits>
#include <utility>

#include "render/public/gRes/buffer/buffer.h"
#include "render/public/gRes/buffer/bufferLayout.hpp"
#include "render/public/gRes/gRes.h"

namespace XD::Render
{
    class VertexBufferBase : public Buffer
    {
    public:
        VertexBufferBase(const std::vector<uint8_t>& data, bool isDynamic, const uuids::uuid& devId = uuids::uuid());
        VertexBufferBase(const std::span<uint8_t>& data, bool isDynamic, const uuids::uuid& devId = uuids::uuid());
        VertexBufferBase(const uint8_t* data, size_t size, bool isDynamic, const uuids::uuid& devId = uuids::uuid());
        ~VertexBufferBase();

        bool update(const std::vector<uint8_t>& data);
        bool update(const std::span<uint8_t>& data);
        bool update(const uint8_t* data, size_t size);

        /// @brief 总大小
        size_t size() const noexcept override { return _size; }
        bool isDynamic() const noexcept { return _isDynamic; }

    private:
        bool _isDynamic;
    };

    template<typename layout>
    requires layout::__xd_is_buffer_layout::value
    class Vertex final
    {
    private:
        template<size_t i>
        struct GetTypeImpl
        { using type = typename BufferLayoutTypeMap<layout::template LayoutAt<i>::value>::type; };

    public:
        using bufferLayout = layout;
        static constexpr std::size_t memSize = layout::memSize;

    public:
        Vertex(const std::vector<uint8_t>::iterator& itr) : _data(itr, itr + memSize) {}
        Vertex(const std::span<uint8_t>& data) : _data(data) {}
        Vertex(uint8_t* data) : _data(data, data ? memSize : 0) {}
        Vertex() : _data(new uint8_t[memSize](), memSize), _isSelfHold(true) {}
        Vertex(Vertex&& o) : _data(o._data), _isSelfHold(o._isSelfHold) { o._data = std::span<uint8_t>(); }
        Vertex(const Vertex& o) : _data(new uint8_t[memSize](), memSize), _isSelfHold(true)
        { for (size_t i = 0; i < memSize; i++) _data[i] = o._data[i]; }
        ~Vertex() { if (!_isSelfHold) return; delete[] _data.data(); }

        const Vertex& operator=(const Vertex& o)
        { for (size_t i = 0; i < memSize; i++) _data[i] = o._data[i]; return *this; }
        const Vertex& operator=(Vertex&& o)
        {
            if (_isSelfHold)
            {
                delete[] _data.data();
                _data = o._data;
                _isSelfHold = o._isSelfHold;
                o._data = std::span<uint8_t>();
            }
            else for (size_t i = 0; i < memSize; i++) _data[i] = o._data[i];
            return *this;
        }
        operator bool() const
        { return _data.empty(); }

        template<size_t i>
        auto get() const
        {
            return *reinterpret_cast<
                typename GetTypeImpl<i>::type*>(
                _data.data() + layout::template getOffset<i>());
        }

        template<size_t i>
        void set(typename GetTypeImpl<i>::type value)
        {
            auto ptr = reinterpret_cast<
                typename GetTypeImpl<i>::type*>(
                _data.data() + layout::template getOffset<i>());
            (*ptr) = value;
        }

        /// @brief 绑定到数组
        /// @param itr 起始迭代器
        /// @return 下一个起始迭代器
        std::vector<uint8_t>::iterator bind(const std::vector<uint8_t>::iterator& itr) noexcept
        {
            if (_isSelfHold) { delete[] _data.data(); _isSelfHold = false; }
            _data = std::span<uint8_t>(itr, itr + memSize);
            return itr + memSize;
        }

        void bind(const std::span<uint8_t>& span) noexcept
        {
            if (_isSelfHold) { delete[] _data.data(); _isSelfHold = false; }
            _data = span;
        }

        void bind(uint8_t* data) noexcept
        {
            if (_isSelfHold) { delete[] _data.data(); _isSelfHold = false; }
            _data = std::span<uint8_t>(data, data ? memSize : 0);
        }

        void unbind()
        {
            if (_isSelfHold)
                throw Exce(__LINE__, __FILE__, "XD::Render::Vertex Error: 解绑无绑定对象.");
            _data = std::span<uint8_t>();
        }

        const std::span<uint8_t>& data() const { return _data; }
        const bool isNil() const { return _data.empty(); }

    private:
        std::span<uint8_t> _data;
        bool _isSelfHold = false;
    };

    template<typename layout>
    requires layout::__xd_is_buffer_layout::value
    class VertexBuffer final : public VertexBufferBase
    {
    public:
        using bufferLayout = layout;
        VertexBuffer(const std::vector<Vertex<layout>>& data, bool isDynamic = false, const uuids::uuid& devId = uuids::uuid())
        : VertexBufferBase(toData(data), isDynamic){}
        VertexBuffer(const std::list<Vertex<layout>>& data, bool isDynamic = false, const uuids::uuid& devId = uuids::uuid())
        : VertexBufferBase(toData(data), isDynamic){}
        VertexBuffer(const std::span<Vertex<layout>>& data, bool isDynamic = false, const uuids::uuid& devId = uuids::uuid())
        : VertexBufferBase(toData(data), isDynamic){}

        VertexBuffer(const std::vector<uint8_t>& data, bool isDynamic = false, const uuids::uuid& devId = uuids::uuid())
        : VertexBufferBase(data, isDynamic) {}
        VertexBuffer(const std::span<uint8_t>& data, bool isDynamic = false, const uuids::uuid& devId = uuids::uuid())
        : VertexBufferBase(data, isDynamic) {}
        VertexBuffer(uint8_t* data, size_t size, bool isDynamic = false, const uuids::uuid& devId = uuids::uuid())
        : VertexBufferBase(data, size, isDynamic) {}

    private:
        template<typename T>
        static std::vector<uint8_t> toData(const T& data)
        {
            auto d = std::vector<uint8_t>();
            d.reserve(data.size() * Vertex<layout>::memSize);
            for (const auto& i : data) for (const auto& j : i.data())
                d.emplace_back(j);
            return d;
        }
    };
}

/// 显示实例化
#ifndef XD__TEMPLATE_INSTANTIATIONS
#define XD__TEMPLATE_EXTERN extern
#else
#define XD__TEMPLATE_EXTERN
#endif

#define XD__TEMPLATE_HXX
#include "render/private/gRes/buffer/vertexBuffer.hxx"
#undef XD__TEMPLATE_HXX
#undef XD__TEMPLATE_EXTERN