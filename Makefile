TARGET = MoltnCore

DEV_CFLAGS = -g -fsanitize=address,undefined -fno-omit-frame-pointer
DEV_LDFLAGS = -fsanitize=address,undefined

TSAN_CFLAGS = -g -O1 -fsanitize=thread -fno-omit-frame-pointer
TSAN_LDFLAGS = -fsanitize=thread

PROD_CFLAGS = -O2
PROD_LDFLAGS =

CFLAGS = -MMD -MP
LDFLAGS =

dev: CFLAGS += $(DEV_CFLAGS)
dev: LDFLAGS += $(DEV_LDFLAGS)
dev: $(TARGET)

tsan: CFLAGS += $(TSAN_CFLAGS)
tsan: LDFLAGS += $(TSAN_LDFLAGS)
tsan: $(TARGET)

prod: CFLAGS += $(PROD_CFLAGS)
prod: LDFLAGS += $(PROD_LDFLAGS)
prod: $(TARGET)

# Linking
$(TARGET): MoltnCore.h libMoltnCore.a libHelper.a  main.o  
	gcc main.o -o $@ $(LDFLAGS) libMoltnCore.a libHelper.a -lm

libHelper.a:
	$(MAKE) -C ../FormNetwork/
	cp ../FormNetwork/libHelper.a .

helper.h:
	$(MAKE) -C ../FormNetwork/
	cp ../FormNetwork/helper.h .


# Static lib
libMoltnCore.a: core.o threads.o  poll.o
	ar rs $@ $^

MoltnCore.h: core.o helper.h
	@echo "Generating moltnCore.h"
	@echo "#pragma once" > MoltnCore.h
	@cat  helper.h poll.h threads.h core.h  >> MoltnCore.h

# Compiling
main.o: main.c
	gcc $(CFLAGS) -c main.c -o $@

#CORE
core.o: core.h core.c helper.h
	gcc $(CFLAGS) -c core.c -o $@

poll.o: poll.c poll.h
	gcc $(CFLAGS) -c poll.c -o $@

threads.o: threads.c threads.h
	gcc $(CFLAGS) -c threads.c -o $@

# tools
clean:
	rm -f *.o *.a *.d

fclean:
	rm -f $(TARGET) *.o *.a *.d helper.h MoltnCore.h libMoltnCore.a

fixTerminal:
	stty sane

# merges .d files into dependency graph
-include *.d
