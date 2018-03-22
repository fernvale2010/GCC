
.SUFFIXES: .c .o .h

SYSROOT=
CROSS_COMPILE=
#CC=$(CROSS_COMPILE)gcc --sysroot=$(SYSROOT)

CC=$(CROSS_COMPILE)gcc

INCS =
CFLAGS = -Wall -O0 -g 
CFLAGS += $(INCS) 

LDFLAGS = -Wall -O2
LDLIBS = -Lc:/utility/strawberry-perl/c/i686-w64-mingw32/lib
LDLIBS += -lpthread

PGM=main
SRC=$(PGM).c parser.c ringbuffer.c
OBJS=$(SRC:%.c=%.o)

all: $(PGM).exe

.c.o :
	$(CC) -c $(CFLAGS) -I . $<

$(PGM).exe : $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $(PGM)

clean:
	rm -f $(PGM)
	rm -f $(OBJS)	

	