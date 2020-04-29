#include <iostream>
#include <string_view>
#include <client.hpp>
#include <memory>

using namespace std;

void run_rust_cpp(string_view url, string_view mint_key, string_view mnemonic);

int main(int argc, const char* argv[])
{
    try
    {
        if(argc < 4)
            throw runtime_error("missing arguments. \n Usage : url mint_key mnemonic");
        
        auto client = make_shared<Client>(argv[1], argv[2], argv[3]);

    }
    catch(const std::exception& e)
    {
        std::cerr << "cought exception : " << e.what() << '\n';
    }
    
    return 0;
}

void run_rust_cpp(string_view url, string_view mint_key, string_view mnemonic)
{

}