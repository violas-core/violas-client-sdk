#pragma once

#include <iostream>
#include <sstream>
#include <iterator>
#include <string>
#include <array>
#include <algorithm>
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

using bytes = std::vector<uint8_t>;
 
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

template <typename T>
void input(T &t)
{
    if (std::cin.get() != '\n')
    {
        std::cin.unget();
        std::cin >> t;
        std::cin.get(); // skip the lastest char 'Enter'
    }
}

template <size_t N>
std::ostringstream &operator<<(std::ostringstream &oss, const std::array<uint8_t, N> &bytes)
{
    auto flags = oss.flags();
    auto fill = oss.fill();

    for (auto v : bytes)
    {
        oss << std::setfill('0') << std::setw(2) << std::hex << (int)v;
    }

    oss.setf(flags);
    std::setfill(fill);

    return oss;
}

template <size_t N>
std::ostream &operator<<(std::ostream &os, const std::array<uint8_t, N> &bytes)
{
    // std::ostream tos(os.rdbuf());   // temp os for setting io manipulator
    std::ostringstream oss;

    oss << bytes;

    // for (auto v : bytes)
    // {
    //     oss << std::setfill('0') << std::setw(2) << std::hex << (int)v;
    // }

    return os << oss.str();
}

// inline std::istringstream &operator>>(std::istringstream &iss, const std::vector<uint8_t> &bytes)
// {
//     string str;
//     iss >> str;

//     for (auto v : bytes)
//     {
//         iss << std::setfill('0') << std::setw(2) << std::hex << (int)v;
//     }

//     return iss;
// }

inline std::ostringstream &operator<<(std::ostringstream &oss, const std::vector<uint8_t> &bytes)
{
    auto flags = oss.flags();

    for (auto v : bytes)
    {
        oss << std::setfill('0') << std::setw(2) << std::hex << (int)v;
    }

    oss.setf(flags);

    return oss;
}

inline std::ostream &operator<<(std::ostream &os, const std::vector<uint8_t> &bytes)
{
    std::ostringstream oss;

    oss << bytes;

    return os << oss.str();
}

template <size_t N>
void operator>>(const std::string &str, std::array<uint8_t, N> &bytes)
{
    for (size_t i = 0, x = 0; i < str.length() && i < N; ++i, x += 2)
    {
        std::istringstream iss(str.substr(x, 2));
        unsigned short b = 0;

        if (!(iss >> std::hex >> b))
        {
            // error!
            break;
        }

        // set bit from high to low
        // bytes[N - 1 - i] = b;
        bytes[i] = b;
    }
}

template <size_t N>
std::istream &operator>>(std::istream &iss, std::array<uint8_t, N> &bytes)
{
    std::string str;

    iss >> str;
    str >> bytes;

    return iss;
}

template <size_t N>
void operator>>(std::istringstream &&iss, std::array<uint8_t, N> &bytes)
{
    std::string str;

    iss >> str;
    str >> bytes;
}

// template <size_t N>
// std::istream &operator>>(std::istream &is, std::array<uint8_t, N> &bytes)
// {
//     std::string temp;

//     is >> temp;

//     size_t numbytes = temp.size() / 2;

//     for (size_t i = 0, x = 0; i < numbytes && i < N; ++i, x += 2)
//     {
//         std::istringstream iss(temp.substr(x, 2));
//         unsigned short b = 0;

//         if (!(iss >> std::hex >> b))
//         {
//             // error!
//             break;
//         }

//         // set from high to low bit
//         //bytes[N - 1 - i] = b;
//         bytes[i] = b;
//     }

//     return is;
// }

// template <size_t N>
// std::istream &operator>>(std::istream &&is, std::array<uint8_t, N> &bytes)
// {
//     std::__rvalue_istream_type<std::istream> __ret_is = is;

//     __ret_is >> bytes;

//     return __ret_is;
// }

inline std::vector<uint8_t> hex_to_bytes(const std::string &str)
{
    std::vector<uint8_t> bytes;

    auto beg = std::begin(str);
    auto end = std::end(str);

    if (*beg == '0' && *(beg + 1) == 'x')
        beg += 2;

    while (beg != end)
    {
        std::stringstream ss;
        uint16_t byte = 0;

        ss << *beg++ << *beg++; // read two chars
        ss >> std::hex >> byte;

        bytes.push_back(byte);
    }

    return bytes;
}

template<size_t N>
inline std::array<uint8_t, N> hex_to_array_u8(std::string_view hex)
{
    std::array<uint8_t, N> array_u8;

    if(hex.size() < array_u8.size())
        std::__throw_runtime_error("hex_to_array_u8 error, the length of hex is less than the size of array");

    for(size_t i = 0; i < array_u8.size(); i++)
    {
        std::stringstream ss;
        uint16_t byte = 0;

        ss << hex.at(i*2) << hex.at(i*2+1); // read two chars
        ss >> std::hex >> byte;

        array_u8[i] = byte;
    }    

    return array_u8;
}

auto bytes_to_hex(const auto &bytes)
{
    using namespace std;
    ostringstream oss;

    for (auto v : bytes)
    {
        oss << hex << setw(2) << setfill('0') << (int)v;
    }

    return oss.str();
};

std::string bytes_to_string(const auto &bytes)
{
    return std::string(std::begin(bytes), std::end(bytes));
}

auto u8_array_to_vector(const auto &u8_array)
{
    return std::vector<uint8_t>(std::begin(u8_array), std::end(u8_array));
}

inline std::vector<uint8_t> string_to_bytes(const std::string &str)
{
    return std::vector<uint8_t>(begin(str), end(str));
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
    // std::unique_ptr<char[]> buf(new char[size]);
    auto buf = std::make_unique<char[]>(size);

    snprintf(buf.get(), size, format.data(), args...);

    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

template <typename... Args>
std::string fmt(Args... args)
{
    std::ostringstream oss;

    ((oss << args), ...);

    return oss.str();
}

inline bool is_not_space(char ch)
{
    return !std::isspace(ch);
}

inline std::string trim_left(std::string s)
{
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(), is_not_space));
    return s;
}

inline std::string trim_right(std::string s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), is_not_space).base(),
            s.end());
    return s;
}

inline std::string trim(std::string s)
{
    return trim_left(trim_right(std::move(s)));
}
//
//  helper for std::visit
// refer to https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;