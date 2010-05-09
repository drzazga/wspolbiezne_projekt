#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>

typedef struct
{
  int id;
  struct sockaddr adres;
} zawodnik;

void *wyslijPozycjeKola();
void wyslijDanePoczatkowe(struct sockaddr *adres);
char *zwrocWspolrzedne();
int wspX, wspY;
int sd, clen;
char *zera[3] = { "", "0", "00" };
int idZawodnika;
//zawodnik tablicaZawodnikow[999];

int main()
{
  char *data = (char*) calloc(1, sizeof(char));
  pthread_t tid;
  struct sockaddr_in sad,cad;

  signal(SIGHUP,SIG_IGN);
  sd=socket(AF_INET,SOCK_DGRAM,0);
  //bzero((char *) &sad, sizeof(sad));
  sad.sin_family=AF_INET;
  sad.sin_addr.s_addr=htonl(INADDR_ANY);
  sad.sin_port=htons((ushort) 5000);
  bind(sd,(struct sockaddr *) &sad,sizeof(sad));
  clen=sizeof(cad);

  idZawodnika = 0;
  wspX = 100-(15/2);
  wspY = 200-(15/2);

  //pthread_create(&tid,NULL,wyslijPozycjeKola,NULL);

  while (1)
  {
    recvfrom(sd, data,sizeof(data),0,(struct sockaddr *) &cad,&clen);

    if(strcmp(data, "init") == 0)
    {
      wyslijDanePoczatkowe((struct sockaddr *) &cad);
    }
    else
    {
      sendto(sd, data,sizeof(data),0,(struct sockaddr *) &cad,clen);
    }
  }

  return 0;
}

void *wyslijPozycjeKola()
{
  return;
}

void wyslijDanePoczatkowe(struct sockaddr *cad)
{
  char *danePoczatkowe = (char*) calloc(1, sizeof(char));

  if(idZawodnika < 10)
    sprintf(danePoczatkowe, "%s%d%s", zera[2], idZawodnika, zwrocWspolrzedne());
  else if(idZawodnika < 100 && idZawodnika > 9)
    sprintf(danePoczatkowe, "%s%d%s", zera[1], idZawodnika, zwrocWspolrzedne());
  else
    sprintf(danePoczatkowe, "%s%d%s", zera[0], idZawodnika, zwrocWspolrzedne());

  printf("dane poczatkowe: %s\n", danePoczatkowe);

  sendto(sd, danePoczatkowe, 10, 0, cad, clen);

  idZawodnika++;
}

char *zwrocWspolrzedne()
{
  char *wspolrzedne = (char*) calloc(1, sizeof(char));
  char *wspolrzednaX = (char*) calloc(1, sizeof(char));
  char *wspolrzednaY = (char*) calloc(1, sizeof(char));

  if(wspX < 10)
    sprintf(wspolrzednaX, "%s%d", zera[2], wspX);
  else if(wspX < 100 && wspX > 9)
    sprintf(wspolrzednaX, "%s%d", zera[1], wspX);
  else
    sprintf(wspolrzednaX, "%s%d", zera[0], wspX);

  if(wspY < 10)
    sprintf(wspolrzednaY, "%s%d", zera[2], wspY);
  else if(wspY < 100 && wspY > 9)
    sprintf(wspolrzednaY, "%s%d", zera[1], wspY);
  else
    sprintf(wspolrzednaY, "%s%d", zera[0], wspY);

  sprintf(wspolrzedne, "%s%s", wspolrzednaX, wspolrzednaY);

  return wspolrzedne;
}

