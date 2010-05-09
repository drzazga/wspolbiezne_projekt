#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>

int main(int argc, char* argv[])
{
  int sd,buff=3,clen;
  struct sockaddr_in sad, cad;

  buff = atoi(argv[1]);

  sd=socket(AF_INET,SOCK_DGRAM,0);
  bzero(&sad, sizeof(sad));
  sad.sin_family=AF_INET;
  sad.sin_addr.s_addr=inet_addr("127.0.0.1");
  sad.sin_port=htons((ushort) 5000);
  bind(sd, (struct sockaddr *) &sad,sizeof(sad));
  clen = sizeof(cad);

  

  while(1)
  {
    buff=htonl(buff);
    sendto(sd, &buff, sizeof(int), 0, (struct sockaddr *) &sad, clen);
    buff = 0;

    recvfrom(sd, &buff, sizeof(int), 0, (struct sockaddr *) &cad, &clen);
    buff=ntohl(buff);

    printf("Mamy: %d\n", buff);
  }
  return 0;
}

