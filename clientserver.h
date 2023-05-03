
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

struct response {
    int ack;
    int result;
    int client_id;
    int seq_number;
};

struct params {
    int key;
    int val;
    int time;
    int type;
    struct response resp;
};

struct client_connection {

    int valid;
    int seq_number;
    int client_id;
    int last_result;

};

#endif