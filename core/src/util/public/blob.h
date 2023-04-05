#pragma once

#include <cstdint>
#include <memory>
#include <fstream>

template class std::tuple<uint8_t*, size_t>;

namespace XD
{
    class Blob final
    {
    public:
        Blob() {}
        Blob(const char* path)
        {
            std::ifstream f(path, std::ios::binary | std::ios::ate);
            if (!f.is_open()) return;
            _size = f.tellg();
            _data = new uint8_t[_size];
            f.seekg(0, std::ios::beg);
            if (!f.read(reinterpret_cast<char*>(_data), _size))
            { delete [] _data; _data = nullptr; _size = 0; }
        }
        Blob(uint8_t* data, size_t size): _data(data), _size(size) {}
        Blob(size_t size) : _data(size ? new uint8_t[size] : nullptr), _size(size) {}
        Blob(Blob&& o) : _data(o._data), _size(o._size) { o._data = nullptr; o._size = 0; }
        Blob(const Blob& o) : _data(o._data ? new uint8_t[o._size] : 0), _size(o._size)
        { if (_size) memcpy(_data, o._data, o._size); }
        ~Blob() { delete [] _data; _data = nullptr; }

        const Blob& operator=(Blob&& o)
        {
            delete [] _data;
            _data = o._data;
            _size = o._size;
            o._data = nullptr;
            o._size = 0;
            return *this;
        }

        const Blob& operator=(const Blob& o)
        {
            if (o._size != _size)
            {
                delete [] _data;
                if (!o._data) { _size = 0; _data = nullptr; return *this; }
                _data = new uint8_t[o._size];
            }
            _size = o._size;
            memcpy(_data, o._data, o._size);
            return *this;
        }

        uint8_t* data() const { return _data; }
        size_t size() const { return _size; }

        void swap(Blob& o)
        {
            auto tmpD = o._data;
            o._data = _data;
            _data = tmpD;

            auto tmpS = o._size;
            o._size = _size;
            _size = tmpS;
        }

        void swap(std::tuple<uint8_t*, size_t>& data)
        {
            auto tmpD = _data;
            _data = std::get<0>(data);
            auto tmpS = _size;
            _size = std::get<1>(data);
            data = {tmpD, tmpS};
        }

        std::tuple<uint8_t*, size_t> move()
        {
            auto data = std::tuple<uint8_t*, size_t>{_data, _size};
            _data = nullptr;
            _size = 0;
            return data;
        }

        operator bool() { return (bool)_data; }

    private:
        uint8_t*    _data = nullptr;
        size_t      _size = 0;
    };
};