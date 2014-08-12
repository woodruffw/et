CC=gcc
CFLAGS=-o ./bin/et -Os
LDFLAGS_WIN=-lws2_32 -lmswsock -ladvapi32 -mwindows

all:
	@echo '========================================================='
	@echo 'Select a target for your Operating System.'
	@echo 'Options: win (Windows), nix (Unix/Linux)'
	@echo '========================================================='

nix:
	$(CC) $(CFLAGS) ./src/nix/et_nix.c ./src/nix/et_main_loop_nix.c ./src/gen_nick.c 

win:
	$(CC) $(CFLAGS) ./src/win/et_win.c ./src/win/et_main_loop_win.c ./src/gen_nick.c $(LDFLAGS_WIN)

clean:
	rm ./bin/et
