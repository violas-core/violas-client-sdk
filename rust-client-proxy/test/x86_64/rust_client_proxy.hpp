#ifndef _TESTLIB_H
#define _TESTLIB_H

#ifdef __cplusplus
extern "C"
{
#endif

    int add(int a, int b);

    int64_t create_native_client_proxy(
        const char *host,
        ushort port,
        const char *validator_set_file,
        const char *faucet_account_file,
        bool sync_on_wallet_recovery,
        const char *faucet_server,
        const char *mnemonic_file);

#ifdef __cplusplus
}
#endif

#endif