#ifndef COMMAND_HPP
#define COMMAND_HPP
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <client.hpp> //rust/client-proxy/ffi/client.hpp

struct Command
{
    static std::shared_ptr<Command> create(violas::client_ptr client);

    virtual ~Command() {}

    virtual bool parse_cmd(std::string_view cmds) = 0;

    virtual std::vector<std::string> get_all_cmds() = 0;
};

using command_ptr = std::shared_ptr<Command>;

struct Terminal : public Command
{
    static std::shared_ptr<Terminal> create(violas::client_ptr client);

    ~Terminal() {}
};

// struct CommandImpFactory : CommandFactory
// {
//     virtual command_ptr create_command(violas::client_ptr client);
// };

#endif