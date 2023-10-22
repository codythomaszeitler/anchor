#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <vector>

namespace util
{
    std::string join(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end, std::string delim);
}

#endif // __UTIL_H__