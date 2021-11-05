cJSON-bench:    json-shootout.c
	gcc json-shootout.c -lcjson -o cJSON-bench

sxe-jitson-bench.o:    json-shootout.c
	gcc -DSXE_JITSON=1 -I../sxe/libsxe/build-linux-64-release/include -c json-shootout.c -o sxe-jitson-bench.o

sxe-jitson-bench:    sxe-jitson-bench.o
	gcc sxe-jitson-bench.o ../sxe/libsxe/build-linux-64-release/libsxe.a -o sxe-jitson-bench
