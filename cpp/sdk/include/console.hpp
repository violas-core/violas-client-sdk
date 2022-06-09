#pragma once
#include <string>
#include <string_view>
#include <memory>

struct Console
{
    static std::shared_ptr<Console>
    create(std::string_view prompt);

    virtual ~Console() {}
    
    virtual void
    add_completion(std::string_view completion) = 0;

    virtual std::string
    read_line() = 0;

    virtual void
    add_history(std::string_view history) = 0;
};

using console_ptr = std::shared_ptr<Console>;