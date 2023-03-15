
#include <iostream>

#include "xdBase/uuidGen.h"

namespace XD::UUID
{
    uuids::uuid gen()
    {
        std::unique_ptr<uuids::uuid_random_generator> generator = nullptr;
        if (!generator)
        {
            std::random_device rd;
            auto seed_data = std::array<int, std::mt19937::state_size> {};
            std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
            std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
            std::mt19937 engine(seq);
            generator = std::make_unique<uuids::uuid_random_generator>(&engine);
        }
        return generator->operator()();
    }
} // namespace XD

// 临时的 main 函数
int main(int argc, char** argv)
{
    std::cout << "program end";
    return 0;
}