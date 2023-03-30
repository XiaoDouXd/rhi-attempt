#pragma once

#include <cstdint>
#include <memory>
#include <fstream>

namespace XD
{
    class Blob final
    {
    public:
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
        Blob(size_t size) : _data(new uint8_t[size]), _size(size) {}
        Blob(Blob&& o) : _data(o._data), _size(o._size) { o._data = nullptr; }
        Blob(const Blob& o) : _data(new uint8_t[o._size]), _size(o._size)
        { memcpy_s(_data, _size, o._data, o._size); }
        ~Blob() { delete [] _data; _data = nullptr; }

        const Blob& operator=(Blob&& o)
        { delete [] _data; _data = o._data; _size = o._size; o._data = nullptr; return *this; }
        const Blob& operator=(const Blob& o)
        {
            if (o._size != _size) { delete [] _data; _data = new uint8_t[o._size]; }
            _size = o._size;
            memcpy_s(_data, _size, o._data, o._size);
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
        uint8_t* swap(uint8_t* data = nullptr, size_t size = 0)
        { auto tmp = _data; _data = data; _size = size; return tmp; }
        operator bool() { return (bool)_data; }

    private:
        uint8_t*    _data = nullptr;
        size_t      _size = 0;
    };
};