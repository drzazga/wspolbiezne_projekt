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
  int wspX;
  int wspY;
  int odebraneWObecnejIteracji;
  struct sockaddr_in adres;
} zawodnik;

void *wyslijPozycjeKola();
void wyslijDanePoczatkowe(struct sockaddr *adres);
char *zwrocWspolrzedne();
char *substring(int start, int stop, char *src, char *dst);
void ustawWspolrzedne();

int wspX, wspY;
int sd, clen;
char *zera[3] = { "", "0", "00" };
int idZawodnika;
int idOdebrane;
int xOdebrane;
int yOdebrane;
char id[3];
char x[3];
char y[3];

zawodnik tablicaZawodnikow[999];

int main()
{
  char *data = (char*) calloc(10, sizeof(char));
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

  pthread_create(&tid,NULL,wyslijPozycjeKola,NULL);

  while (1)
  {
    recvfrom(sd, data,10,0,(struct sockaddr *) &cad,&clen);

    if(strcmp(data, "init") == 0)
    {
      tablicaZawodnikow[idZawodnika].adres = cad;
      wyslijDanePoczatkowe((struct sockaddr *) &cad);
      idZawodnika++;
    }
    else
    {
      substring(0, 3, data, id);
      substring(3, 6, data, x);
      substring(6, 9, data, y);
      idOdebrane = atoi(id);
      xOdebrane = atoi(x);
      yOdebrane = atoi(y);

      tablicaZawodnikow[idOdebrane].wspX = xOdebrane;
      tablicaZawodnikow[idOdebrane].wspY = yOdebrane;
      tablicaZawodnikow[idOdebrane].odebraneWObecnejIteracji = 1;
    }
  }

  return 0;
}

void *wyslijPozycjeKola()
{
  int i;
  char *wspDoWyslania = (char*) calloc(6, sizeof(char));;

  while(1)
  {
    usleep(200000);

    wspDoWyslania = zwrocWspolrzedne();

    for(i = 0; i < idZawodnika; i++)
    {
      sendto(sd, wspDoWyslania, 6, 0, (struct sockaddr *) &tablicaZawodnikow[i].adres, clen);
    }
  }
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

  //printf("dane poczatkowe: %s\n", danePoczatkowe);

  sendto(sd, danePoczatkowe, 10, 0, cad, clen);
}

char *zwrocWspolrzedne()
{
  char *wspolrzedne = (char*) calloc(1, sizeof(char));
  char *wspolrzednaX = (char*) calloc(1, sizeof(char));
  char *wspolrzednaY = (char*) calloc(1, sizeof(char));

  ustawWspolrzedne();

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

char *substring(int start, int stop, char *src, char *dst)
{
  int count = stop - start;
  sprintf(dst, "%.*s", count, src + start);
  return dst;
}

void ustawWspolrzedne()
{
  int i, iloscOdebranych = 0;
  int x, y;
  for(i = 0; i < idZawodnika; i++)
  {
    if(tablicaZawodnikow[i].odebraneWObecnejIteracji == 1)
    {
      x += tablicaZawodnikow[i].wspX;
      y += tablicaZawodnikow[i].wspY;
      tablicaZawodnikow[i].odebraneWObecnejIteracji = 0;
      iloscOdebranych++;
    }
  }

  if(iloscOdebranych != 0)
  {
    wspX = x / iloscOdebranych;
    wspY = y / iloscOdebranych;
  }
}

