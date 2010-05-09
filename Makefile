CFLAGS=-L/usr/X11R6/lib -lX11 -lpthread

programy= klient server

all: klient server

clean :
	rm -f *~ \#* $(programy)
