#### Uncomment one of the following:
## Visual C++
CC = cl
CFLAGS = -GX -Os -G5
LFLAGS = wsock32.lib /Fe$@
OBJEXT = obj

## Borland C++
#CC = bcc32
#CFLAGS = -Os -5
#LFLAGS = -e$@
#OBJEXT = obj

## Cygwin/GNU C++
#CC = g++
#CFLAGS = -Wall -O
#LFLAGS = -lwsock32 -o $@
#OBJEXT = o


.SUFFIXES: .cpp .$(OBJEXT)

## Build rule for VC++ and BC++
.cpp.$(OBJEXT):
	$(CC) -c $(CFLAGS) $<

## Build rule for Cygwin/GNU C++
#.cpp.$(OBJEXT):
#	$(CC) -c $(CFLAGS) $< -o $*.o


#### Major targets #####################################################

all: basic-client.exe basic-server.exe threaded-server.exe \
		select-server.exe

ctags:
	ctags *.cpp *.h

clean:
	del *.$(OBJEXT) *.bak

distclean: clean
	del *.exe *.swp


#### Common dependencies ###############################################

main.$(OBJEXT): main.cpp
ws-util.$(OBJEXT): ws-util.cpp ws-util.h


#### Basic Winsock client ##############################################

BASIC_CLIENT_OBJS = basic-client.$(OBJEXT) main.$(OBJEXT) \
		ws-util.$(OBJEXT)
basic-client.exe: $(BASIC_CLIENT_OBJS)
	$(CC) $(BASIC_CLIENT_OBJS) $(LFLAGS)

basic-client.$(OBJEXT): basic-client.cpp ws-util.h


#### Basic Winsock server ##############################################

BASIC_SERVER_OBJS = basic-server.$(OBJEXT) main.$(OBJEXT) \
		ws-util.$(OBJEXT)
basic-server.exe: $(BASIC_SERVER_OBJS)
	$(CC) $(BASIC_SERVER_OBJS) $(LFLAGS)

basic-server.$(OBJEXT): basic-server.cpp ws-util.h


#### Threaded Winsock server ###########################################

THREADED_SERVER_OBJS = threaded-server.$(OBJEXT) main.$(OBJEXT) \
		ws-util.$(OBJEXT)
threaded-server.exe: $(THREADED_SERVER_OBJS)
	$(CC) $(THREADED_SERVER_OBJS) $(LFLAGS)

threaded-server.$(OBJEXT): threaded-server.cpp ws-util.h


#### select()-based Winsock server #####################################

SELECT_SERVER_OBJS = select-server.$(OBJEXT) main.$(OBJEXT) \
		ws-util.$(OBJEXT)
select-server.exe: $(SELECT_SERVER_OBJS)
	$(CC) $(SELECT_SERVER_OBJS) $(LFLAGS)

select-server.$(OBJEXT): select-server.cpp ws-util.h


