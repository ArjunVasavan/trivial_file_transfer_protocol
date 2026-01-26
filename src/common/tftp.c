#include "../../include/tftp.h"
#include <stdio.h>

void send_file(int sockfd, struct sockaddr_in address, socklen_t len, char *filename) {

    printf("inside sendfile\n");
    // Implement file sending logic here
    char buf[100];
    getcwd(buf,sizeof(buf));
    int fd = open(filename,O_RDONLY);

    if ( fd < 0 ) {
        perror("open failed");
        return;
    }

    uint16_t block_number = 1;

    int read_count = 0;

    tftp_packet packet;

    packet.opcode = htons(DATA);

    int last_was_full = 0;

    while ( (read_count = read(fd,packet.body.data_packet.data,512)) > 0 ) {


        printf("readed ->%s<-\n",packet.body.data_packet.data);

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

        if ( read_count == 512 ) {

            last_was_full  = 1;

        } else {
        
            last_was_full = 0;

        }

        block_number+=1;

        if ( read_count < 512 ) {

            break;
        }

        printf("Sended\n");

    }

    printf("exited\n");

    // NOTE: EDGE CASE => if last data size is mutliple of 512
    // the sender must send one extra data packet with 0 bytes

    if (last_was_full) {
        // send final DATA with 0 bytes
        memset(&packet,0,sizeof(packet));
        packet.opcode = htons(DATA);
        packet.body.data_packet.block_number = htons(block_number);
        unsigned long packet_length = 2 + 2;

    edge_case:

        sendto(sockfd, &packet, packet_length, 0,
               (struct sockaddr*)&address, len);

        // wait for ACK of this block

        tftp_packet ack_packet;

        int ack_pack_length = 2 + 2 ;
        // ack_pack_length = 2 (opcode) + 2(block_number) 

        struct sockaddr_in from_addr;
        socklen_t from_len = sizeof(from_addr);


        recvfrom(sockfd,&ack_packet,ack_pack_length,0,(struct sockaddr*)&from_addr,&from_len);

        int ack_block_number = ntohs(ack_packet.body.ack_packet.block_number);

        if ( ntohs(ack_packet.opcode) !=  ACK ) {

            goto edge_case;


        } else if ( ack_block_number != block_number ) {

            goto edge_case;

        }

        printf("Completed last_was_full case\n");

    }

    printf("Completed Sending\n");

    close(fd);
}

void receive_file(int sockfd, struct sockaddr_in address, socklen_t len, char *filename) 
{
    // Implement file receiving logic here 


    char fullpath[265];

    sprintf(fullpath,"src/server/%s",filename);


    int fd = open(fullpath,O_WRONLY|O_TRUNC|O_CREAT,0666); 

    printf("created %s\n",filename);

    unsigned long int expected_block = 1;

    tftp_packet data_packet;

    long int recieved_bytes;

    struct sockaddr_in client_addr;

    socklen_t client_len = sizeof(client_addr);

    tftp_packet ack_packet;

    while ( true ) {

    once_more_read:

        recieved_bytes = recvfrom(sockfd,&data_packet,BUFFER_SIZE,
                                  0,(struct sockaddr*)&client_addr,&client_len);

        printf("[BYTES RECIEVED]: %lu\n",recieved_bytes);

        uint16_t opcode = ntohs(data_packet.opcode);

        uint16_t block_number = ntohs(data_packet.body.data_packet.block_number);

        if ( opcode == DATA ) {

            if ( block_number != expected_block ) {

                ack_packet.opcode = htons(ACK);

                ack_packet.body.ack_packet.block_number = htons(expected_block - 1);

                // 4 byets => opcode + block_number

                sendto(sockfd,&ack_packet,4,0,(struct sockaddr*)&client_addr,client_len);

                goto once_more_read;

                // FIXME: modify goto later 

            }

            int data_length = recieved_bytes - 4 ; // 4 => opcode + block_number

            if ( data_length > 0 ) {

                write(fd,data_packet.body.data_packet.data,data_length);

            }

            ack_packet.opcode = htons(ACK);

            ack_packet.body.ack_packet.block_number = htons(expected_block);

            sendto(sockfd,&ack_packet,4,0,(struct sockaddr*)&client_addr,client_len);

            expected_block+=1;

            if ( data_length < 512 ) break;

        }

    }

    printf("Completed Recieving\n");

    close(fd);
}
