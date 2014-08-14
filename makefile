CC=gcc
CCFLAGS=-o ./bin/et -Os -ffunction-sections 
CCFLAGS_WIN=
CCFLAGS_NIX=-fdata-sections
LDFLAGS=
LDFLAGS_WIN=-s -lws2_32 -mwindows
LDFLAGS_NIX=

all:
	@echo '========================================================='
	@echo 'Select a target for your Operating System.'
	@echo 'Options: win (Windows), nix (Unix/Linux)'
	@echo '========================================================='

nix:
	$(CC) $(CCFLAGS) $(CCFLAGS_NIX) ./src/nix/et_nix.c ./src/nix/et_main_loop_nix.c ./src/gen_nick.c $(LDFLAGS) $(LDFLAGS_NIX)

win:
	$(CC) $(CCFLAGS) $(CCFLAGS_WIN) ./src/win/et_win.c ./src/win/et_main_loop_win.c ./src/gen_nick.c $(LDFLAGS) $(LDFLAGS_WIN)

clean:
	rm ./bin/et
