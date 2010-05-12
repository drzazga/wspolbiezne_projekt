#include <X11/Xlib.h>
#include <X11/X.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>

Display *mydisplay;
Window mywindow;
XSetWindowAttributes mywindowattributes;
XGCValues mygcvalues;
GC mygc;
Visual *myvisual;
int mydepth;
int myscreen;
int xw, yw;
int x,y;
Colormap mycolormap;
XColor mycolor, dummy;
XEvent myevent;
unsigned long i;
int wspX, wspY;
int oldX, oldY;
int roznicaX, roznicaY;
int idZawodnika;
int sd,clen;
struct sockaddr_in sad, cad;
char *zera[3] = { "", "0", "00" };
char *daneJakies;

void *odczytujNadchodzaceDane();
void rysujKolo();
int sprawdzPolozenieKursora(int x, int y);
void ustawWspolrzedne(char *wspolrzedne);
void ustawDanePoczatkowe(char *dane);
char *substring(int start, int stop, char *src, char *dst);
char *zwrocDaneDoWyslania();
char *zwrocIdZawodnika();
char *zwrocWspolrzedne();

int main(int argc, char **argv)
{
  char *data = (char*) calloc(10, sizeof(char));
  pthread_t tid;

  if(argc != 2)
  {
    printf("Niepoprawne wywolanie programu.\n");
    printf("./klient ADRES_IP_SERWERA\n");
    return -1;
  }

  sd=socket(AF_INET,SOCK_DGRAM,0);
  sad.sin_family=AF_INET;
  sad.sin_addr.s_addr=inet_addr(argv[1]);
  sad.sin_port=htons((ushort) 5000);
  bind(sd, (struct sockaddr *) &sad,sizeof(sad));
  clen = sizeof(cad);

  mydisplay = XOpenDisplay("");
  myscreen = DefaultScreen(mydisplay);
  myvisual = DefaultVisual(mydisplay,myscreen);
  mydepth = DefaultDepth(mydisplay,myscreen);
  mywindowattributes.background_pixel = XWhitePixel(mydisplay,myscreen);
  mywindowattributes.override_redirect = False;

  mywindow = XCreateWindow(mydisplay,XRootWindow(mydisplay,myscreen),
                          100,100,500,500,0,mydepth,InputOutput,
                          myvisual,CWBackPixel|CWOverrideRedirect,
                          &mywindowattributes);
  XSelectInput(mydisplay,mywindow,ExposureMask|KeyPressMask|ButtonPressMask|ButtonMotionMask|ButtonReleaseMask);
  mycolormap = DefaultColormap(mydisplay,myscreen);
  XMapWindow(mydisplay,mywindow);
  mygc = DefaultGC(mydisplay,myscreen);

  mycolormap = DefaultColormap(mydisplay,myscreen);
  XAllocNamedColor(mydisplay,mycolormap,"green",&mycolor,&dummy);

  oldX = -1;
  oldY = -1;

  sprintf(data, "init");
  sendto(sd, data, sizeof(data), 0, (struct sockaddr *) &sad, clen);
  recvfrom(sd, data, 10, 0, (struct sockaddr *) &cad, &clen);

  ustawDanePoczatkowe(data);

  pthread_create(&tid,NULL,odczytujNadchodzaceDane,NULL);

  //printf("id: %d, x: %d, y: %d\n", idZawodnika, wspX, wspY);

  char *wsp = (char*) calloc(6, sizeof(char));
  while(1)
  {
    recvfrom(sd, wsp, 6, 0, (struct sockaddr *) &cad, &clen);
    ustawWspolrzedne(wsp);
    rysujKolo();
  }
}

void rysujKolo()
{
  //printf("otrzymalem od serwera nastepujace dane: %d %d\n", wspX, wspY);
  XClearWindow(mydisplay, mywindow);
  XSetForeground(mydisplay,mygc,mycolor.pixel);
  XFillArc(mydisplay, mywindow, mygc, wspX-(100/2), wspY-(100/2), 100, 100, 0, 360*64);
  XFlush(mydisplay);
}

int sprawdzPolozenieKursora(int x, int y)
{
  if(sqrt(pow(wspX-x, 2.0) + pow(wspY-y, 2.0)) <= 50.0)
    return 1;
  else
    return 0;
}

void ustawWspolrzedne(char *wspolrzedne)
{
  char x[3];
  char y[3];

  substring(0, 3, wspolrzedne, x);
  substring(3, 7, wspolrzedne, y);

  wspX = atoi(x);
  wspY = atoi(y);
}

void ustawDanePoczatkowe(char *dane)
{
  char id[3];
  char wspolrzedneXY[6];

  substring(0, 3, dane, id);
  substring(3, 10, dane, wspolrzedneXY);

  idZawodnika = atoi(id);
  ustawWspolrzedne(wspolrzedneXY);
}

char *substring(int start, int stop, char *src, char *dst)
{
  int count = stop - start;
  sprintf(dst, "%.*s", count, src + start);
  return dst;
}

void *odczytujNadchodzaceDane()
{
  while (1)
  {
    XNextEvent(mydisplay,&myevent);
    //printf("event no. %5d\n",myevent.type);
    switch (myevent.type)
    {
      case ButtonRelease:
        oldX = -1;
        oldY = -1;
        break;

      case MotionNotify:
        x = myevent.xmotion.x;
        y = myevent.xmotion.y;

        if(sprawdzPolozenieKursora(x, y) == 1)
        {
          if(oldX == -1)
          {
            oldX = x;
            oldY = y;
          }
          roznicaX = x - oldX;
          roznicaY = y - oldY;

          oldX = x;
          oldY = y;

          wspX += roznicaX;
          wspY += roznicaY;

          daneJakies = zwrocDaneDoWyslania();
          sendto(sd, daneJakies, 10, 0, (struct sockaddr *) &sad, clen);
        }
        else
        {
          oldX = -1;
          oldY = -1;
        }
      break;

      case Expose:
        rysujKolo();
        break;

      case KeyPress:
        XCloseDisplay(mydisplay);
        exit(0);
    }
  }
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

char *zwrocIdZawodnika()
{
  char *mojeId = (char*) calloc(1, sizeof(char));

  if(idZawodnika < 10)
    sprintf(mojeId, "%s%d", zera[2], idZawodnika);
  else if(idZawodnika < 100 && idZawodnika > 9)
    sprintf(mojeId, "%s%d", zera[1], idZawodnika);
  else
    sprintf(mojeId, "%s%d", zera[0], idZawodnika);

  return mojeId;
}

char *zwrocDaneDoWyslania()
{
  char *daneDoWyslania = (char*) calloc(10, sizeof(char));
  sprintf(daneDoWyslania, "%s%s", zwrocIdZawodnika(), zwrocWspolrzedne());
  return daneDoWyslania;
}

