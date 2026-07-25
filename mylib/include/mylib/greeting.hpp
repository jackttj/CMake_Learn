#pragma once

#include <string>

namespace mylib {

std::string greeting(const std::string& name);


std::string formatted_greeting(
    const std::string& name,
    int visit_count);
}  // namespace mylib
