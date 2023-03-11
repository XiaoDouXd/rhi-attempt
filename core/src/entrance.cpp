
#include <iostream>

#include "xdBase/uuidGen.h"

namespace XD
{
    std::unique_ptr<uuids::uuid_random_generator> uuidGenerator;
} // namespace XD

int main(int argc, char** argv)
{
    if (!XD::uuidGenerator)
    {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size> {};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::mt19937 engine(seq);
        XD::uuidGenerator = std::make_unique<uuids::uuid_random_generator>(&engine);
    }

    std::cout << "program end";
    return 0;
}