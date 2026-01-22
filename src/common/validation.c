#include "../../include/tftp.h"    
#include "../client/tftp_client.h"

status validate_ip(char* ip_address) {

    struct in_addr dummy;

    /* NOTE: inet_pton for IP address validation
     * Internet Presentation TO Network
     * converts human readable string to binary format (used by sockets)
     * using AF_INET it will try to validate if its IPv4 only
     * it will also reject invalid number
     * it will return '1' if its valid else any other number will come
     * ~>The reason i used dummy is that if i put NULL on that function it will
     *   lead to segmentaion fault
     */

    if ( inet_pton(AF_INET,ip_address,&dummy) != 1 ) {

        fprintf(stderr,"[ERROR]: Invalid IP address\n");
        return FAILURE;

    } else {

        return SUCCESS;

    }

}

status validate_port(int port_number) {

    /* Ports are 16-bit → max = 2^16 - 1 = 65535
       User ports start from 1024 */


    if ( port_number >= 1024 && port_number <= 65535 ) {
        return SUCCESS;
    }

    fprintf(stderr,"[ERROR]: Invalid Port Number\n");
    return FAILURE;
}



status read_client(char* ip_address,int* port_number) {

    printf("Enter IP address: ");
    scanf("%s",ip_address);

    if ( validate_ip(ip_address) == FAILURE ) {
        return FAILURE;
    }

    printf("Enter Port Number: ");
    scanf("%d",port_number);

    if (validate_port(*port_number) == FAILURE ) {
        return FAILURE;
    }

    return SUCCESS;
}

status validate_filename(char* filename) {

    
    

    return SUCCESS;
}
