CPPFLAGS+=-Iinclude -Ilogging
LDFLAGS+=-Llib
CFLAGS+=-O0 -g -Wall -Wextra

AEBP=bin/ae-blocking-parser
AECC=bin/aecc

INST=lib include bin
include sxm/Makefile.vars
include server/Makefile.vars
include sos/Makefile.vars

all: $(INST) $(SERVER_SERVER) $(SOS_TESTS)

%.o: %.ae
	$(AECC) $< -o $@ -- -c $(CPPFLAGS) $(CFLAGS) -Wno-unused-label \
		-Wno-unused-parameter

include sxm/Makefile.rules
include server/Makefile.rules
include sos/Makefile.rules

$(INST):
	mkdir -p $(INST)

clean:
	make sxm_clean
	make server_clean
	make sos_clean

.PHONY: all prereqs clean sxm_clean
