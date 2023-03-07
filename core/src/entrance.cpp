
#include <iostream>

#include "xdBase/uuidGen.h"

namespace XD
{
    uuids::uuid_random_generator uuidGenerator;
} // namespace XD

int main(int argc, char** argv)
{
    std::cout << "program end";
    return 0;
}