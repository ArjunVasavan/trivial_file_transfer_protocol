/* Common file for server & client*/

#ifndef TFTP_H
#define TFTP_H

#include <stdint.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define PORT 6969
#define BUFFER_SIZE 516  // TFTP data packet size (512 bytes data + 4 bytes header)

// TFTP OpCodes
typedef enum {
    RRQ = 1,  // Read Request
    WRQ = 2,  // Write Request
    DATA = 3, // Data Packet
    ACK = 4,  // Acknowledgment
    ERROR = 5 // Error Packet
} tftp_opcode;

typedef enum {
    SUCCESS = 0,
    FAILURE = 1
} status;


// TFTP Packet Structure
typedef struct {
    uint16_t opcode; // Operation code (RRQ/WRQ/DATA/ACK/ERROR)
    /*
     NOTE: all the sending packets are happened here
     based on opcode enum we does the Operation
    */
    union {
        struct {
            char filename[256];
            int mode[8];  // Typically "octet"
        } request;  // RRQ and WRQ
        struct {
            uint16_t block_number; // packet number is block_number 
            char data[512];
            int data_size; // stores how much data we sent eg: 512 is overall send but content insdie is 100
        } data_packet; // DATA
        struct {
            uint16_t block_number;
            int data_size;
        } ack_packet; // ACK
        struct {
            uint16_t error_code;
            char error_msg[512];
        } error_packet; // ERROR
    } body;
} tftp_packet;

void send_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename);
void receive_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename);

status validate_filename(char* filename);

#endif // TFTP_H
