# How to udpate rust code from libra client

1. git merge-file src/client_proxy.rs $LIBRA/client/cli/src/client_proxy.rs src/client_proxy.rs
   git merge-file src/libra_client.rs $LIBRA/client/cli/src/libra_client.rs src/libra_client.rs

2. resolve the conflicts