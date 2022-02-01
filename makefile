webserver: src/list.c src/main.c src/meta.c src/robust_io.c src/utils.c
	gcc -g -I./include src/list.c src/main.c src/meta.c src/robust_io.c src/utils.c -o webserver