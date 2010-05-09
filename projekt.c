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

main()
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
  XSelectInput(mydisplay,mywindow,ExposureMask|KeyPressMask);
  mycolormap = DefaultColormap(mydisplay,myscreen);
  XMapWindow(mydisplay,mywindow);
  mygc = DefaultGC(mydisplay,myscreen);

  mycolormap = DefaultColormap(mydisplay,myscreen);
  XAllocNamedColor(mydisplay,mycolormap,"green",&mycolor,&dummy);

  while (1)
  {
    XNextEvent(mydisplay,&myevent);
    printf("event no. %5d\n",myevent.type);
    switch (myevent.type)
    {
     case MotionNotify:
    /* store the mouse button coordinates in 'int' variables. */
    /* also store the ID of the window on which the mouse was */
    /* pressed.                                               */
    x = myevent.xmotion.x;
    y = myevent.xmotion.y;
    //the_win = an_event.xbutton.window;

    /* if the 1st mouse button was held during this event, draw a pixel */
    /* at the mouse pointer location.                                   */
    if (myevent.xmotion.state & Button1Mask) {
        /* draw a pixel at the mouse position. */
        XDrawPoint(mydisplay, mywindow, mygc, x, y);
    }
    break;

      case Expose:
        XSetForeground(mydisplay,mygc,mycolor.pixel);
        //XFillRectangle(mydisplay,mywindow,mygc,100,100,300,300);
        XDrawArc(mydisplay, mywindow, mygc, 50-(15/2), 100-(15/2), 100, 100, 0, 360*64);
        //XDrawArc(mydisplay, mywindow, mygc, 100, 100, 50, 50, 0.0, 1);
        //XDrawLine(mydisplay,mywindow,mygc,100,400,400,100);
        XSetFunction(mydisplay,mygc,GXcopy);
        XFlush(mydisplay);
        break;

      case KeyPress:
        XCloseDisplay(mydisplay);
        exit(0);
    }
  }
}

