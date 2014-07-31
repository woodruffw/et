all:
	@echo 'dummy'

nix:
	gcc -o ./et ./src/nix/et_nix.c ./src/nix/et_main_loop.c ./src/gen_nick.c 

win:
	@echo 'dummy'

clean:
	rm ./et
