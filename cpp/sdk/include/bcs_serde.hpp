#ifndef SERDE_BCS_HPP
#define SERDE_BCS_HPP
#include <iomanip>
#include <sstream>
#include <optional>
#include <variant>
#include <map>

class BcsSerde;

template <typename T>
concept has_serde = requires(T t)
{
    // check if T has the method serde
    // struct T
    // {
    //      BcsSerde& serde(BomSerde& bs) { return bs; }
    // }
    std::same_as<decltype(t.serde(std::declval<BcsSerde &>())), BcsSerde &>;
};

//
//  Binary Serialization and Deserialization
//
class BcsSerde
{
    std::vector<uint8_t> _bytes;
    std::vector<uint8_t>::iterator _iterator = std::begin(_bytes);
    bool _is_serialization = true;

    void encode_integer(size_t value)
    {
        do
        {
            // get the low 7 bits
            uint8_t v = value & 0b01111111;

            // shift rgiht 7 bits
            value >>= 7;

            if (value)
                v |= 0b10000000; // set the highest bit to 1

            _bytes.push_back(v);
        } while (value);
    }

    //
    //  Decode ULEB128 Integer, refer to https://en.wikipedia.org/wiki/LEB128
    //
    size_t decode_integer()
    {
        size_t value = 0;
        uint8_t v = 0;

        for (size_t i = 0; i < 4; i++)
        {
            v = *_iterator++;

            // get the low 7 bits
            size_t t = v & 0b01111111; //0x80

            // Shift 7 bits to left
            t <<= 7 * i;

            // and low bits
            value |= t;

            // break loop if the highest bit is zero
            if (!(v & 0b10000000))
                break;
        }

        return value;
    }

public:
    BcsSerde() { _is_serialization = true; }

    BcsSerde(const std::vector<uint8_t> &bytes) : _is_serialization(false)
    {
        _bytes = bytes;
        _iterator = std::begin(_bytes);
    }

    const BcsSerde &operator=(const BcsSerde &bs) = delete;

    std::vector<uint8_t> bytes() { return _bytes; }

    void reset() { _iterator = std::begin(_bytes); }
    
    void set_des()
    {
        _is_serialization = false;
        reset();
    }

    std::string dump()
    {
        int i = 0;
        std::ostringstream oss;

        for (auto v : _bytes)
        {
            if (i % 16)
                oss << std::endl;

            oss << std::setfill('0') << std::setw(2) << std::hex << (short)v << " ";
        }

        return oss.str();
    }

    //
    //  Serialize a class that has serde
    //
    template <has_serde T>
    BcsSerde &operator&&(T &t)
    {
        return t.serde(*this);
    }

    //
    //  Integer, sunch char, short, int, long
    //
    template <std::integral T>
    BcsSerde &operator&&(T &t)
    {
        if (_is_serialization)
        {
            auto beg = (uint8_t *)&t;
            auto end = beg + sizeof(T);

            std::copy(beg, end, back_inserter(_bytes));
        }
        else //deserialize
        {
            auto beg = (uint8_t *)&t;
            auto end = beg + sizeof(T);
            while (beg < end)
            {
                *beg++ = *_iterator++;
            }
        }

        return *this;
    }

    BcsSerde &operator&&(bool &t)
    {
        uint8_t v = t;

        (*this) && v;

        t = v;

        return *this;
    }

    //
    //  for string
    //
    BcsSerde &operator&&(std::string &t)
    {
        if (_is_serialization)
        {
            encode_integer(t.length());

            std::copy(std::begin(t), std::end(t), back_inserter(_bytes));
        }
        else //deserialize
        {
            size_t size = decode_integer();
            t.clear();

            for (auto i = 0; i < size; i++)
                t.push_back(*_iterator++);
        }

        return *this;
    }
    //
    //  for container, such as vector, list
    //
    template <typename T,
              template <class TElem, typename Alloc = std::allocator<TElem>> class Container>
    BcsSerde &operator&&(Container<T> &container)
    {
        if (_is_serialization)
        {
            encode_integer(container.size());

            for (auto &v : container)
            {
                *this &&v;
            }
        }
        else //deserialize
        {
            size_t size = decode_integer();
            container.resize(size);

            for (auto &v : container)
            {
                *this &&v;
            }
        }

        return *this;
    }
    //
    // For std::set
    //
    template <typename T,
              template <typename Key, class Compare = std::less<Key>, class Allocator = std::allocator<Key>>
              typename Set>
    BcsSerde &operator&&(Set<T> &set)
    {
        if (_is_serialization)
        {
            encode_integer(set.size());

            for (auto v : set)
            {
                *this &&v;
            }
        }
        else //deserialize
        {
            set.clear();

            size_t size = decode_integer();

            for (size_t i = 0; i < size; i++)
            {
                T k;
                *this &&k;

                set.insert(k);
            }
        }

        return *this;
    }
    //
    // For std::pair
    //
    template <typename Key, typename Value>
    BcsSerde &operator&&(std::pair<Key, Value> &p)
    {
        (*this) && p.first;
        (*this) && p.second;

        return *this;
    }

    //
    //  For container with key and value
    //
    template <typename Key, typename Value>
    BcsSerde &operator&&(std::map<Key, Value> &container)
    {
        if (_is_serialization)
        {
            encode_integer(container.size());

            for (auto v : container)
            {
                Key k = v.first;

                (*this) && k;
                (*this) && v.second;
            }
        }
        else //deserialize
        {
            container.clear();

            size_t size = decode_integer();
            for (size_t i = 0; i < size; i++)
            {
                std::pair<Key, Value> p;

                (*this) && p;

                container.insert(p);
            }
        }

        return *this;
    }

    //
    //  serialize std::tuple
    //
    template <typename... Args>
    BcsSerde &operator&&(std::tuple<Args...> &tup)
    {
        std::apply([&](auto &...args)
                   { ((*this && args), ...); },
                   tup);

        return *this;
    }

    //
    //  serialize std::array
    //
    template <typename T, size_t N>
    BcsSerde &operator&&(std::array<T, N> &arr)
    {
        for (auto &a : arr)
            *this &&a;

        return *this;
    }

    //
    //  serialize std::array
    //
    template <typename T, size_t N>
    BcsSerde &operator&&(T arr[])
    {
        std::for_each(arr, arr + N, [this](T &a)
                      { *this &&a; });

        return *this;
    }
    //
    //  serialize and deserialize for std::optional
    //
    template <typename T>
    BcsSerde &operator&&(std::optional<T> &opt)
    {
        if (_is_serialization)
        {
            if (opt != std::nullopt)
            {
                auto v = opt.value();
                uint8_t flag = 1;

                *this &&flag;
                *this &&v;
            }
            else
            {
                uint8_t flag = 0;
                *this &&flag;
            }
        }
        else //deserialize
        {
            uint8_t flag;

            *this &&flag;

            if (!flag)
                opt = std::nullopt;
            else
            {
                T t;

                *this &&t;
                opt = t;
            }
        }

        return *this;
    }

    template <typename... Args>
    BcsSerde &operator&&(std::variant<Args...> &var)
    {
        if (_is_serialization)
        {
            encode_integer(var.index());
        }
        else //deserialize
        {
            //size_t index = decode_integer();
            decode_integer();
        }

        std::visit([this](auto &arg)
                   {
                       //using T = std::decay_t<decltype(arg)>;
                       //T t;
                       (*this) && arg;
                   },
                   var);

        return *this;
    }
};

#endif