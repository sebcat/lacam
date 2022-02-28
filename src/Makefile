# Temporary makefile to get non-android parts up and running
SRCS := lacam.cc lua-5.4.3/src/lapi.c lua-5.4.3/src/lauxlib.c \
	lua-5.4.3/src/lbaselib.c lua-5.4.3/src/lcode.c \
	lua-5.4.3/src/lcorolib.c lua-5.4.3/src/lctype.c \
	lua-5.4.3/src/ldblib.c lua-5.4.3/src/ldebug.c \
	lua-5.4.3/src/ldo.c lua-5.4.3/src/ldump.c lua-5.4.3/src/lfunc.c \
	lua-5.4.3/src/lgc.c lua-5.4.3/src/linit.c lua-5.4.3/src/liolib.c \
	lua-5.4.3/src/llex.c lua-5.4.3/src/lmathlib.c \
	lua-5.4.3/src/lmem.c lua-5.4.3/src/loadlib.c \
	lua-5.4.3/src/lobject.c lua-5.4.3/src/lopcodes.c \
	lua-5.4.3/src/loslib.c lua-5.4.3/src/lparser.c \
	lua-5.4.3/src/lstate.c lua-5.4.3/src/lstring.c \
	lua-5.4.3/src/lstrlib.c lua-5.4.3/src/ltable.c \
	lua-5.4.3/src/ltablib.c lua-5.4.3/src/ltm.c lua-5.4.3/src/lua.c \
	lua-5.4.3/src/lundump.c lua-5.4.3/src/lutf8lib.c \
	lua-5.4.3/src/lvm.c lua-5.4.3/src/lzio.c

OBJS := $(SRCS:.c=.o)
CFLAGS := -Ilua-5.4.3/src/
LDFLAGS := -lm
RM ?= rm -f

.PHONY: clean

all: lacam

lacam: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:
	$(RM) lua-5.4.3/src/*.o lacam
