
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define PORT 5001   //The port on which to listen for incoming data
#define SERVER "192.168.1.10"

int main()
{
    struct sockaddr_in si_me, si_other;
    int s;

    //int buff_size=2048;
    //int buff_size=64*1024;
    int buff_size = 128 * 1024 * 1024;
    int array_size = buff_size / sizeof(int);

    int* buff_int = malloc(buff_size);
    int* recv_buff_int = malloc(buff_size);
    char* buff, * recv_buff;

    int i, n;
    int bytes_left;

    int max_transfers = 16;
    int curr_tr;

    //create a TCP socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    // zero out the structure
    memset((char*)&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    memset((char*)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    inet_aton(SERVER, &si_other.sin_addr);
    //bind socket to port
    connect(s, (struct sockaddr*)&si_other, sizeof(si_other));
    for (i = 0; i < array_size; i++)
    {
        buff_int[i] = i;
    }
    //start send
    printf("Sending %d bytes\n", buff_size);
    for (curr_tr = 0; curr_tr < max_transfers; curr_tr++)
    {
        bytes_left = buff_size;
        buff = (char*)buff_int;
        while (bytes_left > 0)
        {
            n = send(s, buff, bytes_left, 0);
            bytes_left -= n;
            buff += n;
            printf("Sent bytes: %d, remaining bytes: %d\n", n, bytes_left);
        }
    }
    //start receive
    for (curr_tr = 0; curr_tr < max_transfers; curr_tr++)
    {
        bytes_left = buff_size;
        recv_buff = (char*)recv_buff_int;
        while (bytes_left > 0)
        {

            n = recv(s, recv_buff, bytes_left, 0);
            bytes_left -= n;
            recv_buff += n;
            //printf("Received bytes: %d, remaining bytes: %d\n",n,bytes_left);
        }
    }
    /*   for(i=0; i<array_size; i++)
       {
           //if (i == 0 || i==array_size-1)
           {
               printf("buff[%d] %d, recv_buff[%d] %d\n",i,buff_int[i],i,recv_buff_int[i]);
           }
       }*/
    return 0;
}