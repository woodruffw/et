all:
	@echo 'dummy'

nix:
	gcc -o ./et ./src/nix/et_nix.c ./src/gen_nick.c ./src/et_main_loop.c

win:
	@echo 'dummy'

clean:
	rm ./et
