#include <mylib/greeting.hpp>

#include <fmt/format.h>
#include <upstream/message.hpp>

#include <ctime>
#include <mutex>
#include <stdexcept>

namespace {

std::tm current_local_time()
{
    const std::time_t now = std::time(nullptr);
    std::tm local_time = {};

#if defined(_MSC_VER)
    if (localtime_s(&local_time, &now) != 0) {
        throw std::runtime_error("failed to convert current time");
    }
#elif defined(__unix__) || defined(__APPLE__)
    if (localtime_r(&now, &local_time) == nullptr) {
        throw std::runtime_error("failed to convert current time");
    }
#else
    // C++11 does not provide a portable thread-safe localtime function.
    // Serialize the fallback for older toolchains such as legacy MinGW.
    static std::mutex localtime_mutex;
    const std::lock_guard<std::mutex> lock(localtime_mutex);
    const std::tm* converted = std::localtime(&now);
    if (converted == nullptr) {
        throw std::runtime_error("failed to convert current time");
    }
    local_time = *converted;
#endif

    return local_time;
}

}  // namespace

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

std::string formatted_today()
{
    const std::tm local_time = current_local_time();

    return fmt::format(
        u8"{:04}年{:02}月{:02}日 {:02}时{:02}分{:02}秒",
        local_time.tm_year + 1900,
        local_time.tm_mon + 1,
        local_time.tm_mday,
        local_time.tm_hour,
        local_time.tm_min,
        local_time.tm_sec);
}

}  // namespace mylib
