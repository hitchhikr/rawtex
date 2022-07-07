LIBS = -lgtk-win32-2.0.dll -lgdk-win32-2.0.dll -lgdk_pixbuf-2.0.dll -lgmodule-2.0.dll -lgobject-2.0.dll -latk-1.0.dll -lpangowin32-1.0.dll -lglib-2.0.dll
FLAGS = -O2 -c -mms-bitfields -D G_OS_WIN32
all: RemObj RAWTex

RemObj:
	-rm -f rawtex.o

RAWTex: rawtex.o
	gcc -mwindows -o RAWTex rawtex.o $(LIBS)
	strip -s RAWTex.exe
	-rm -f rawtex.o

rawtex.o: src/rawtex.c
	gcc -o rawtex.o $(FLAGS) src/rawtex.c
