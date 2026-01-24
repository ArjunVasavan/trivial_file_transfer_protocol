#include "../../include/tftp.h"
#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>

void send_file(int sockfd, struct sockaddr_in address, socklen_t len, char *filename) 
{
    // Implement file sending logic here

    char buf[100];
    getcwd(buf,sizeof(buf));
    printf("here the cwd is %s\n",buf);

    printf("file name is %s\n",filename);
    int fd = open(filename,O_RDONLY);

    if ( fd < 0 ) {
        perror("open failed");
        return;
    }

    uint16_t block_number = 1;

    int read_count = 0;

    tftp_packet packet;

    packet.opcode = htons(DATA);

    while ( (read_count = read(fd,packet.body.data_packet.data,512)) > 0 ) {

        packet.body.data_packet.block_number = htons(block_number);

        packet.body.data_packet.data_size = read_count;

        unsigned long int packet_length = 2 + 2 + read_count;

        // PACKET LENGTH => 2 bytes(opcode) + 2 bytes (block_number) + read_count


    once_more_send:

        sendto(sockfd,&packet,packet_length,0,(struct sockaddr*)&address,len);

        /*
         * NOTE: How sendto sends data from start of address to packet_length
         * data is sended from [start of address: &packet -> packet_length]  
         * when i fill the data in union of this tftp_packet type
         * here memory become like this =>> [ opcode ][ block ][ data bytes ][ rest garbage ]
         *                                  start address -->
         * in union all the unnecessery type we doesnt used will become garbage and will be only 
         * after the essential data we added
         * * Union members do not have their own particular region in memory  
         * * they overlap at the same address i.e. which type is used will be at first
         *   rest garbage will be after the essential types
         * * so sending start address plus exact length sends only the intended fields.
        */

        tftp_packet ack_packet;

        int ack_pack_length = 2 + 2 ;
        // ack_pack_length = 2 (opcode) + 2(block_number) 

        struct sockaddr_in from_addr;
        socklen_t from_len = sizeof(from_addr);


        recvfrom(sockfd,&ack_packet,ack_pack_length,0,(struct sockaddr*)&from_addr,&from_len);

        int ack_block_number = ntohs(ack_packet.body.ack_packet.block_number);

        if ( ntohs(ack_packet.opcode) !=  ACK ) {

            goto once_more_send;


        } else if ( ack_block_number != block_number ) {

            // FIXME: modify goto section logic
            // fix: wrong op code
            // recvfrom failure
            goto once_more_send;

        }

        // ELSE : ACK is valid


        if ( read_count < 512 ) {

            break;
        }

        block_number+=1;

    }

    close(fd);
}

void receive_file(int sockfd, struct sockaddr_in address, socklen_t len, char *filename) 
{
    // Implement file receiving logic here
}

