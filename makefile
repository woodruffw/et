CC=gcc
CFLAGS=-o ./bin/et -Os -ffunction-sections 
CFLAGS_WIN=-s
CFLAGS_NIX=-fdata-sections
LDFLAGS=
LDFLAGS_WIN=-lws2_32 -lmswsock -ladvapi32 -mwindows
LDFLAGS_NIX=

all:
	@echo '========================================================='
	@echo 'Select a target for your Operating System.'
	@echo 'Options: win (Windows), nix (Unix/Linux)'
	@echo '========================================================='

nix:
	$(CC) $(CFLAGS) $(CFLAGS_NIX) ./src/nix/et_nix.c ./src/nix/et_main_loop_nix.c ./src/gen_nick.c $(LDFLAGS) $(LDFLAGS_NIX)

win:
	$(CC) $(CFLAGS) $(CFLAGS_WIN) ./src/win/et_win.c ./src/win/et_main_loop_win.c ./src/gen_nick.c $(LDFLAGS) $(LDFLAGS_WIN)

clean:
	rm ./bin/et
