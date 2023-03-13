#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handling(char *message); //  error handling function
char * calculate(char *message);    //  calculate and make result fucntion

int main(int argc, char *argv[]){
    int serv_sock;  //  make server socket
    int clnt_sock;  //  make client socket

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    int recv_size;
    int recv_count;

    char message[BUF_SIZE];     //  packet pointer
    char *result;   //  result pointer

    //  check argc parameter
    if(argc != 2){
        printf("Usage : %s <port>\n",argv[0]);
        exit(1);
    }

    //  make server socket
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    //  error check to make server socket 
    if(serv_sock == -1){
        error_handling("socket() error");
    }

    //  server socket addressing
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    //  server socket binding, error check
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
        error_handling("bind() error");
        exit(1);
    }

    //  server socket listening, error check
    if(listen(serv_sock, 5) == -1){
        error_handling("listen() error");
        exit(1);
    }

    //  accept client socket, error check
    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if(clnt_sock == -1){
        error_handling("accept() error");
        exit(1);
    }

    recv_size = 0;
    recv_count = 0;
    
    //  if you receive packet in once, try to receive all size packet     
    while(BUF_SIZE > recv_size){
        recv_count = read(clnt_sock, message, BUF_SIZE);
        if(recv_count == -1){
            error_handling("read() error");
        }   
        recv_size += recv_count;
    }

    //  calculate and return result
    result = calculate(message);

    //  send result to client socket
    write(clnt_sock, result, sizeof(int));

    //  close server socket 
    close(clnt_sock);
    //  close client socket
    close(serv_sock);

    return 0;
}

//  error handling function
void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

//  calculate and make result fucntion
char * calculate(char *message){
    int count;      //  oeprand count
    char operator;  //  operator
    int *result;    //  result pointer 
    int *operand;   //  operand pointer
    int i;          //  use for loop

    //  extract operand count from packet
    count = (int)message[0];

    //  extract operator from packet
    operator = message[count * sizeof(int) + 1];

    //  extract operand from packet and calculate
    result = (int *)&message[1];
    for(i=1; i<count; i++){
        operand = (int *)&message[i * sizeof(int) + 1];

        if(operator == '+')
            *result += *operand;
        else if(operator == '-')
            *result -= *operand;
        else if(operator == '*')
            *result *= *operand;

    }

    //  return the result of type casting to char 
    return (char *)result;
}
