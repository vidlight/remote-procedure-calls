#include "client.h"

struct rpc_connection RPC_init(int src_port, int dst_port, char dst_addr[]){
    struct rpc_connection rpc;
    struct sockaddr_storage addr;
    socklen_t addrlen;
    populate_sockaddr(AF_INET, dst_port, dst_addr, &addr, &addrlen);
    rpc.dst_addr = *((struct sockaddr *)(&addr));
    rpc.client_id = rand();
    rpc.recv_socket = init_socket(src_port);
    rpc.seq_number = "?";

    return rpc;
}
