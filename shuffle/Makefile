##----------------------------------------------------------------------------
# Created with genmake.pl v1.1 on Wed Feb 10 09:00:35 2016
# genmake.pl home: http://muquit.com/muquit/software/
# Copryright: GNU GPL (http://www.gnu.org/copyleft/gpl.html)
##----------------------------------------------------------------------------
# Build win32 commandline app
# gcc is from strawberry perl
#

rm = rm -rf
MKDIR = mkdir -p 
CC= gcc
DEFS =
PROGNAME= main.exe
INCLUDES=  -I.
LIBS=


DEFINES= $(INCLUDES) $(DEFS)
CFLAGS= -g $(DEFINES) -ffunction-sections -fdata-sections

LDFLAGS = -Wl,--gc-sections

SRC_DIR = src
SRCS = $(SRC_DIR)/main.c

OBJDIR = obj
OBJS = $(patsubst %.o, $(OBJDIR)/%.o,$(SRCS:.c=.o)) 

$(OBJDIR)/%.o: %.c
	-@$(MKDIR) $(dir $@)
#	$(CC) $(CFLAGS) -c $*.c
	$(CC) $(CFLAGS) $< -c -o $@

all: $(PROGNAME)

$(PROGNAME) : $(OBJS)
	$(CC) $(LDFLAGS) -o $(PROGNAME) $(OBJS) $(LIBS)

clean:
	-$(rm) $(OBJS) $(PROGNAME) core *~
	-$(rm) $(OBJDIR)

	
	
	