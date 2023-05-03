#include "client.h"
#include "clientserver.h"
#include "udp.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int sequenceNo = 0;

struct rpc_connection RPC_init(int src_port, int dst_port, char dst_addr[]){
  struct rpc_connection rpc;
  struct sockaddr_storage addr;
  socklen_t addrlen;
  populate_sockaddr(AF_INET, dst_port, dst_addr, &addr, &addrlen);
  rpc.dst_addr = *(struct sockaddr *)(&addr);
  rpc.client_id = rand();
  rpc.recv_socket = init_socket(src_port);
  rpc.seq_number = sequenceNo;
  rpc.dst_len = addrlen;
  sequenceNo++;
  return rpc;
}

// idle 0, get 1, put 2
int RPC_put(struct rpc_connection *rpc, int key, int value){
  int retryCnt = 0;
  char buf[BUFLEN];
  rpc->seq_number = sequenceNo;
  sequenceNo++;
  struct response server_resp;
  while (retryCnt < 5) {
    struct message messagepayload;
    messagepayload.client_id = rpc->client_id;
    messagepayload.key = key;
    messagepayload.val = value;
    messagepayload.seq_number = rpc->seq_number;
    messagepayload.message_type = 2;
    memcpy(buf, &messagepayload, sizeof(struct message));
    send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, buf, sizeof(struct message));
    struct packet_info recPacket = receive_packet_timeout(rpc->recv_socket, 1);
    if (recPacket.recv_len < 0){
      retryCnt++;
      continue;
    }
    memcpy(&server_resp, recPacket.buf, sizeof(struct response));
    if (server_resp.client_id != messagepayload.client_id || server_resp.seq_number < messagepayload.seq_number) {
        continue;
    }
    if (server_resp.ack == 1) {
        sleep(1);
        continue;
    }
    return server_resp.result;
  }

  return -1;
}

int RPC_get(struct rpc_connection *rpc, int key){
  int retryCnt = 0;
  char buf[BUFLEN];
  rpc->seq_number = sequenceNo;
  sequenceNo++;
  struct response server_resp;
  while ( retryCnt < 5){
    struct message messagepayload;
    messagepayload.client_id = rpc->client_id;
    messagepayload.key = key;
    messagepayload.seq_number = rpc->seq_number;
    messagepayload.message_type = 1;
    memcpy(buf, &messagepayload, sizeof(struct message));
    send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, buf, sizeof(struct message));
    struct packet_info recPacket = receive_packet_timeout(rpc->recv_socket, 1);       
    if (recPacket.recv_len < 0){
      retryCnt++;
      continue;
    }
    memcpy(&server_resp, recPacket.buf, sizeof(struct response));
    if (server_resp.client_id != messagepayload.client_id || server_resp.seq_number < messagepayload.seq_number) {
        continue;
    }
    if (server_resp.ack == 1) {
        sleep(1);
        continue;
    }
    return server_resp.result;
  }

  return -1;
}

void RPC_idle(struct rpc_connection *rpc, int time){
  int retryCnt = 0;
  char buf[BUFLEN];
  rpc->seq_number = sequenceNo;
  sequenceNo++;
  struct response server_resp;
  while (retryCnt < 5) {
    struct message messagepayload;
    messagepayload.client_id = rpc->client_id;
    messagepayload.seq_number = rpc->seq_number;
    messagepayload.message_type = 0;
    messagepayload.time = time;
    memcpy(buf, &messagepayload, sizeof(struct message));
    send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, buf, sizeof(struct message));
    struct packet_info recPacket = receive_packet_timeout(rpc->recv_socket, 1);
    if (recPacket.recv_len < 0){
      retryCnt++;
      continue;
    }
    memcpy(&server_resp, recPacket.buf, sizeof(struct response));
    if (server_resp.client_id != messagepayload.client_id || server_resp.seq_number < messagepayload.seq_number) {
        continue;
    }
    if (server_resp.ack == 1) {
        sleep(1);
        continue;
    }
  }
}

void RPC_close(struct rpc_connection *rpc) {
  close_socket(rpc->recv_socket);
}