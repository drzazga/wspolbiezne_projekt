#include <X11/Xlib.h>
#include <X11/X.h>
#include <stdio.h>
#include <stdlib.h>

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

void rysujKolo();
int sprawdzPolozenieKursora(int x, int y);

int main()
{
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

  wspX = 200-(15/2);
  wspY = 200-(15/2);
  oldX = -1;
  oldY = -1;

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
