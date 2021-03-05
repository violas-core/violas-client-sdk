#ifndef COMMAND_HPP
#define COMMAND_HPP
#include <memory>
#include <string_view>
#include <client.hpp> //rust/client-proxy/ffi/client.hpp

struct Command
{
    static std::shared_ptr<Command> create(violas::client_ptr client);

    virtual ~Command() { }
    
    virtual bool parse_cmd(std::string_view cmds) = 0;

    virtual void show_all_cmd() = 0;
};

using command_ptr = std::shared_ptr<Command>;


// struct CommandFactory
// {
//     virtual command_ptr create_command(violas::client_ptr client) = 0;
// };

// struct CommandImpFactory : CommandFactory
// {
//     virtual command_ptr create_command(violas::client_ptr client);
// };

#endif