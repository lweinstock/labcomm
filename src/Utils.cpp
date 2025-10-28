#include <labcomm/Utils.hh>
#include <labcomm/Exceptions.hh>

#include <sstream>

namespace labcomm
{

template <typename T> T convertTo(const std::string &t_val)
{
    std::istringstream iss(t_val);
    T ret;
    iss >> ret;
    bool success = !iss.fail();
    if (!success)
        throw ConversionError("Failed to convert '" + t_val + " to "
            + typeid(T).name());
    return ret;
}

template int convertTo<int>(const std::string &t_val);
template unsigned convertTo<unsigned>(const std::string &t_val);
template uint8_t convertTo<uint8_t>(const std::string &t_val);
template float convertTo<float>(const std::string &t_val);
template double convertTo<double>(const std::string &t_val);

}