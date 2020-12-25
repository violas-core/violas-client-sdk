#include <unistd.h>
#include <string>
#include <iostream>

struct Arguments
{
    std::string url;
    std::string mint_key;
    std::string mnemoic;
    std::string waypoint;
    uint8_t chain_id = -1;
    bool distrbuting = false;

    void parse_command_line(int argc, char *argv[])
    {
        int opt;

        while ((opt = getopt(argc, argv, "du:m:n:w:c:")) != -1)
        {
            switch (opt)
            {
            case 'u':
                url = optarg;
                break;
            case 'm':
                mint_key = optarg;
                break;
            case 'n':
                mnemoic = optarg;
                break;
            case 'w':
                waypoint = optarg;
                break;
            case 'c':
                chain_id = std::stoi(optarg);
                break;
            case 'd':
                distrbuting = true;
                break;
            case '?':
            default:
                throw std::runtime_error("usage : -u url -m mint.key -n mnemonic -w waypoint -c chain_id");
                break;
            }
        }
    }

    void show()
    {
        std::cout << url << " : "
                  << mint_key << " : "
                  << mnemoic << " : "
                  << waypoint << " : "
                  << chain_id << " : "
                  << distrbuting
                  << std::endl;
    }
};
