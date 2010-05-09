#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>

void *wyslijPozycjeKola();

int main()
{
  int sd,buff,clen;
  char *data = (char*) calloc(1, sizeof(char)); 
  pthread_t tid;  
  struct sockaddr_in sad,cad;

  signal(SIGHUP,SIG_IGN);
  sd=socket(AF_INET,SOCK_DGRAM,0);
  bzero((char *) &sad, sizeof(sad));
  sad.sin_family=AF_INET;
  sad.sin_addr.s_addr=htonl(INADDR_ANY);
  sad.sin_port=htons((ushort) 5000);
  bind(sd,(struct sockaddr *) &sad,sizeof(sad));
  clen=sizeof(cad);

  //pthread_create(&tid,NULL,wyslijPozycjeKola,NULL);

  while (1)
  {
   recvfrom(sd,(char *) &buff,sizeof(int),0,(struct sockaddr *) &cad,&clen);
   buff=ntohl(buff);

   buff=htonl(buff);
   sendto(sd,(char *) &buff,sizeof(int),0,(struct sockaddr *) &cad,clen);
  }

  return 0;
}

void *wyslijPozycjeKola()
{
  return;
}
