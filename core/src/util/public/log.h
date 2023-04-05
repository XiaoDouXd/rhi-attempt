#pragma once

#include <string>

namespace XD::Log
{
    void log(const std::string_view& info);
    void logError(const std::string_view& info);
    void logWarning(const std::string_view& info);
}