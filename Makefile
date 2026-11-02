all:
	cc -O3 -march=native -flto -pipe src/main.c -o exe -lm
