#include <mylib/greeting.hpp>

#include <upstream/message.hpp>
#include<fmt/format.h>
namespace mylib {

std::string greeting(const std::string& name)
{
    return upstream::message(name) + "!";
}

}  // namespace mylib
