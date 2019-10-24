#ifndef _TESTLIB_H
#define _TESTLIB_H

#ifdef __cplusplus
extern "C"
{
#endif

    uint64_t create_libra_client_proxy(
        const char *host,
        ushort port,
        const char *validator_set_file,
        const char *faucet_account_file,
        bool sync_on_wallet_recovery,
        const char *faucet_server,
        const char *mnemonic_file);

    void destory_libra_client_proxy(uint64_t raw_ptr);

    bool libra_test_validator_connection(uint64_t raw_ptr);

    void libra_create_next_account(uint64_t raw_ptr, bool sync_with_validator);

#ifdef __cplusplus
}
#endif

#endif