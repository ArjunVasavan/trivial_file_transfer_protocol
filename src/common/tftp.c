/* Common file for server & client */

#include "../../include/tftp.h"

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

    int block_number = 1;

    int read_count = 0;

    tftp_packet packet;

    packet.opcode = DATA;

    while ( (read_count = read(fd,packet.body.data_packet.data,512)) > 0 ) {
        
        packet.body.data_packet.block_number = block_number;

        packet.body.data_packet.data_size = read_count;

        unsigned long int packet_length = sizeof(packet.opcode) + 
                                          sizeof(packet.body.data_packet.block_number) +
                                          sizeof(packet.body.data_packet.data_size);

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

        int ack_pack_length = sizeof(ack_packet.opcode) + sizeof(ack_packet.body.ack_packet);

        // recvfrom(sockfd,&ack_packet,, int flags, struct sockaddr *restrict addr, socklen_t *restrict addr_len)


    }

}

void receive_file(int sockfd, struct sockaddr_in address, socklen_t len, char *filename) 
{
    // Implement file receiving logic here
}

