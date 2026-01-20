# [Trivial File transfer protocol]

**Notes:**
It is somewhat like shareit or xender
in tvtp it will work like half duplex
server will be on background
client will be on foreground
udp is used because tftp works on udp
Server and client should have their own directory

## Main Flow
**Client**
- [ ] MENU
- [ ] Step 1: connect
- [ ] Step 2: put
- [ ] Step 3: get
- [ ] Step 4: mode
- [ ] Step 5: Exit


**Client Flow**

- [ ] Step 1: Read IP address and port number
- [ ] Step 2: Validate IP and port number {Numeric only+ this much dot on;y eg: 1270.0.0.1 } {port -> 1024 -> 2^16-1 range only}
- [ ] Step 3: back to main Menu  

## [MENU]
**Description:** [inside main function]

- [ ] Ask the user to enter the option


## [Connect]
**Description:** [Just basic connection of server and client]


## [PUT]
**Description:** [Send data from Client to Server]


**1. Client Side**
happens on client 

- [ ] Read File name Which you want to senf
- [ ] Validate File Exist or note (if O_CREAT is not present open will return -1 if file doesnt exist )
    - [ ] if ( file doesnt exist ) => print error message then back to main menu  
    - [ ] if ( file exist ) => send the file name and operation to the server 
    **Notes:**
    SERVER SIDE DOES THE WRITE operation
    format => file.txt.WWR 

**2. Server Side**

- [ ] Create Socket And Make it Wait for Client
- [ ] use recvfrom ( it wait until data is recieved ) => recieves file name and operation 
- [ ] based on operation [WWR -> write] [RRD -> read]
    **WWR**
    - [ ] Validate file exist or not
        - [ ] if file doesnt exist create an file with same name where user given 
        - [ ] if file exist overwrite the file 
- [ ] gives acknowledgement to client (SUCCESS/FAILURE) should be send
- [ ] 

**3. Client Side**

- [ ] {3.0} Recieve the acknowledgement from server (SUCCESS/FAILURE)
    - [ ] if ( FAILURE ) => print error and go back to maun menu
    - [ ] if ( SUCCESS ) => Start sending the data from file
- [ ] We have to send data packet by packet (to avoid data loss) => Packet size is 512 bytes
- [ ] {3.1} Read 512 Bytes of data from file then send 512 byte if data with packed number and data size 
      ( thus reciever and understand how much is acutal data inside 512 bytes)

**4. Server Side**

- [ ] Recievs 512 byte from client and all other size and packet number
- [ ] write the data which server recieved from client 
- [ ] send acknowledgement back to client [Packet size of recieved and data size which it recieved]

**5. Client**

- [ ] Client recieves the acknowledgement [Packet number and data size ]
- [ ] if  ( acknowledgement both are same  ) => send next data. else if ( both are not same ) => send that data once more

**Loop Flow**

3.1 to 5 will be in an Loop Until EOF
once the loop is completed client should go back to main menu
server  should be back to 3.0
