TARGET = MoltnCore

LIBDIR = lib/
INCDIR = include/

HELPERDIR = ../HelperFuncs/
HELPERINC = $(HELPERDIR)include/
HELPERLIB = $(HELPERDIR)lib/

DEV_CFLAGS = -g -fsanitize=address,undefined -fno-omit-frame-pointer
DEV_LDFLAGS = -fsanitize=address,undefined

TSAN_CFLAGS = -g -O1 -fsanitize=thread -fno-omit-frame-pointer
TSAN_LDFLAGS = -fsanitize=thread

PROD_CFLAGS = -O2
PROD_LDFLAGS =

CFLAGS = -MMD -MP -I$(HELPERINC) -I$(INCDIR)
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
$(TARGET): $(INCDIR)MoltnCore.h $(LIBDIR)libMoltnCore.a $(HELPERLIB)libHelper.a  main.o  
	gcc main.o -o $@ $(LDFLAGS) $(LIBDIR)libMoltnCore.a -L$(HELPERLIB) -lHelper -lm

$(HELPERLIB)libHelper.a $(HELPERINC)helper.h:
	$(MAKE) -C $(HELPERDIR)

# Static lib
$(LIBDIR)libMoltnCore.a: core.o threads.o  poll.o | $(LIBDIR)
	ar rs $@ $^

# Compiling
main.o: main.c
	gcc $(CFLAGS)  -I$(INCDIR) -c main.c -o $@

#CORE
core.o: $(INCDIR)core.h core.c $(HELPERINC)helper.h
	gcc $(CFLAGS) -c core.c -o $@

poll.o: poll.c $(INCDIR)poll.h
	gcc $(CFLAGS) -c poll.c -o $@

threads.o: threads.c $(INCDIR)threads.h
	gcc $(CFLAGS) -c threads.c -o $@

$(LIBDIR):
	mkdir -p $(LIBDIR)

# tools
clean:
	rm -f *.o *.d

fclean:
	rm -f $(TARGET) *.o *.d $(LIBDIR)libMoltnCore.a 

# merges .d files into dependency graph
-include *.d
