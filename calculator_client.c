#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handling(char *message);  //  error handling function
char * make_packet(char *message);   //  make packet function

int main(int argc, char *argv[]){
    int sock;
    struct sockaddr_in serv_addr;
    char message[BUF_SIZE];
    int str_len;

    int *result;    //  calculate result pointer
    char *packet;   //  packet pointer

    //  check argc parameter 
    if(argc != 3){
        printf("Usage : %s <IP> <PORT>", argv[0]);
        exit(1);
    }

    //  make socket
    sock = socket(PF_INET, SOCK_STREAM, 0);
    //  error check to make socket
    if(sock == -1){
        error_handling("socket() error");
    }

    //  socket addressing
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    //  connect to server_socket, and error check
    if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
        error_handling("connect() error");
    }

    //  socket connected 
    printf("Connected..........\n");
    
    //  operand count, operand, operator in one packet
    packet = make_packet(message);

    //  send packet to server
    write(sock, packet, BUF_SIZE);

    //  read packet from server
    str_len = read(sock, message, sizeof(int));
    //  if read fucntion returned -1, it's error 
    if(str_len == -1){
        error_handling("read() error");
    }

    //  recv_packet type casting to integer pointer
    result = (int *)message; 

    //  print result
    printf("Operation result : %d \n", *result);
    
    //  socket close
    close(sock);

    return 0;
}

//  error handling function
void error_handling(char *message){
    //  just print error message, and exit program
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

//  make packet function
char * make_packet(char *message){
    int count;      //  operand count
    char operator;  //  operator
    int i;          //  use for loop

    //  input operand count
    printf("Operand count: ");
    scanf("%d",&count);

    //  input operand count in message[0]
    message[0] = (char)count;

    //  input operand in message array
    for(i=0; i<count; i++){
        printf("Operand %d:", i+1);
        scanf("%d",(int *)&message[sizeof(int) * i + 1]);
        getchar();  //  flush input buffer
    }

    //  input operator
    printf("Operator: ");
    scanf("%c", &operator);

    //  after input operand, input operator in end of the point 
    message[sizeof(int) * count + 1] = operator;

    return message;
}