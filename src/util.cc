#include "util.hh"

namespace util 
{
    std::string join(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end, std::string delim)
    {
        if (begin == end) {
            return "";
        }

        std::string joined = "";
        std::for_each(begin, end - 1, [&](std::string& i) {
            joined += (i + delim);
        });

        joined += *(end - 1);

        return joined;
    }
}
