
#ifndef CLIENTSERVER_H
#define CLIENTSERVER_H

struct message {
    int seq_number;
    int client_id;
    int time;
    int message_type;
    int key;
    int val;
    int length;
};

struct client_connection {

    int valid;
    int seq_number;
    int client_id;
    int last_result;

};

#endif