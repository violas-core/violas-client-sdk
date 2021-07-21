#include <vector>
#include <readline/readline.h>
#include <readline/history.h>
#include "../include/console.hpp"

using namespace std;
static vector<string> _completions;

class ConsoleImp : public Console
{
    string _prompt;

public:
    void initialize(std::string_view prompt)
    {
        _prompt = prompt;

        rl_readline_name = ">";

        // command completion
        rl_attempted_completion_function = [](const char *text, int start, int end) -> char **
        {
            char **matches = nullptr;

            /* 
            * If this word is at the start of the line, then it is a command
            * to complete. Otherwise it is the name of a file in the current
            * directory. 
            */
            if (start == 0)
                matches = rl_completion_matches(text, [](const char *text, int state) -> char *
                                                {
                                                    static int list_index, text_len;
                                                    if (!state)
                                                    {
                                                        list_index = 0;
                                                        text_len = strlen(text);
                                                    }

                                                    while (list_index < _completions.size())
                                                    {
                                                        const auto &cmd = _completions[list_index];
                                                        list_index++;

                                                        if (cmd.compare(0, text_len, text) == 0)
                                                            return strdup(cmd.c_str());
                                                    }

                                                    return nullptr;
                                                });

            return (matches);
        };
    }

    virtual void
    add_completion(std::string_view completion) override
    {
        _completions.push_back(completion.data());
    }

    virtual std::string
    read_line() override
    {
        string line = readline(_prompt.c_str());

        return line;
    }

    virtual void
    add_history(std::string_view history) override
    {
        ::add_history(history.data());
    }
};

shared_ptr<Console> Console::create(std::string_view prompt)
{
    auto console = make_shared<ConsoleImp>();

    console->initialize(prompt);

    return console;
}