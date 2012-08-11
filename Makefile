# CHANGE IF NEEDED
DEVKEY=4601467a7a6b84ea18a7975639e82e28

GCC=gcc
CFLAGS=-std=c99 -Wall -pedantic
EXECUTABLE=pb
LIBNAME=libpastebin.so
LIBPATH=lib
BINPATH=bin
VPATH=src
INCLUDE=-Iinclude
LIBS=-lcurl

.SILENT:

all: $(EXECUTABLE)

pb_debug: main.c pastebin.c
	$(GCC) -DRSHDEBUG -DPB_CLIENT_API_KEY=\"$(DEVKEY)\" $(CFLAGS) $(INCLUDE) $^ $(LIBS) -o $(BINPATH)/$@

$(EXECUTABLE): main.c pastebin.c
	$(GCC) -DPB_CLIENT_API_KEY=\"$(DEVKEY)\" $(CFLAGS) $(INCLUDE) $^ $(LIBS) -o $(BINPATH)/$@

.PHONY library: $(LIBNAME)

$(LIBNAME): pastebin.c
	$(GCC) $(CFLAGS) $(INCLUDE) $^ -fPIC -shared -o $(LIBPATH)/$(LIBNAME)

clean:
	rm -f $(BINPATH)/$(EXECUTABLE) $(LIBPATH)/$(LIBNAME) $(BINPATH)/pb_debug
