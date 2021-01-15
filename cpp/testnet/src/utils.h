#ifndef TERMINAL_H
#define TERMINAL_H
#include <string>
#include <array>
#include <iomanip>
#include <termios.h>
#include <unistd.h>

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
        if (showing_exp)
            std::cerr << e.what() << '\n';
    }
}

template <size_t N>
std::ostream &operator<<(std::ostream &os, const std::array<uint8_t, N> &bytes)
{
    for (auto v : bytes)
    {
        os << std::setfill('0') << std::setw(2) << std::hex << (int)v;
    }

    return os << std::dec;
}

inline void set_stdin_echo(bool enable)
{
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if (!enable)
        tty.c_lflag &= ~ECHO;
    else
        tty.c_lflag |= ECHO;

    (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

template <typename... Args>
std::string format(std::string_view format, Args... args)
{
    size_t size = snprintf(nullptr, 0, format.data(), args...) + 1; // Extra space for '\0'
    //std::unique_ptr<char[]> buf(new char[size]);
    auto buf = std::make_unique<char[]>(size);
    
    snprintf(buf.get(), size, format.data(), args...);
    
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

#endif