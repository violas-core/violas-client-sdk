#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <readline/readline.h>
#include <readline/history.h>
#include <client.hpp>   //rust/client-proxy/ffi/client.hpp
#include <argument.hpp> //cpp/include
#include "command.hpp"
#include "utils.h"

using namespace std;
using namespace violas;

bool parse_cmd(client_ptr client, const char *line);
vector<string> g_cmds;

void initialize_readline()
{
    rl_readline_name = ">";

    // command completion
    rl_attempted_completion_function = [](const char *text, int start, int end) -> char ** {
        char **matches = nullptr;

        /* 
            * If this word is at the start of the line, then it is a command
            * to complete. Otherwise it is the name of a file in the current
            * directory. 
            */
        if (start == 0)
            matches = rl_completion_matches(text, [](const char *text, int state) -> char * {
                static int list_index, text_len;
                if (!state)
                {
                    list_index = 0;
                    text_len = strlen(text);
                }

                while (list_index < g_cmds.size())
                {
                    const auto &cmd = g_cmds[list_index];
                    list_index++;

                    if (cmd.compare(0, text_len, text) == 0)
                        return strdup(cmd.c_str());
                }

                return nullptr;
            });

        return (matches);
    };
}

int main(int argc, char *argv[])
{
    try
    {
        Arguments args;

        args.parse_command_line(argc, argv);

        auto client = Client::create(args.chain_id, args.url, args.mint_key, args.mnemonic, args.waypoint);

        client->test_connection();

        auto command = Command::create(client);

        // initialize readline completion
        g_cmds = command->get_all_cmds();
        initialize_readline();

        for (;;)
        {
            char *line = readline("Violas$ ");
            if (*line == '\0')
                continue;

            string cmd = line;
            free(line);

            // string trim
            cmd.erase(0, cmd.find_first_not_of(" "));
            cmd.erase(cmd.find_last_not_of(" ") + 1);

            if (cmd == "quit")
                break;

            try
            {
                if (command->parse_cmd(cmd))
                    ;
            }
            catch (const std::exception &e)
            {
                std::cerr << color::RED <<"error : " << e.what() << color::RESET << endl;
            }

            add_history(cmd.c_str());
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
