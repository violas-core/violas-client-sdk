#include <cpprest/filestream.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include "utils.h"
#include "../include/json_rpc.hpp"

using namespace std;
using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;

namespace json_rpc
{

    class ClientImp : public Client
    {
        http_client cli;

    public:
        ClientImp(string url) : cli(U(url), client_config_for_proxy())
        {
        }

        virtual ~ClientImp()
        {
        }

        web::http::client::http_client_config client_config_for_proxy()
        {
            web::http::client::http_client_config client_config;
#ifdef _WIN32
            wchar_t *pValue = nullptr;
            std::unique_ptr<wchar_t, void (*)(wchar_t *)> holder(nullptr, [](wchar_t *p)
                                                                 { free(p); });
            size_t len = 0;
            auto err = _wdupenv_s(&pValue, &len, L"http_proxy");
            if (pValue)
                holder.reset(pValue);
            if (!err && pValue && len)
            {
                std::wstring env_http_proxy_string(pValue, len - 1);
#else
            if (const char *env_http_proxy = std::getenv("http_proxy"))
            {
                std::string env_http_proxy_string(env_http_proxy);
#endif
                if (env_http_proxy_string == U("auto"))
                    client_config.set_proxy(web::web_proxy::use_auto_discovery);
                else
                    client_config.set_proxy(web::web_proxy(env_http_proxy_string));
            }

            return client_config;
        }

        virtual std::vector<Currency>
        get_currencies() override
        {
            vector<Currency> crc;

            return crc;
        }

        virtual AccountStateWithProof
        get_account_state_blob(string account_address) override
        {
            AccountStateWithProof asp;
            string method = format(R"({"jsonrpc":"2.0","method":"get_account_state_with_proof","params":["%s", null, null],"id":1})", account_address.c_str());
            string content_type = "application/json";

            auto rpc_response = cli.request(methods::POST, "/", method, content_type)
                                    .then([=](http_response response) -> pplx::task<json::value>
                                          {
                                              printf("Response status code %u returned.\n", response.status_code());

                                              //   if( response.status_code() != 200 )
                                              //     __throw_runtime_error(response.extract_json().get().serialize().c_str());

                                              return response.extract_json();
                                          })
                                    .get();

            auto error = rpc_response["error"];
            if (!error.is_null())
                __throw_runtime_error(("fun : get_account_state_blob, error : " + error.serialize()).c_str());

            auto result = rpc_response["result"];
            // cout << result.serialize() << endl;

            asp.version = result["version"].as_integer();

            auto blob = result["blob"];
            if (blob.is_null())
                __throw_runtime_error(("fun : get_account_state_blob, error : " + rpc_response.serialize()).c_str());

            asp.blob = blob.as_string();

            asp.proof.ledger_info_to_transaction_info_proof = result["proof"]["ledger_info_to_transaction_info_proof"].as_string();
            asp.proof.transaction_info = result["proof"]["transaction_info"].as_string();
            asp.proof.transaction_info_to_account_proof = result["proof"]["transaction_info_to_account_proof"].as_string();

            return asp;
        }
    };

    std::shared_ptr<Client>
    Client::create(std::string url)
    {
        return make_shared<ClientImp>(url);
    }

}
