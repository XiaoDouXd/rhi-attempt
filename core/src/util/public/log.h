#pragma once

#include <string>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

namespace XD::Log
{
    void log(const std::string_view& info);
    void logError(const std::string_view& info);
    void logWarning(const std::string_view& info);
}

#pragma clang diagnostic pop