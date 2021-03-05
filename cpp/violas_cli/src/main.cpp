#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <map>
#include <functional>
#include <readline/readline.h>
#include <readline/history.h>
#include <client.hpp> //rust/client-proxy/ffi/client.hpp
#include "../../testnet/src/argument.hpp"
#include "command.hpp"

using namespace std;
using namespace violas;

bool parse_cmd(client_ptr client, const char *line);

int main(int argc, char *argv[])
{
    try
    {
        Arguments args;

        args.parse_command_line(argc, argv);

        auto client = Client::create(args.chain_id, args.url, args.mint_key, args.mnemonic, args.waypoint);

        client->test_connection();

        auto command = Command::create(client);

        for (;;)
        {
            char *cmd = readline("Violas $ ");
            if (*cmd == '\0')
                continue;

            try
            {
                if (command->parse_cmd(cmd))
                    ;
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }

            if (!strcmp(cmd, "quit"))
            {
                free(cmd);
                break;
            }

            add_history(cmd);

            free(cmd);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
