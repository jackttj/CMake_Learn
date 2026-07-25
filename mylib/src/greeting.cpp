#include <mylib/greeting.hpp>

#include <fmt/format.h>
#include <upstream/message.hpp>

namespace mylib {

std::string greeting(const std::string& name)
{
    return upstream::message(name) + "!";
}

std::string formatted_greeting(
    const std::string& name,
    int visit_count)
{
    return fmt::format(
        "{}! This is your visit number {}.",
        upstream::message(name),
        visit_count);
}

}  // namespace mylib