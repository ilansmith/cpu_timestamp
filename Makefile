CC=gcc
CFLAGS=-Wall -Werror -m64
LDFLAGS=-pthread
APP=rdtsc

OBJS=rdtsc.o

ifeq ($(CONFIG_DEBUG),y)
    CFLAGS+=-g -O0
else
    CFLAGS+=-O3
endif

o.%: %.c
	$(GCC) -o $@ $(CFLAGS) -c $<

.PHONY: all clean cleanall

all: $(APP)

$(APP): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

clean:
	@echo "removing executables"
	@rm -f $(APP)
	@echo "removing object files"
	@rm -f *.o

cleanall: clean
	@echo "removing pre compilation files"
	@rm -f *_pre.c
	@echo "removing tag file"
	@rm -f tags

