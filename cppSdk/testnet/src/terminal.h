#ifndef TERMINAL_H
#define TERMINAL_H
#include <string>

namespace color
{
const std::string RED("\033[0;31m");
const std::string GREEN("\033[1;32m");
const std::string YELLOW("\033[1;33m");
const std::string CYAN("\033[0;36m");
const std::string MAGENTA("\033[0;35m");
const std::string RESET("\033[0m");
} // namespace color

template <typename F>
void try_catch(F f, bool showing_exp = true)
{
    try
    {
        f();
    }
    catch (const std::exception &e)
    {
        if(showing_exp)
            std::cerr << e.what() << '\n';
    }
}

#endif