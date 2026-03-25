#ifndef LK_UTILS_HH
#define LK_UTILS_HH

#include <string>
#include <vector>
#include <numeric>
#include <cmath>

namespace labkit 
{

/// Split string into a vector of strings by given delimiters
std::vector<std::string> split(std::string list, std::string delim,
    size_t max_size = -1);
    
/// Returns a string with all control characters removed (i.e. \n, \r, \0, ...)
std::string removeCtrlChars(const std::string &str);

/// Type conversion using templates
template <typename T> T convertTo(const std::string &t_val);

/// Returns the mean of given container
template <typename Iter> double getMean(Iter t_first, Iter t_last)
{
    double sum = std::accumulate(t_first, t_last, .0);
    size_t n = std::distance(t_first, t_last);
    return sum/n;
}

/// Returns the standard deviation of given container
template <typename Iter> double getStDev(Iter t_first, Iter t_last)
{
    double mean = getMean(t_first, t_last);
    size_t n = std::distance(t_first, t_last);
    double variance = std::transform_reduce(t_first, t_last, .0, 
        std::plus<double>{},    // Accumulate result
        [mean](double x) {      // Transformation: (x - mean)^2
            double diff = x - mean;
            return diff*diff;
        }
    );
    return std::sqrt(variance/(n-1));
}

}

#endif