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

void rysujKolo();
int sprawdzPolozenieKursora(int x, int y);
void ustawWspolrzedne(char *wspolrzedne);
void ustawDanePoczatkowe(char *dane);
char *substring(int start, int stop, char *src, char *dst);

int main(int argc, char **argv)
{
  int sd,clen;
  char *data = (char*) calloc(10, sizeof(char));
  struct sockaddr_in sad, cad;

  if(argc != 2)
  {
    printf("Niepoprawne wywolanie programu.\n");
    printf("./klient ADRES_IP_SERWERA\n");
    return -1;
  }

  sd=socket(AF_INET,SOCK_DGRAM,0);
  //bzero(&sad, sizeof(sad));
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

  printf("id: %d, x: %d, y: %d\n", idZawodnika, wspX, wspY);

  while (1)
  {
    XNextEvent(mydisplay,&myevent);
    printf("event no. %5d\n",myevent.type);
    switch (myevent.type)
    {
      case ButtonPress:
        x = myevent.xmotion.x;
        y = myevent.xmotion.y;

        printf("nacisnieto przycisk x: %d y: %d\n", x, y);

        break;

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
          printf("roznica x: %d, roznica y: %d\n", roznicaX, roznicaY);

          wspX += roznicaX;
          wspY += roznicaY;
          rysujKolo();
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

void rysujKolo()
{
  XClearWindow(mydisplay, mywindow);
  XSetForeground(mydisplay,mygc,mycolor.pixel);
  XFillArc(mydisplay, mywindow, mygc, wspX-(100/2), wspY-(100/2), 100, 100, 0, 360*64);
}

int sprawdzPolozenieKursora(int x, int y)
{
  if(abs(wspX-x) <= 50 && abs(wspY-y) <= 50)
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
  printf("dane: %s", dane);
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

