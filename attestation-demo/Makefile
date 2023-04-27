VPATH = $(SRCDIR)
CC := aarch64-linux-gnu-gcc-9

CFLAGS = --static -fPIC
CFLAGS += -mstrict-align
CFLAGS += -Iqcbor/inc 
SRC = call.c call.h parser.c parser.h common.h
QCBOR = qcbor/libqcbor.a

all: call

call: $(SRC) $(QCBOR)
	$(CC) $(CFLAGS) $^ -o call	

$(QCBOR):
	make -C qcbor libqcbor.a

client: client.c common.h  parser.c parser.h $(QCBOR)
	$(CC) $(CFLAGS) $^ -o client

server: server.c attest_user.h attest_user.c common.h
	$(CC) $(CFLAGS) $^ -o server

clean:
	@rm -rf call client server
	@make -C qcbor clean

