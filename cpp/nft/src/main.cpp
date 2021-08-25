#include <iostream>
#include <map>
#include <functional>
#include <random>

#include <utils.hpp>
#include <violas_sdk2.hpp>
#include <argument.hpp>
#include <bcs_serde.hpp>
#include <json_rpc.hpp>
#include <console.hpp>
#include <ssl_aes.hpp>
#include "tea.hpp"

using namespace std;
using namespace violas;
using namespace violas::nft;

std::ostream &operator<<(std::ostream &os, const NftInfo &nft_info);
std::ostream &operator<<(ostream &os, const vector<MintedEvent> &minted_events);
std::ostream &operator<<(ostream &os, const vector<BurnedEvent> &minted_events);
std::ostream &operator<<(ostream &os, const vector<nft::SentEvent> &minted_events);
std::ostream &operator<<(ostream &os, const vector<nft::ReceivedEvent> &minted_events);

void mint_tea_nft(client_ptr client, Address addr);

using handle = function<void(istringstream &params)>;
map<string, handle> create_commands(client_ptr client, string url, nft_ptr<Tea> nft);

int main(int argc, char *argv[])
{
    if (argc < 11)
    {
        cout << "usage : bin/nft -u url -m mint.key -n mnemonic -w waypoint -c chain_id" << endl;
        return 0;
    }

    try
    {
        Arguments args;

        args.parse_command_line(argc, argv);

        auto client = Client::create(args.chain_id, args.url, args.mint_key, args.mnemonic, args.waypoint);
        auto nft = make_shared<NonFungibleToken<Tea>>(client, args.url);

        cout << "NFT Management 1.0" << endl;

        client->create_next_account();
        //auto admin = client->create_next_account();
        //auto dealer1 = client->create_next_account();
        //auto dealer2 = client->create_next_account();

        // cout << "Admin      : " << admin.address << "\n"
        //      << "Dealer 1   : " << dealer1.address << "\n"
        //      << "Dealer 2   : " << dealer2.address << endl;

        auto console = Console::create("NFT$ ");
        const string exit = "exit";

        console->add_completion(exit);

        auto commands = create_commands(client, args.url, nft);
        for (auto cmd : commands)
        {
            console->add_completion(cmd.first);
        }

        // show all account
        istringstream iss;
        commands["list-accounts"](iss);

        //
        //  Loop to read a line
        //
        for (auto line = trim(console->read_line());
             line != exit;
             line = trim(console->read_line()))
        {
            istringstream iss(line);
            string cmd;

            // Read a command
            iss >> cmd;

            auto iter = commands.find(cmd);
            if (iter != end(commands))
            {
                try
                {
                    iter->second(iss);
                }
                catch (const std::invalid_argument &e)
                {
                    std::cerr << "Invalid argument : " << e.what() << endl;
                }
                catch (runtime_error &e)
                {
                    std::cerr << color::RED
                              << "Runtime error : " << e.what()
                              << color::RESET << endl;
                }
            }

            console->add_history(line);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << color::RED
                  << "Exceptions : " << e.what()
                  << color::RESET
                  << endl;
    }

    return 0;
}

void check_istream_eof(istream &is, string_view err)
{
    if (is.eof())
    {
        ostringstream oss;

        oss << err;
        __throw_invalid_argument(oss.str().c_str());
    }
}

template <typename T>
T get_from_stream(istringstream &params, client_ptr client, string_view err_info = "index or address")
{
    Address addr;
    int account_index = -1;

    check_istream_eof(params, err_info);

    string temp;

    params >> temp;
    istringstream iss(temp);

    if (temp.length() == sizeof(T) * 2)
    {
        iss >> addr;
    }
    else
    {
        auto accounts = client->get_all_accounts();

        iss >> account_index;
        if (account_index >= accounts.size())
            __throw_invalid_argument("account index is out of account size.");
        else
            addr = accounts[account_index].address;
    }

    return addr;
}

map<string, handle> create_commands(client_ptr client, string url, nft_ptr<Tea> nft)
{
    return map<string, handle>{
        {"deploy", [=](istringstream &params)
         {
             //deploy_stdlib(client);
             nft->deploy();
         }},
        {"register", [=](istringstream &params)
         {
             check_istream_eof(params, "NFT total number");

             uint64_t total = 1000;
             params >> total;

             //register_mountwuyi_tea_nft(client, total);
             nft->register_instance(total);
         }},
        {"accept", [=](istringstream &params)
         {
             size_t account_index = 0;
             params >> account_index;

             //accept(client, account_index);
             nft->accept(account_index);
         }},
        {"mint", [=](istringstream &params)
         {
             auto addr = get_from_stream<Address>(params, client);
             mint_tea_nft(client, addr);
         }},
        {"burn", [=](istringstream &params)
         {
             TokenId token_id;

             params >> token_id;

             //burn_tea_nft(client, token_id);
             nft->burn(token_id);
         }},
        {"transfer", [=](istringstream &params)
         {
             size_t account_index = 0, nft_index = 0;
             Address receiver;

             check_istream_eof(params, "account_index");

             params >> account_index;

             check_istream_eof(params, "receiver address");
             receiver = get_from_stream<Address>(params, client);

             check_istream_eof(params, "nft_index");
             params >> nft_index;

             //transfer(client, account_index, receiver, nft_index);
             nft->transfer(account_index, receiver, nft_index);
         }},
        {"balance", [=](istringstream &params)
         {
             auto addr = get_from_stream<Address>(params, client);

             auto opt_balance = nft->balance(addr);
             if (opt_balance != nullopt)
             {
                 int i = 0;
                 for (const auto &tea : *opt_balance)
                 {
                     cout << i++ << " - " << tea << endl;
                 }
             }
         }},
        {"owner", [=](istringstream &params)
         {
             TokenId id;

             params >> id;

             auto owners = nft->get_owners(url, id);
             if (owners != nullopt)
             {
                 // print the address of owner
                 cout << *(owners->rbegin()) << endl;
             }
             else
                 cout << "cannot find owner." << endl;
         }},
        {"trace", [=](istringstream &params)
         {
             TokenId token_id;

             check_istream_eof(params, "token id");
             params >> token_id;

             auto receivers = nft->get_owners(url, token_id);
             if (receivers != nullopt)
             {
                 size_t i = 0;
                 for (const auto receiver : *receivers)
                 {
                     cout << i++ << "  -  " << receiver << endl;
                 }
             }
         }},
        {"info", [=](istringstream &params)
         {
             auto opt_info = nft->get_nft_info(url);

             if (opt_info != nullopt)
                 cout << *opt_info << endl;
         }},
        {"create_child_account", [=](istringstream &params)
         {
             Address addr;
             AuthenticationKey auth_key;

             check_istream_eof(params, "authentication key");
             params >> auth_key;

             copy(begin(auth_key) + 16, begin(auth_key) + 32, begin(addr));

             client->create_child_vasp_account("VLS", 0, addr, auth_key, true, 0, true);
         }},
        {"add-account", [=](istringstream &params)
         {
             client->create_next_account();
         }},
        {"list-accounts", [=](istringstream &params)
         {
             auto accounts = client->get_all_accounts();

             cout << color::CYAN
                  << left << setw(10) << "index"
                  << left << setw(40) << "Address"
                  << left << setw(40) << "Authentication key"
                  << color::RESET << endl;

             int i = 0;
             for (auto &account : accounts)
             {
                 cout << left << setw(10) << i++
                      << left << setw(40) << account.address
                      << left << setw(40) << account.auth_key
                      << endl;
             }
         }},
        {"query-events", [=](istringstream &params)
         {
             string event_type;

             check_istream_eof(params, "(usage) : query_event [minted, burned, sent, received] address start limit");
             params >> event_type;

             auto addr = get_from_stream<Address>(params, client);
             uint64_t start = 0, limit = 10;

             if (!params.eof())
                 params >> start;

             if (!params.eof())
                 params >> limit;

             if (event_type == "minted")
             {
                 auto events = nft->query_events<MintedEvent>(EventType::minted, addr, start, limit);
                 cout << events << endl;
             }
             else if (event_type == "burned")
             {
                 auto events = nft->query_events<BurnedEvent>(EventType::burned, addr, start, limit);
                 cout << events << endl;
             }
             else if (event_type == "sent")
             {
                 auto events = nft->query_events<SentEvent>(EventType::sent, addr, start, limit);
                 cout << events << endl;
             }
             else if (event_type == "received")
             {
                 auto events = nft->query_events<ReceivedEvent>(EventType::received, addr, start, limit);
                 cout << events << endl;
             }
             else
             {
                 __throw_invalid_argument("event type is invalid, please input [minted, burned, sent, received]");
             }

             //  for (auto &event : events)
             //  {
             //      //cout << event << endl;
             //  }
         }},
    };
}

template <typename T>
void input(T &t)
{
    if (cin.get() != '\n')
    {
        cin.unget();
        cin >> t;
        cin.get(); //skip the lastest char 'Enter'
    }
}

vector<uint8_t> string_to_bytes(const string &str)
{
    return vector<uint8_t>(begin(str), end(str));
}

void mint_tea_nft(client_ptr client, Address addr)
{
    cout << "minting Tea NFT ... " << endl;

    auto accounts = client->get_all_accounts();
    auto &admin = accounts[0];

    default_random_engine e(clock());
    uniform_int_distribution<unsigned> u(0, 25);
    size_t kind = 0;

    cout << "input kind(0, 1, 2, 3, 4, default is 0) :";
    input(kind);

    string PA = "MountWuyi City";
    cout << "input production area (default is '" << PA << "') : ";
    input(PA);

    string manufacturer = "21VNET Tea";
    cout << "input manufacturer (default is '" << manufacturer << "') : ";
    input(manufacturer);

    string SN = {'1', '2', '3', '4', '5', '6', char('a' + u(e)), char('a' + u(e))};
    cout << "input sequence number (default is '" << SN << "') : ";
    input(SN);

    string url = "https://www.mountwuyitea.com";
    cout << "input a url (default is '" << url << "' : ";
    input(url);

    time_t now = time(nullptr);
    cout << "input production date (default is '" << put_time(localtime(&now), "%F") << "') : ";
    if (cin.get() != '\n')
    {
        cin.unget();

        //cout << "local = " << std::locale("").name() << endl;
        std::tm t = {};
        cin.imbue(locale("zh_CN.UTF-8"));
        cin >> std::get_time(&t, "%Y-%m-%d");

        if (cin.fail())
            cout << "The date format is error, use default date " << put_time(localtime(&now), "%F") << "." << endl;
        else
        {
            now = mktime(&t);
        }

        cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Keep date and remove time
    std::tm *t = localtime(&now);
    t->tm_hour = t->tm_min = t->tm_sec = 0;
    now = mktime(t);

    client->execute_script_file(admin.index,
                                "move/tea/scripts/mint_mountwuyi_tea_nft.mv",
                                {},
                                {
                                    uint8_t(kind),
                                    string_to_bytes(manufacturer),
                                    string_to_bytes(PA),
                                    uint64_t(now),
                                    string_to_bytes(SN),
                                    string_to_bytes(url),
                                    addr,
                                });

    cout << "Minted a Tea NFT to address " << addr << endl;
}

std::ostream &operator<<(std::ostream &os, const nft::NftInfo &nft_info)
{
    os << "NonFungibleToken Info { \n"
       << "\t"
       << "total : " << nft_info.total << "\n"
       << "\t"
       << "amount : " << nft_info.amount << "\n"
       << "\t"
       << "admin address: " << nft_info.admin << "\n"
       << "\t"
       << "minted amount : " << nft_info.mint_event.counter << "\n"
       << "\t"
       << "burned amount : " << nft_info.burn_event.counter << "\n"
       << "}";

    return os;
}

std::ostream &operator<<(ostream &os, const vector<nft::MintedEvent> &minted_events)
{

    cout << color::CYAN
         << left << setw(10) << "SN"
         << left << setw(70) << "Token ID"
         << left << setw(40) << "Receiver Address"
         << left << setw(10) << "Version"
         << color::RESET << endl;

    for (auto &e : minted_events)
    {
        cout << left << setw(10) << e.sequence_number
             << left << setw(70) << e.token_id
             << left << setw(40) << e.receiver
             << left << setw(10) << e.transaction_version
             << endl;
    }

    return os;
}

std::ostream &operator<<(ostream &os, const vector<nft::BurnedEvent> &minted_events)
{

    cout << color::YELLOW
         << "Burned events list" << endl
         << color::CYAN
         << left << setw(10) << "SN"
         << left << setw(70) << "Token ID"
         << left << setw(10) << "Version"
         << color::RESET << endl;

    for (auto &e : minted_events)
    {
        cout << left << setw(10) << e.sequence_number
             << left << setw(70) << e.token_id
             << left << setw(10) << e.transaction_version
             << endl;
    }

    return os;
}

std::ostream &operator<<(ostream &os, const vector<nft::SentEvent> &minted_events)
{

    cout << color::YELLOW
         << "Sent events list" << endl
         << color::CYAN
         << left << setw(10) << "SN"
         << left << setw(70) << "Token ID"
         << left << setw(40) << "Payee"
         << left << setw(20) << "Metadata"
         << left << setw(10) << "Version"
         << color::RESET << endl;

    for (auto &e : minted_events)
    {
        cout << left << setw(10) << e.sequence_number
             << left << setw(70) << e.token_id
             << left << setw(40) << e.payee
             << left << setw(20) << e.metadata
             << left << setw(10) << e.metadata
             << endl;
    }

    return os;
}

std::ostream &operator<<(ostream &os, const vector<nft::ReceivedEvent> &minted_events)
{

    cout << color::YELLOW
         << "Received events list" << endl
         << color::CYAN
         << left << setw(10) << "SN"
         << left << setw(70) << "Token ID"
         << left << setw(40) << "Payer"
         << left << setw(20) << "Metadata"
         << left << setw(10) << "Version"
         << color::RESET << endl;

    for (auto &e : minted_events)
    {
        cout << left << setw(10) << e.sequence_number
             << left << setw(70) << e.token_id
             << left << setw(40) << e.payer
             << left << setw(20) << e.metadata
             << left << setw(10) << e.transaction_version
             << endl;
    }

    return os;
}