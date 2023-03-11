#pragma once

#include <memory>
#include "uuid.h"

namespace XD
{
    extern std::unique_ptr<uuids::uuid_random_generator> uuidGenerator;
} // namespace XD::Render
