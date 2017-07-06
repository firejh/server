#include "string_kit.h"

namespace base{
namespace component{


std::string toString(const TimePoint& tp)
{
    return timePointToString(tp);
}

template<>
TimePoint fromString<TimePoint>(const std::string& str)
{
    return stringToTimePoint(str);
}

std::vector<std::string> splitString(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> result;
    splitString(&result, str, delimiter);
    return result;
}

void splitStr(std::vector<std::string>* ret, const std::string& str, const std::string& delimiter)
{
    splitString(ret, str, delimiter);
}


}}
