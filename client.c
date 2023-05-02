#include "client.h"
#include "udp.c"

struct message {

    int identifier;
    int seq;
    int cid;
    int time_val;
    int key;
    int value;

};

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

void RPC_idle(struct rpc_connection *rpc, int time) {

}

int RPC_get(struct rpc_connection *rpc, int key) {

    return 0;
}

int RPC_put(struct rpc_connection *rpc, int key, int value) {

    // Post 1892
    // 1. Sending
    //send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, (char*) message, message->message_size);
    // 2. Receiving
    // 3. Blocking until Timeout
    // 4. Reattempt if no response
    // 5. Exit when retry over 5 times
    // If receive ACK, sleep 1 second, reset retry counts then back to step 1

    return 0;
}

void RPC_close(struct rpc_connection *rpc) {

    close_socket(rpc->recv_socket);

}
