#ifndef LK_UTILS_HH
#define LK_UTILS_HH

#include <string>
#include <vector>

namespace labkit 
{

/// Split string into a vector of strings by given delimiters
std::vector<std::string> split(std::string list, std::string delim,
    size_t max_size = -1);

/// Type conversion using templates
template <typename T> T convertTo(const std::string &t_val);

}

#endif