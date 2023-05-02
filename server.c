#include "clientserver.h"
#include "client.h"
#include "udp.h"
#include "server_functions.c"
#include <pthread.h>



struct client_connection* call_table[100];

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Expected Usage: ./server <port>\n");
        exit(1);
    }

    int port = atoi(argv[1]);

    struct socket server_port = init_socket(port);
    struct message *curr_message;

    int curr_index = 101;
    int handled = 0;

    while(1) {

        struct packet_info curr_packet = receive_packet(server_port);
        curr_message = curr_packet.buf;

        for (int i = 0; i < 100; i++) {

            if (call_table[i]->client_id == curr_message->client_id) {
                curr_index = i;
                handled = 1;
                break;
            }

            if (call_table[i]->valid == 0 && i < curr_index) {
                curr_index = i;
            }

        }

        if (handled == 0) {

            call_table[curr_index]->valid = 1;
            call_table[curr_index]->seq_number = curr_message->seq_number;
            call_table[curr_index]->client_id = curr_message->client_id;

        } else {

            if (curr_message->seq_number < call_table[curr_index]->seq_number) {
                curr_index = 101;
                handled = 0;
                continue;
            } else if (curr_message->seq_number == call_table[curr_index]->seq_number) {
                // RESPOND FROM SERVER
            }

        }

        pthread_t thread_id;

        // Idle
        if (curr_message->message_type == 0) {

        // Get
        } else if (curr_message->message_type == 1) {

        // Put
        } else if (curr_message->message_type == 2) {

        }

        pthread_join(thread_id, NULL);

        curr_index = 101;
        handled = 0;

    }

}