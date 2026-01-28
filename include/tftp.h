/* Common file for server & client*/

#ifndef TFTP_H
#define TFTP_H

// Libraries
#include <stdint.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>


#define PORT 6969
#define BUFFER_SIZE 516  // Maximum possible packet size
//[opcode(2)][block_number(2)][Data(0-512)]

/* NOTE:  TFTP OpCodes
 * RRQ: requesting server to send the data " Client <- Server "
 * WRQ: sending data from client to sever " Server -> Client "
 * DATA: to indicate the packet sended contains data
 * ACK: to indicate if the data is correctly recieved or not 
 * ERROR: if something went wrong and to stop the transfer 
 *       -> file not found
 *       -> permission denied 
 *       -> illegal opcode
 *       -> disk full
 */


#pragma pack (1)

/* NOTE: Main culprit of data getting lost
 * without pragma pack data was sended with padding there because of that 
 * hidden garbages was there when i tried to send data
 */

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


typedef enum {
    MODE_DEFAULT = 0,
    MODE_OCTET = 1,
    MODE_NETASCII = 2,
} tftp_mode;


/* NOTE: TFTP Packet Structure
 * opcode: first 2 bytes of every tftp packet, tells what kind of packet it is 
 * union: only one of these exist at a time 
 *        request: only sent once at start
 *        data_packet: transfer file data 
 *        ack_packet: ACK block_number must match the data block number
 *        error_packet: once ERROR is sent -> connection ends
 */



typedef struct {
    uint16_t opcode; // Operation code (RRQ/WRQ/DATA/ACK/ERROR)
    
    union {
        struct {
            char filename[256];
            uint16_t mode;  // Typically "octet"
        } request;  // RRQ and WRQ
        struct {
            uint16_t block_number; // packet number is block_number 
            char data[512];
            int data_size; // stores how much data we sent eg: 512 is overall send but content insdie is 100
        } data_packet; // DATA
        struct {
            uint16_t block_number;
            int data_size; // use this for debugging only not used for sending data
        } ack_packet; // ACK
        struct {
            uint16_t error_code;
            char error_msg[512];
        } error_packet; // ERROR
    } body;
} tftp_packet;

#pragma pack()

void send_file(int sockfd, struct sockaddr_in address, socklen_t client_len, char *filename);
void receive_file(int sockfd, struct sockaddr_in address, socklen_t client_len, char *filename);
status validate_filename(char* filename);
void set_mode();
const char* mode_to_string(tftp_mode mode);

#endif // TFTP_H
