CC:=gcc
CFLAGS:=-pipe -mtune=native -I. -g3
LDFLAGS:=-lz

LIB_OBJS:=lib/libDB.o lib/libCurse.o main.o 
LIBHTTP_OBJS:=lib/libHttp.o lib/compat/CompatSocket.o

LIBZIP_SRCS:= $(shell ls -1 lib/zip/*.c)
LIBZIP_OBJS:= $(LIBZIP_SRCS:.c=.o)

all: curse
	
clean:
	rm -f $(LIB_OBJS) $(LIBHTTP_OBJS) $(LIBZIP_OBJS)

curse: $(LIB_OBJS) $(LIBHTTP_OBJS) $(LIBZIP_OBJS)
	$(CC) $(LDFLAGS) -o $@ $?
