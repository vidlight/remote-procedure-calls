#include "clientserver.h"
#include "client.h"
#include "udp.h"
#include "server_functions.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct client_connection call_table[100];

void *thread_helper(void *void_args) {

    struct params *args = (struct params*) void_args;
    char* ret = "blank return";

    // Idle
    if (args->type == 0) {
        idle(args->time);
    // Get
    } else if (args->type == 1) {
        args->resp.result = get(args->key);
    // Put
    } else if (args->type == 2) {
        int test = put(args->key, args->val);
        args->resp.result = test;
    }

    pthread_exit(ret);
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Expected Usage: ./server <port>\n");
        exit(1);
    }

    int port = atoi(argv[1]);

    struct socket server_port = init_socket(port);
    struct message curr_message;
    char buf[BUFLEN];

    int curr_index = 101;
    int handled = 0;

    while(1) {

        struct packet_info curr_packet = receive_packet(server_port);
        memcpy(&curr_message, curr_packet.buf, sizeof(struct message));

        for (int i = 0; i < 100; i++) {

            if (call_table[i].client_id == curr_message.client_id) {
                curr_index = i;
                handled = 1;
                break;
            }

            if (call_table[i].valid == 0 && i < curr_index) {
                curr_index = i;
            }

        }

        if (handled == 0) {

            call_table[curr_index].valid = 1;
            call_table[curr_index].seq_number = curr_message.seq_number;
            call_table[curr_index].client_id = curr_message.client_id;

        } else {

            if (curr_message.seq_number < call_table[curr_index].seq_number) {
                curr_index = 101;
                handled = 0;
                continue;
            } else if (curr_message.seq_number == call_table[curr_index].seq_number) {
                // RESPOND FROM SERVER
                curr_index = 101;
                handled = 0;
                struct response ack_response;
                ack_response.ack = 1;
                ack_response.client_id = curr_message.client_id;
                ack_response.seq_number = curr_message.seq_number;
                ack_response.result = call_table[curr_index].last_result;
                memcpy(buf, &ack_response, sizeof(struct response));
                send_packet(server_port, curr_packet.sock, curr_packet.slen, buf, sizeof(struct response));
            }

        }

        pthread_t thread_id;

        struct response rpc_resp;
        struct params args;

        rpc_resp.client_id = curr_message.client_id;
        rpc_resp.seq_number = curr_message.seq_number;

        args.resp = rpc_resp;
        args.key = curr_message.key;
        args.time = curr_message.time;
        args.val = curr_message.val;
        args.type = curr_message.message_type;

        pthread_create(&thread_id, NULL, thread_helper, &args);

        // Adjust call table
        call_table[curr_index].last_result = args.resp.result;
        call_table[curr_index].seq_number = curr_message.seq_number;

        printf("Thread test: %d\n", args.resp.result);

        memcpy(buf, &(args.resp), sizeof(struct response));
        send_packet(server_port, curr_packet.sock, curr_packet.slen, buf, sizeof(struct response));

        pthread_join(thread_id, NULL);

        curr_index = 101;
        handled = 0;

    }

}