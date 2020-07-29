#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <functional>
#include "terminal.h"

using namespace std;

void run_test_libra(
    const string &url,
    const string &mint_key_file,
    const string &mnemonic_file,
    const string &waypoint,
    uint8_t chain_id);

void run_test_token(const string &url,
                    const string &mint_key_file,
                    const string &mnemonic_file,
                    const string &waypoint,
                    uint8_t chain_id);

void run_exchange(const string &url,
                  const string &mint_key_file,
                  const string &mnemonic_file,
                  const string &waypoint,
                  uint8_t chain_id);

void run_test_bank(const string &url,
                   const string &mint_key_file,
                   const string &mnemonic_file,
                   const string &waypoint,
                   uint8_t chain_id);

void run_account_management(const string &url,
                            const string &mint_key_file,
                            const string &mnemonic_file,
                            const string &waypoint,
                            uint8_t chain_id);

int main(int argc, char *argv[])
{
    ofstream file("log.txt");
    streambuf *mylog = clog.rdbuf(file.rdbuf());

    try
    {
        if (argc < 6)
        {
            cout << "usage : test_violas url mint_key_file mnemonic_file waypoint chain_id script_files_path";
            return -1;
        }

        using handler = function<void()>;
        map<int, handler> handlers = {
            {0, [=]() { run_test_libra(argv[1], argv[2], argv[3], argv[4], stoi(argv[5])); }},
            {1, [=]() { run_test_token(argv[1], argv[2], argv[3], argv[4], stoi(argv[5])); }},
            {2, [=]() { run_exchange(argv[1], argv[2], argv[3], argv[4], stoi(argv[5])); }},
            {3, [=]() { run_account_management(argv[1], argv[2], argv[3], argv[4], stoi(argv[5])); }},
            {4, [=]() { run_test_bank(argv[1], argv[2], argv[3], argv[4], stoi(argv[5])); }},
        };

        cout << "input index\n"
                "0 for testing Client, \n"
                "1 for testing Token \n"
                "2 for testing Exchange \n"
                "3 for testing Account Management.\n"
                "4 for testing Bank \n"
                "Please input index : ";
        //<< endl;

        int index;
        cin >> index;

        handlers[index]();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    clog.rdbuf(mylog);

    return 0;
}