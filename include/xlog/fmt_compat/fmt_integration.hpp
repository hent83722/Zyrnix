#pragma once
#include "fmt_shim.hpp"
#include <string>

namespace xlog {

template<typename... Args>
inline std::string fmt_format(const std::string& fmt_str, Args&&... args) {
    return format(fmt_str, std::forward<Args>(args)...);
}

}
