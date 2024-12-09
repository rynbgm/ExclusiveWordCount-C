bst_dir = ../bst/
holdall_dir = ../holdall/
hashtable_dir = ../hashtable
CC = gcc
CFLAGS = -std=c2x \
  -Wall -Wconversion -Werror -Wextra -Wpedantic -Wwrite-strings \
  -O2\
  -I$(bst_dir) -I$(holdall_dir) -I$(hashtable_dir) \
  -DHOLDALL_PUT_TAIL=1 \
  -DHASHTABLE_STATS=1
vpath %.c $(bst_dir) $(holdall_dir) $(hashtable_dir)
vpath %.h $(bst_dir) $(holdall_dir) $(hashtable_dir)
objects = main.o bst.o holdall.o hashtable.o
executable = xwc2
makefile_indicator = .\#makefile\#

.PHONY: all clean

all: $(executable)

clean:
	$(RM) $(objects) $(executable)
	@$(RM) $(makefile_indicator)

$(executable): $(objects)
	$(CC) $(objects) -o $(executable)

main.o: main.c bst.h holdall.h hashtable.h
bst.o: bst.c bst.h
holdall.o: holdall.c holdall.h
hashtable.o: hashtable.c hashtable.h

include $(makefile_indicator)

$(makefile_indicator): makefile
	@touch $@
	@$(RM) $(objects) $(executable)
